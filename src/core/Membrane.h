
#pragma once

#include <functional>
#include <vector>

#include "Particle.h"
//#include "ParticleContainer.h"
#include "math/Vec3.h"
#include <cmath>
#include <limits>
#include "./utils/Args.h"
class ParticleContainer;

class Membrane {
   private:
    // two dimensional sheet of particles
    const Args arguments;


public:
    std::vector<std::vector<int>> particles;

    /**
     * @brief Typedef for particle getter function
     */
    typedef std::function<Particle&(int)> get_particle;

    /**
     * @brief Standard stiffness of the particles in the membrane.
     */
    double stiffness;

    /**
     * @brief Standard bond length of the particles in the membrane.
     */
    double bond_length;

    //ParticleContainer particleContainer;

   public:

    /**
     * @brief Constructor specifying particle getter.
     */
    /*Membrane(ParticleContainer particleContainer1, int width, int height) : particleContainer(particleContainer1) {
        for (auto x = 0; x < width; x++) {
            particles.emplace_back(height);
        }
    }*/

    Membrane(get_particle getter, int width, int height, const Args &args) :arguments(args), particleGetter(getter) {
        for (auto x = 0; x < width; x++) {
            particles.emplace_back(height);
        }
    }

    /**
     * @brief Membrane destructor.
     */
    ~Membrane() = default;

    /**
     * @brief Update forces for all particles in the membrane.
     * Iterates over all particles and calculates forces from direct and diagonal neighbors.
     */
    void updateForce() {


        /*for (size_t i = 0; i < particles.size(); ++i) {
            for (size_t j = 0; j < particles[0].size(); ++j) {
                int idx = particles[i][j];
                Particle& pTest = particleGetter(idx);
                std::cout << "Particle index/id: " << idx << " has " << pTest.neighborParticles.size() << " neighbors\n";
                for (int nIndex : pTest.neighborParticles) {
                    std::cout << " Neighbor index/id: " << nIndex << "\n";
                }
                std::cout<<std::endl;
            }
        }*/



        size_t width = particles.size();
        //applyLJForce();
        for (size_t x = 1; x < width; ++x) {
            for (size_t y = 1; y < particles[x].size(); ++y) {
                int par = particles[x][y];

                /*updateForcesDirectNeighbors(par, particles[x-1][y]);
                updateForcesDirectNeighbors(par, particles[x][y-1]);
                updateForcesDiagonalNeighbors(par, particles[x-1][y-1]);

                if (x + 1 < width) {
                    updateForcesDirectNeighbors(par, particles[x+1][y]);
                }*/

                if (x > 0) updateForcesDirectNeighbors(par, particles[x-1][y]);
                if (y > 0) updateForcesDirectNeighbors(par, particles[x][y-1]);
                if (x > 0 && y > 0) updateForcesDiagonalNeighbors(par, particles[x-1][y-1]);
                if (x + 1 < width && y > 0) updateForcesDiagonalNeighbors(par, particles[x+1][y-1]);


            }
        }
    }


    void updateZUPForce(double amount){
        if(particles.size()>=18&&particles[0].size()>=25){
            Particle particle1 = particleGetter(particles[17][24]);
            Particle particle2 = particleGetter(particles[17][25]);
            Particle particle3 = particleGetter(particles[18][24]);
            Particle particle4 = particleGetter(particles[18][25]);
            particle1.force.y += amount;
            particle2.force.y += amount;
            particle3.force.y += amount;
            particle4.force.y += amount;


        }


    }
   private:
    get_particle particleGetter;




