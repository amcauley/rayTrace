#pragma once
#ifndef LIGHTSOURCES_H
#define LIGHTSOURCES_H

#include "Sphere.hpp"

/* NOTES:
    Light source is a type of object.
*/

class SimpleSource : public Sphere {
  public:
    SimpleSource() {};
    SimpleSource(Vec3& a, float b, Rgb& c, float i, ScaleParams s) : Sphere(a, b, c, i, s) {};

    virtual void traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc);
};

#endif //LIGHTSOURCES_H