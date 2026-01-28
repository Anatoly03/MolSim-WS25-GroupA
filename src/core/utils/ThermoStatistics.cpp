#include "ThermoStatistics.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "spdlog/spdlog.h"

ThermoStatistics::ThermoStatistics(int statsEvery, double dr, int rdfBins, std::string outputPrefix)
    : statsEvery_(statsEvery), dr_(dr), rdfBins_(rdfBins), outputPrefix_(std::move(outputPrefix)) {}

void ThermoStatistics::init(const ParticleContainer &ps) {
    const int N = ps.particleCount();
    refPos_.resize(N);
    for (int i = 0; i < N; ++i) {
        refPos_[i] = ps[i].position;
    }
}

void ThermoStatistics::maybeMeasure(int step, const ParticleContainer &ps, const Domain &dom) {
    if (statsEvery_ <= 0) return;
    if (step % statsEvery_ != 0) return;
    if (refPos_.size() != static_cast<size_t>(ps.particleCount())) init(ps);

    const double var = computeDiffusionVar(ps, dom);
    const std::vector<double> rdf = computeRDF(ps, dom);

    writeToCSV(step, var, rdf);

    // update reference positions
    const int N = ps.particleCount();
    for (int i = 0; i < N; ++i) {
        refPos_[i] = ps[i].position;
    }
}

double ThermoStatistics::computeDiffusionVar(const ParticleContainer &ps, const Domain &dom) const {
    const int N = ps.particleCount();
    if (N <= 0) return 0.0;

    double sum = 0.0;
    for (int i = 0; i < N; ++i) {
        Vec3D d = periodicDisplacement(ps[i].position, refPos_[i], dom);
        sum += d.dot(d);
    }
    return sum / static_cast<double>(N);
}

std::vector<double> ThermoStatistics::computeRDF(const ParticleContainer &ps, const Domain &dom) const {
    const int N = ps.particleCount();
    std::vector<long long> counts(rdfBins_, 0);

    Vec3D L = dom.size();
    const double rMax = 0.5 * std::min(L.x, std::min(L.y, L.z));

    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            Vec3D rij = periodicDisplacement(ps[j].position, ps[i].position, dom);
            const double r = rij.length();
            if (r >= rMax) continue;

            const int bin = static_cast<int>(r / dr_);
            if (0 <= bin && bin < rdfBins_) {
                counts[bin] += 1;
            }
        }
    }

    std::vector<double> g(rdfBins_, 0.0);
    const double V = dom.volume();
    const double rho = (V > 0.0) ? (static_cast<double>(N) / V) : 0.0;

    constexpr double kPi = 3.14159265358979323846;
    for (int b = 0; b < rdfBins_; ++b) {
        const double r0 = b * dr_;
        const double r1 = r0 + dr_;
        const double shellVol = (4.0 * kPi / 3.0) * (r1 * r1 * r1 - r0 * r0 * r0);
        if (shellVol <= 0.0 || rho <= 0.0 || N <= 1) continue;

        const double neighborsPerParticle = (2.0 * static_cast<double>(counts[b])) / static_cast<double>(N);
        const double localDensity = neighborsPerParticle / shellVol;
        g[b] = localDensity / rho;
    }
    return g;
}

void ThermoStatistics::writeToCSV(int step, double var, const std::vector<double> &rdf) const {
    if (outputPrefix_.empty()) {
        spdlog::warn("ThermoStatistics: output prefix is empty, skipping CSV write.");
        return;
    }

    // diffusion.csv: step,var
    {
        std::ofstream diffFile;
        diffFile.open(outputPrefix_ + "_diffusion.csv", std::ios::app);
        if (!diffFile) {
            spdlog::warn("ThermoStatistics: could not open diffusion output file");
        } else {
            diffFile << step << "," << std::setprecision(16) << var << "\n";
        }
    }

    // rdf_stepXXXX.csv: r,g(r)
    {
        std::ostringstream name;
        name << outputPrefix_ << "_rdf_" << std::setfill('0') << std::setw(6) << step << ".csv";
        std::ofstream rdfFile(name.str());
        if (!rdfFile) {
            spdlog::warn("ThermoStatistics: could not open RDF output file");
        } else {
            for (int b = 0; b < static_cast<int>(rdf.size()); ++b) {
                const double r = (b + 0.5) * dr_;
                rdfFile << std::setprecision(16) << r << "," << rdf[b] << "\n";
            }
        }
    }
}
