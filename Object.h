#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>

#include "Pixel.h"
#include "Ray.h"

class Object {
  public:
    /* Object location. */
    Vec3 loc3;
    /* HSV color for this object. */
    Rgb rgb;

    Object() {};
    Object(Vec3&, Rgb&);

    virtual void checkRayHit(Ray ray, Vec3** hitPtr);
    virtual void CheckRayHitExt(Ray ray, Object*** hitObjPtrArrayPtr, Vec3** hitPtr);
    virtual void traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object* srcList, int nSrc);
};

class Sphere : public Object {
  public:
    float rad;

    Sphere() {};
    Sphere(Vec3& a, float b, Rgb& c) : Object(a, c), rad(b) {};

    virtual void checkRayHit(Ray ray, Vec3** hitPtr);
    virtual void traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object* srcList, int nSrc);
};

#endif //OBJECT_H