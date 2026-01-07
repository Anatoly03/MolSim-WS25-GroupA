
#include <cmath>
#include <iostream>
#include <list>
#include <chrono>

#include "FileReader.h"
#include "Frame.h"
#include "ParticleContainer.h"
#include "utils/ArrayUtils.h"

#ifdef ENABLE_VTK_OUTPUT
#include "outputWriter/VTKWriter.h"
#else
#include "outputWriter/XYZWriter.h"
#endif

#include "outputWriter/CheckpointWriter.h"

/**
 * using ParticleContainer instead of std::list<Particle> particles
 */
static ParticleContainer particles;
// global gravity acceleration used by calculateForce (set from Args.ggrav in main)
static double gravity_g = 0.0;
// Lennard-Jones parameters
static double lj_sigma = 1.2;
static double lj_epsilon = 1.0;
static double lj_rcut = 2.5 * 1.2;
// domain and thermostat global state (set from args in main)
static double domain_Lx = 303.0;
static double domain_Ly = 180.0;
static bool domain_periodic_lr = false;
static int thermostat_N = 1000;
static double thermostat_T = 0.5;
static const double kB = 1.0;

// helper: apply boundary conditions to a particle after its position has been updated
static void applyBoundaries(Particle &p) {
    // make local copies (explicit copy ctor used)
    Vec3D pos = p.getPosition();
    Vec3D vel = p.getVelocity();

    // X-direction
    if (domain_periodic_lr) {
        // wrap x into [0, Lx)
        double x = pos.x;
        // fmod behavior for negative numbers
        x = std::fmod(x, domain_Lx);
        if (x < 0) x += domain_Lx;
        pos.x = x;
    } else {
        // Reflective boundary - handle multiple bounces
        while (pos.x < 0 || pos.x > domain_Lx) {
            if (pos.x < 0) {
                pos.x = -pos.x;
                vel.x = -vel.x;
            }
            if (pos.x > domain_Lx) {
                pos.x = 2 * domain_Lx - pos.x;
                vel.x = -vel.x;
            }
        }
    }

    // Y-direction: always reflective - handle multiple bounces
    while (pos.y < 0 || pos.y > domain_Ly) {
        if (pos.y < 0) {
            pos.y = -pos.y;
            vel.y = -vel.y;
        }
        if (pos.y > domain_Ly) {
            pos.y = 2 * domain_Ly - pos.y;
            vel.y = -vel.y;
        }
    }

    p.setPosition(pos);
    p.setVelocity(vel);
}

// helper: apply thermostat scaling to particle velocities
static void applyThermostat(ParticleContainer &particles) {
    const auto N = particles.size();
    if (N == 0) return;

    // sum m * (vx^2 + vy^2)
    double sum_mv2 = 0.0;
    particles.forEach([&sum_mv2](Particle &p) {
        const auto &v = p.getVelocity();
        sum_mv2 += p.getMass() * (v.x * v.x + v.y * v.y);
    });

    if (sum_mv2 == 0.0) return;

    double Ndof = 2.0 * static_cast<double>(N);
    double target_sum_mv2 = Ndof * kB * thermostat_T;
    double scale = std::sqrt(target_sum_mv2 / sum_mv2);

    particles.forEach([scale](Particle &p) {
        const auto &v = p.getVelocity();
        p.setVelocity(Vec3D(v.x * scale, v.y * scale, v.z * scale));
    });

    std::cout << "Thermostat applied: scale=" << scale << " targetT=" << thermostat_T << std::endl;
}

/**** forward declaration of the calculation functions ****/

/**
 * calculate the force for all particles
 */
void calculateForce();

/**
 * calculate the position for all particles
 */
void calculatePosition(double dt);

/**
 * calculate the position for all particles
 */
void calculateVelocity(double dt);

/**
 * plot the particles to a xyz-file
 */
void plotParticles(int iteration, const char *output_path);

/**
 * @brief Add a disc of particles centered at (cx,cy) with radius r
 */
void addDisc(ParticleContainer &particles, double cx, double cy, double r);

/**
 * @brief The program entry point is the Rahmenprogramm which after getting all
 * variables calls the molecular simulation methods.
 */
