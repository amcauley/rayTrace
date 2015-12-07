#pragma once
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object.hpp"
#include "Vec3.hpp"
#include "Ray.hpp"

class Triangle : public Object {
  public:
    /* Centroid stored in loc3 variable */
    /* 3 vertices of the triangle. */
    Vec3 pts[3];

    /* Normal vector - the sign of this can flip depending on the direction of the incoming ray. */
    Vec3 norm;

    /* Offset distance for the plane containing pts[]. If the normal distance flips, so does this. */
    float d;

    /* normMajorAxis - strongest component of the norm. */
    AxisEnum normMajorAxis;

    Triangle();
    Triangle(Vec3 pt1, Vec3 pt2, Vec3 pt3, Rgb& c, ScaleParams& s);

    virtual void checkRayHit(Ray& ray, Vec3** hitPtr);
    virtual void traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc);
};

#endif //TRIANGLE_H
