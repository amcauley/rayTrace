
#include <assert.h>
#include "Object.h"
#include "Params.h"
#include "TestWorld.h"

Object::Object() {};
Object::~Object() 
{ 
  if (bbox != NULL) delete bbox; 
};

Object::Object(Vec3& loc, Rgb& color, float i, ScaleParams& sParams):
  loc3(loc),
  rgb(color),
  ior(i),
  sParams(sParams)
{
  bbox = new AABB;
}

void Object::checkRayHit(Ray& ray, Vec3** hitPtr)
{
  assert(0); //Shouldn't be using default version.
}

void Object::CheckRayHitExt(Ray& ray, Object*** hitObjPtrArrayPtr, Vec3** hitPtr)
{
  assert(0); //Shouldn't be using default version.
}

void Object::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  assert(0); //Shouldn't be using default version.
}