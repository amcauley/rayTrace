#pragma once
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object.h"
#include "Vec3.h"
#include "Ray.h"

class Triangle : public Object {
  public:
    /* Centroid stored in loc3 variable */
    /* 3 vertices of the triangle. */
    Vec3 pts[3];

    /* Normal vector - not necessarily any notion of outward or inward facing since this triangle doesn't know
       about any higher level object it may be a part of.Objects that are constructed of Triangles
       may or may not force their normals to be inward / outward facing. */
    Vec3 norm;

    /* normMajorAxis - strongest component of the norm. */
    AxisEnum normMajorAxis;

    /* Offset distance for the plane containing pts[]. */
    float d;

    Triangle();
    Triangle(Vec3 pt1, Vec3 pt2, Vec3 pt3, Rgb& c, ScaleParams& s);

    virtual void checkRayHit(Ray& ray, Vec3** hitPtr);
    virtual void traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc);
};

#endif //TRIANGLE_H
