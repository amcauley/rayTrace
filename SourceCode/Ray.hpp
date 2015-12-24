#pragma once
#ifndef RAY_H
#define RAY_H

/* Forward class definitions. */
class Object;

#include "Vec3.hpp"

class Ray {
public:
  /* Ray location. */
  Vec3 loc3;
  /* Direction vector. */
  Vec3 vec3;
  /* Count the depth of this ray, e.x original ray is depth 0, reflected ray is depth 1, reflected of
     that one is depth 2, etc. */
  unsigned int depth;
  /* Distance (squared) traveled from last location to current one. */
  float dist2;

#ifdef EN_PARALLEL_PROC
  /* With parallel processing, can't save triObj hit triangle per triangle, since different threads may each want to store info there.
     Instead, we'll store pointers to the last hit object (TriObj) and sub-object (an individual triangle) within the ray itself, since each
     thread only has one ray. TODO: see if storing last hit info within ray could help other classes as well, even without parallel processing. */
  Object *lastHitObj;
  Object *lastHitSubObj;
#endif

  Ray() {};
  Ray(Vec3& pos, Vec3& dir, int depth, float dist2);
};

#endif //RAY_H