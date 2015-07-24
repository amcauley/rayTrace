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
  /* Count the depth of this ray, e.x original ray is depth 0, reflected ray is depth 1, reflected of
     that one is depth 2, etc. */
  int depth;

  Ray() {};
  Ray(Vec3& pos, Vec3& dir, int depth);
};

#endif //RAY_H