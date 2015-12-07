#pragma once
#ifndef AABB_H
#define AABB_H

#include <vector>
#include "Ray.hpp"
#include "Object.hpp"

typedef enum {
  AXIS_X_ENUM = 0,
  AXIS_Y_ENUM,
  AXIS_Z_ENUM,
  NUM_AXIS
} AxisEnum;

/* Class prototypes for Axis-Aligned Bounding Box. */
class AABB {
  public:
    float xl, xh, yl, yh, zl, zh;
    AABB();
    AABB(float x1, float xh, float y1, float yh, float zl, float zh);
    bool intersects(const AABB& box2);
    void checkRayHit(Ray& ray, Vec3** hitPtr);
};

#endif //AABB_H
