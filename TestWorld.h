#pragma once
#ifndef TESTWORLD_H
#define TESTWORLD_H

#include "Object.h"

/* NOTES:
  This is the class that contains all other objects. It's responsible for actually running the simulation. The main
  function in this program sets up this object and then sets it running to produce the final image. It derives from
  the Object class so that it will have a traceRay method.
*/

class TestWorld : public Object {
public:
  Object *objects, *sources;
  int nObj, nSrc;
  Vec3 eye;
  Image* img;

  TestWorld(Object* objects, int nObj, Object* sources, int nSrc, Vec3& eye, Image* img);

  virtual void traceRay(Ray& ray, Rgb& outRgb, Object* srcList, int nSrc)
  {
    std::cout << "WARNING: TestWorld traceRay not complete!\n"; //should test for each object (optimize later w/kd tree)
    if (objects[0].isHitByRay(ray))
    {
      std::cout << "Hit\n";
      objects[0].traceRay(ray, outRgb, srcList, 1);
    }
    else
    {
      std::cout << "Miss\n";
    }
  }

  void runTest(void);
};

#endif //TESTWORLD_H