int main(int argc, char *argsv[]) {
    const auto args = ProcessArgs(argc, argsv);

    // set global gravity
    gravity_g = args.ggrav;
    if (gravity_g != 0.0) {
        std::cout << "Gravity enabled: g = " << gravity_g << std::endl;
    }

    // set domain and thermostat parameters
    domain_Lx = args.Lx;
    domain_Ly = args.Ly;
    domain_periodic_lr = args.periodic_lr;
    thermostat_N = args.nthermostat;
    thermostat_T = args.Tinit;

    // set Lennard-Jones parameters
    lj_sigma = args.sigma;
    lj_epsilon = args.epsilon;
    lj_rcut = args.rcut;

    std::cout << "Domain Lx=" << domain_Lx << " Ly=" << domain_Ly << " periodic_lr=" << domain_periodic_lr << std::endl;
    std::cout << "Thermostat: N=" << thermostat_N << " T=" << thermostat_T << std::endl;
    std::cout << "LJ params: sigma=" << lj_sigma << " epsilon=" << lj_epsilon << " rcut=" << lj_rcut << std::endl;

    // allow FileReader to set current_time from checkpoint header if present
    double current_time = args.start_time;
    FileReader fileReader;
    fileReader.readFile(particles, args.input_file, &current_time);

    // if requested, add a disc on top of the loaded system (useful for restart + drop)
    if (args.disc_radius >= 0) {
        addDisc(particles, args.disc_center_x, args.disc_center_y, args.disc_radius);
        std::cout << "Added disc: center=(" << args.disc_center_x << "," << args.disc_center_y << ") radius=" << args.disc_radius << "\n";
    }

    int iteration = 0;

    // Compute initial forces before the main loop
    // (needed for correct position integration in the first iteration)
    calculateForce();

    // initial plot (skip if no_io)
    if (!args.no_io) {
        plotParticles(iteration, args.output_path);
    } else {
        std::cout << "I/O disabled (--no-io). Running without plotting/checkpointing for benchmarking." << std::endl;
    }

    // checkpoint writer and state
    outputWriter::CheckpointWriter checkpointWriter;
    bool checkpoint_written = false;

    // start timing the main simulation loop
    auto t_start = std::chrono::steady_clock::now();

    // for this loop, we assume: current x, current f and current v are known
    while (current_time < args.end_time) {
        calculatePosition(args.delta_t);
        calculateForce();
        calculateVelocity(args.delta_t);

        iteration++;
        if (!args.no_io && (iteration % 10 == 0)) {
            plotParticles(iteration, args.output_path);
        }
        if (!args.no_io && (iteration % 100 == 0)) {
            std::cout << "Iteration " << iteration << " finished." << std::endl;
        }

        // advance time
        current_time += args.delta_t;

        // if checkpointing is requested and not yet written, check time and write
        if (!args.no_io && !checkpoint_written && args.checkpoint_path != nullptr && args.checkpoint_time >= 0.0) {
            if (current_time >= args.checkpoint_time) {
                checkpointWriter.writeCheckpoint(particles, std::string(args.checkpoint_path), current_time);
                checkpoint_written = true;
            }
        }

        // apply thermostat at requested intervals (if nthermostat > 0)
        if (thermostat_N > 0 && (iteration % thermostat_N) == 0) {
            applyThermostat(particles);
        }
    }

    // stop timing and report
    auto t_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = t_end - t_start;
    double seconds = elapsed.count();
    const auto N = particles.size();
    double mups = 0.0;
    if (seconds > 0.0) {
        mups = (static_cast<double>(N) * static_cast<double>(iteration)) / seconds / 1e6;
    }

    std::cout << "Simulation loop time: " << seconds << " s\n";
    std::cout << "Iterations: " << iteration << " Particles: " << N << " MUPS: " << mups << "\n";

    
    std::cout << "output written. Terminating..." << std::endl;
    return 0;
}

void addDisc(ParticleContainer &particles, double cx, double cy, double r) {
    // simple grid fill with spacing ~ 1.2 (sigma)
    const double spacing = 1.2;
    const double r2 = r * r;
    for (double x = cx - r + spacing / 2.0; x <= cx + r; x += spacing) {
        for (double y = cy - r + spacing / 2.0; y <= cy + r; y += spacing) {
            double dx = x - cx;
            double dy = y - cy;
            if (dx * dx + dy * dy <= r2) {
                Vec3D pos(x, y, 0.0);
                Vec3D vel(0.0, 0.0, 0.0);
                double mass = 1.0;
                int type = 1;  // use type 1 for disc/drop particles
                particles.emplace_back(pos, vel, mass, type);
            }
        }
    }
}

void calculateForce() {
    // First: save current forces to old_force and reset current force
    particles.forEach([](Particle &p) {
        p.delayForce();
    });

    // Second: compute all pairwise LJ forces
    particles.forEachPair([](Particle &p1, Particle &p2) {
        Vec3D diffX = p2.getPosition() - p1.getPosition();
        double distance = diffX.length();
        
        // Skip if particles are at same position or beyond cutoff
        if (distance < 1e-10 || distance > lj_rcut) return;

        // Lennard-Jones interaction with cutoff
        double inv_r = 1.0 / distance;
        double s_over_r = lj_sigma * inv_r;
        double s_over_r2 = s_over_r * s_over_r;
        double s6 = s_over_r2 * s_over_r2 * s_over_r2; // (sigma/r)^6
        double s12 = s6 * s6;
        // force magnitude along unit vector (from p1 to p2)
        double fmag = 24.0 * lj_epsilon * (2.0 * s12 - s6) * inv_r;
        
        // Clamp force magnitude to prevent explosions
        if (std::abs(fmag) > 1e6) {
            // Silent clamping - warning output can slow down performance significantly
            fmag = std::copysign(1e6, fmag);
        }
        
        Vec3D unit = diffX * inv_r;
        Vec3D force12 = unit * fmag;

        // Newton's third law: equal and opposite forces
        p1.setForce(p1.getForce() + force12);
        p2.setForce(p2.getForce() - force12);
    });

    // Third: add external gravity to all particles
    particles.forEach([](Particle &p) {
        Vec3D gravityForce(0.0, p.getMass() * gravity_g, 0.0);
        p.setForce(p.getForce() + gravityForce);
    });
}

void calculatePosition(const double dt) {
    particles.forEach([dt](Particle &particle) {
        Vec3D x = particle.getPosition() + dt * particle.getVelocity() +
                  std::pow(dt, 2) * particle.getForce() / (2 * particle.getMass());
        particle.setPosition(x);
        // apply boundary conditions (may modify velocity too)
        applyBoundaries(particle);
    });
}

void calculateVelocity(const double dt) {
    particles.forEach([dt](Particle &particle) {
        Vec3D v =
            particle.getVelocity() + dt * ((particle.getForce() + particle.getOldForce()) / (2 * particle.getMass()));
        particle.setVelocity(v);
    });
}

void plotParticles(int iteration, const char *output_path) {
    std::string out_name(output_path);
#ifdef ENABLE_VTK_OUTPUT
    outputWriter::VTKWriter writerVTK;
    writerVTK.plotParticles(particles, out_name, iteration);
#else
    outputWriter::XYZWriter writer;
    writer.plotParticles(particles, out_name, iteration);
#endif
}
