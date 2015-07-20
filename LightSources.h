#pragma once
#ifndef LIGHTSOURCES_H
#define LIGHTSOURCES_H

#include "Object.h"

/* NOTES:
    Light source is a type of object.
*/

class SimpleSource : public Cube {
  public:
    SimpleSource() {};
    SimpleSource(Vec3& a, Vec3& b, Rgb& c) : Cube(a, b, c) {};

    virtual void traceRay(Ray& ray, Rgb& outHsv, Object& srcList, int nSrc)
    {
      /* For simple source, just return its color. Don't care about other sources. */
      outHsv = rgb;
    }
};

#endif //LIGHTSOURCES_H