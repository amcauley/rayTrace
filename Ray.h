#pragma once
#ifndef RAY_H
#define RAY_H

#include "Vec3.h"

class Ray {
public:
  /* Ray location. */
  Vec3 loc3;
  /* Direction vector. */
  Vec3 vec3;

  Ray() {};
  Ray(Vec3& pos, Vec3& dir);
};

#endif //RAY_H