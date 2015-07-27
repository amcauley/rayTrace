#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include "Object.h"

class Sphere : public Object {
public:
  float rad;

  Sphere() {};
  Sphere(Vec3& a, float b, Rgb& c, float i, ScaleParams s);

  virtual void checkRayHit(Ray& ray, Vec3** hitPtr);
  virtual void traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc);
};

#endif //SPHERE_H