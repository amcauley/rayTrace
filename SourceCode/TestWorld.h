#pragma once
#ifndef TESTWORLD_H
#define TESTWORLD_H

#include "Object.h"
#include "AabbTree.h"

/* NOTES:
  This is the class that contains all other objects. It's responsible for actually running the simulation. The main
  function in this program sets up this object and then sets it running to produce the final image. It derives from
  the Object class so that it will have a traceRay method.
*/

class TestWorld : public Object {
public:
  Object **objects, **sources;
  int nObj, nSrc;
  Vec3 eye;
  Image* img;
  Aabb3dTree objTree;

  TestWorld(Object** objects, int nObj, Object** sources, int nSrc, Vec3& eye, Image* img, float ior);
  ~TestWorld();

  virtual void traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc);
  virtual void CheckRayHitExt(Ray& ray, std::vector<Object*> &hitObjs, std::vector<Vec3> &hitPts);
  void runTest(void);
};

#endif //TESTWORLD_H