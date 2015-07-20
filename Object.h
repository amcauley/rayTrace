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

    virtual bool isHitByRay(Ray ray)
    {
      /* Derived classes will implement this for themselves. */
      return false;
    }

    virtual void traceRay(Ray ray, Rgb& outHsv, Object* srcList, int nSrc)
    {
      /* Return without any real processing for this base class. */
    }
};

class Sphere : public Object {
  public:
    float rad;

    Sphere() {};
    Sphere(Vec3& a, float b, Rgb& c) : Object(a, c), rad(b) {};

    virtual bool isHitByRay(Ray ray)
    {
      /* Sphere/Ray intersection problem is quadratic. Check discriminant to see if they intersect. */
      float xx, yy, zz;

      xx = ray.loc3.x - loc3.x;
      yy = ray.loc3.y - loc3.y;
      zz = ray.loc3.z - loc3.z;

      float vx, vy, vz, aa, bb, cc;
        
      vx = ray.vec3.x;
      vy = ray.vec3.y;
      vz = ray.vec3.z;
      aa = vx*vx + vy*vy + vz*vz;
      bb = 2 * (xx*vx + yy*vy + zz*vz);
      cc = xx*xx + yy*yy + zz*zz - rad*rad;

      if (bb*bb - 4 * aa*cc >= 0)
      {
        return true;
      }
      else
      {
        return false;
      }
    
    }

    virtual void traceRay(Ray ray, Rgb& outRgb, Object* srcList, int nSrc)
    {
      //TODO: should mix object color and incoming ray color based on incidence angle
      outRgb = rgb;
    }

};

#endif //OBJECT_H