#pragma once

#include <string>
#include <vector>

#include "../math/Vec3.h"
#include "../ParticleContainer.h"

struct Domain {
    Vec3D min;
    Vec3D max;
    bool periodicX{true};
    bool periodicY{true};
    bool periodicZ{true};

    Vec3D size() const { return max - min; }
    double volume() const {
        Vec3D L = size();
        return L.x * L.y * L.z;
    }
};

// minimum-image displacement for periodic boundaries
inline Vec3D periodicDisplacement(const Vec3D &x_now, const Vec3D &x_ref, const Domain &dom) {
    Vec3D d = x_now - x_ref;
    Vec3D L = dom.size();

    if (dom.periodicX) {
        if (d.x > 0.5 * L.x) d.x -= L.x;
        if (d.x < -0.5 * L.x) d.x += L.x;
    }
    if (dom.periodicY) {
        if (d.y > 0.5 * L.y) d.y -= L.y;
        if (d.y < -0.5 * L.y) d.y += L.y;
    }
    if (dom.periodicZ) {
        if (d.z > 0.5 * L.z) d.z -= L.z;
        if (d.z < -0.5 * L.z) d.z += L.z;
    }
    return d;
}

class ThermoStatistics {
   public:
    ThermoStatistics(int statsEvery, double dr, int rdfBins, std::string outputPrefix);

    void init(const ParticleContainer &ps);

    // call each step; it will compute only every statsEvery_ steps
    void maybeMeasure(int step, const ParticleContainer &ps, const Domain &dom);

   private:
    double computeDiffusionVar(const ParticleContainer &ps, const Domain &dom) const;
    std::vector<double> computeRDF(const ParticleContainer &ps, const Domain &dom) const;

    void writeToCSV(int step, double var, const std::vector<double> &rdf) const;

   private:
    int statsEvery_{1000};
    double dr_{0.1};
    int rdfBins_{200};
    std::string outputPrefix_;
    std::vector<Vec3D> refPos_;
};
