#pragma once
#ifndef OBJECT_H
#define OBJECT_H

/* Forward class definitions. */
class AABB;

/* Included files. */
#include <iostream>

#include "Pixel.h"
#include "Ray.h"
#include <math.h> //should go after Params.h to get the benefit of _USE_MATH_DEFINES
#include "Physics.h"
#include "AABB.h"

class Object {
  public:
    /* AABB (axis-aligned bounding box). */
    AABB* bbox;
    /* Object location. What this means will differ depending on the derived class. Ex. center of sphere
       vs. one of the corners of a box. */
    Vec3 loc3;
    /* HSV color for this object. */
    Rgb rgb;
    /* Index of refraction. */
    float ior;
    /* Scaling parameters. */
    ScaleParams sParams;

    Object();
    virtual ~Object();
    Object(Vec3& loc, Rgb& rgb, float ior, ScaleParams& s);

    virtual void checkRayHit(Ray& ray, Vec3** hitPtr);
    virtual void CheckRayHitExt(Ray& ray, Object*** hitObjPtrArrayPtr, Vec3** hitPtr);
    virtual void traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc);
};

#endif //OBJECT_H