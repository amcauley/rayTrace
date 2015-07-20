#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>

#include "Pixel.h"
#include "Ray.h"

class Object {
  public:
    /* Location of hitbox corner. */
    Vec3 loc3;
    /* Hitbox dimensions extending along +x, +y, +z directions from corner. */
    Vec3 dim3;
    /* HSV color for this object. */
    Rgb rgb;

    Object() {};
    Object(Vec3&, Vec3&, Rgb&);

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

class Cube : public Object {
  public:
    Cube() {};
    Cube(Vec3& a, Vec3& b, Rgb& c) : Object(a, b, c) {};

    virtual bool isHitByRay(Ray ray)
    {
      /* A cube is hit if the ray intersects the hitbox. */
      std::cout << "WARNING: using bad hit detection as a temp hack!\n"; //TODO: proper hit detection using math, not stepping ray through space
      {
        Ray tempRay = ray;

        int step;
        for (step = 0; step < 100; step++)
        {
          if ( (tempRay.loc3.x >= loc3.x) && (tempRay.loc3.x <= (loc3.x + dim3.x)) &&
               (tempRay.loc3.y >= loc3.y) && (tempRay.loc3.y <= (loc3.y + dim3.y)) &&
               (tempRay.loc3.z >= loc3.z) && (tempRay.loc3.z <= (loc3.z + dim3.z)) )
          {
            return true;
          }
          tempRay.loc3.x += tempRay.vec3.x;
          tempRay.loc3.y += tempRay.vec3.y;
          tempRay.loc3.z += tempRay.vec3.z;
        }
        return false;
      }     
    }

    virtual void traceRay(Ray ray, Rgb& outRgb, Object* srcList, int nSrc)
    {
      std::cout << "WARNING: only using object color as a temp hack!\n"; //TODO: should mix object color and incoming ray color based on incidence angle
      outRgb = rgb;
    }

};

#endif //OBJECT_H