    void applyLJForce() {
        size_t width = particles.size();
        size_t height = particles[0].size();


        for (size_t x1 = 0; x1 < width; ++x1) {
            for (size_t y1 = 0; y1 < height; ++y1) {

                int i = particles[x1][y1];
                Particle &pi = particleGetter(i);

                for (size_t x2 = x1; x2 < width; ++x2) {
                    for (size_t y2 = (x2 == x1 ? y1 + 1 : 0); y2 < height; ++y2) {

                        int j = particles[x2][y2];
                        Particle &pj = particleGetter(j);


                        Vec3D dist = pi.position - pj.position;

                        double r1 = dist.length();
                        if (r1 > arguments.cutoff_radius) continue;  // cut off for performance
                        double r2 = dist.length2();
                        if (r2 == 0.0) continue; // cut in to avoid high values

                        double averagedSigma = (pi.sigma + pj.sigma) / 2;
                        double rootedEpsilon = std::sqrt(pi.epsilon * pj.epsilon);


                        double min = (pow(2,1/6)) * averagedSigma;
                        //double a = 0. * averagedSigma;
                        if (r1 < min) {
                            r2 = min * min;
                            //r2=r1*r1;
                        }
                        //r2 = r2 + a * a;

                        double inv_r2 = 1.0 / r2;               // (xi -xj)
                        //std::cout<<"trigger inv_r2 "<<inv_r2<<std::endl;
                        double sr2 = std::pow(averagedSigma, 2) * inv_r2;  // (sigma / (xi -xj))^2
                        //std::cout<<"trigger sr2 "<<sr2<<std::endl;

                        //std::cout<<"trigger sr2 "<<sr2<<std::endl;
                        double sr6 = sr2 * sr2 * sr2;           // (sigma / (xi -xj))^6
                        //std::cout<<"trigger sr6 "<<sr6<<std::endl;

                        double sr12 = sr6 * sr6;                // (sigma / (xi -xj))^12
                        //std::cout<<"trigger sr12 "<<sr12<<std::endl;


                        double scalar = 24.0 * rootedEpsilon * inv_r2 * (2.0 * sr12 - sr6);
                        //std::cout<<"trigger scalar "<<scalar<<std::endl;

                        /*double Fmax = 50.0; // try 50, then adjust up/down
                         if (scalar > Fmax) scalar = Fmax;
                         if (scalar < -Fmax) scalar = -Fmax;*/

                        Vec3 F = scalar * dist.normal();


                        pi.force += F;
                            pj.force -= F;
                        }
                    }
                }
            }
        }


    /**
     * @brief Calculate forces between particle and its direct neighbors using harmonic potential.
     * Harmonic potential: U(xi, xj) = k/2 * (||xi - xj||2 - r0)^2
     * Force: F = -2 * k * (||xi - xj||2 - r0) * (xi - xj)
     * 
     * @param i Particle 1 index
     * @param j Particle 2 index
     */
    void updateForcesDirectNeighbors(size_t i, size_t j) {
        // stiffness constant
        double k = stiffness;

        // average bond length
        double r0 = bond_length;

        Particle& p_i = particleGetter(i);
        Particle& p_j = particleGetter(j);

        Vec3 delta = p_i.position - p_j.position;
        double dist = delta.length2();
        
        // Force magnitude: -2 * k * (distance - r0)
        double forceMagnitude = -2.0 * k * (dist - r0);
        
        // Force direction: normalized displacement vector
        Vec3 force = delta * (forceMagnitude / dist);
        
        // Apply Newton's third law: forces are equal and opposite
        p_i.force += force;
        p_j.force += -force;
    }

    /**
     * @brief Calculate forces between particle and its diagonal neighbors.
     * 
     * @param i Particle 1
     * @param j Particle 2
     */
    void updateForcesDiagonalNeighbors(size_t i, size_t j) {
        // TODO: Implementation

        // stiffness constant
        double k = stiffness;

        // average bond length
        double r0 = bond_length;

        Particle& p_i = particleGetter(i);
        Particle& p_j = particleGetter(j);
        Vec3 d = p_j.position - p_i.position;
        double dist2 = d.length2();

        if (dist2 == 0.0) { return; }

        double rest_term = std::sqrt(2.0) * r0;

        double factor = k * (dist2 - rest_term) / dist2;

        Vec3 F = d * factor;


        p_i.force += F;
        p_j.force += -(F);


    }
};
