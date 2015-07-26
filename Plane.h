#pragma once
#ifndef PLANE_H
#define PLANE_H

#include "Object.h"

class Plane : public Object {
public:
  float d;

  Plane() {};
  Plane(Vec3& norm, float zOffset, Rgb& c, float i);

  virtual void checkRayHit(Ray& ray, Vec3** hitPtr);
  virtual void traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc);
};

#endif //PLANE_H