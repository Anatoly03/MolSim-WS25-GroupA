/*
 * Particle.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include <array>
#include <string>
#include "math/Vec3.h"

class Particle {
 private:
  /**
   * Position of the particle
   */
  Vec3D x;

  /**
   * Velocity of the particle
   */
  Vec3D v;

  /**
   * Force effective on this particle
   */
  Vec3D f;

  /**
   * Force which was effective on this particle
   */
  Vec3D old_f;

  /**
   * Mass of this particle
   */
  double m;

  /**
   * Type of the particle. Use it for whatever you want (e.g. to separate
   * molecules belonging to different bodies, matters, and so on)
   */
  int type;

 public:
  explicit Particle(int type = 0);

  Particle(const Particle &other);

  Particle(
      // for visualization, we need always 3 coordinates
      // -> in case of 2d, we use only the first and the second
      Vec3D x_arg, Vec3D v_arg, double m_arg, int type = 0);

  virtual ~Particle();

  const Vec3D &getX() const;

  const Vec3D &getV() const;

  const Vec3D &getF() const;

  void setX(Vec3D &x_);

  void setV(Vec3D &v_);

  void setF(Vec3D &f_);

  void delayF();

  const Vec3D &getOldF() const;

  double getM() const;

  int getType() const;

  bool operator==(Particle &other);

  std::string toString() const;
};

std::ostream &operator<<(std::ostream &stream, Particle &p);
