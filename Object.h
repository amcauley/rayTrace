#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>

#include "Pixel.h"
#include "Ray.h"
#include <math.h> //should go after Params.h to get the benefit of _USE_MATH_DEFINES
#include "Physics.h"

class Object {
  public:
    /* Object location. */
    Vec3 loc3;
    /* HSV color for this object. */
    Rgb rgb;
    /* Index of refraction. */
    float ior;
    /* Scaling parameters. */
    ScaleParams sParams;

    Object() {};
    Object(Vec3& loc, Rgb& rgb, float ior, ScaleParams& s);

    virtual void checkRayHit(Ray& ray, Vec3** hitPtr);
    virtual void CheckRayHitExt(Ray& ray, Object*** hitObjPtrArrayPtr, Vec3** hitPtr);
    virtual void traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc);
};

#endif //OBJECT_H