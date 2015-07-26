#pragma once
#ifndef LIGHTSOURCES_H
#define LIGHTSOURCES_H

#include "Object.h"

/* NOTES:
    Light source is a type of object.
*/

class SimpleSource : public Sphere {
  public:
    SimpleSource() {};
    SimpleSource(Vec3& a, float b, Rgb& c, float i) : Sphere(a, b, c, i) {};

    virtual void traceRay(Ray ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc);
};

#endif //LIGHTSOURCES_H