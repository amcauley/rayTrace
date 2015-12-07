
#include <assert.h>
#include "Object.hpp"
#include "Params.hpp"
#include "TestWorld.hpp"

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

/* Override assignment operator to enable deep copy of bbox. */
Object& Object::operator= (Object& otherObj)
{
  loc3 = otherObj.loc3;
  rgb = otherObj.rgb;
  ior = otherObj.ior;
  sParams = otherObj.sParams;

  bbox = new AABB( otherObj.bbox->xl, otherObj.bbox->xh,
                   otherObj.bbox->yl, otherObj.bbox->yh,
                   otherObj.bbox->zl, otherObj.bbox->zh);

  return *this;
}

void Object::checkRayHit(Ray& ray, Vec3** hitPtr)
{
  assert(0); //Shouldn't be using default version.
}

void Object::CheckRayHitExt(Ray& ray, std::vector<Object*> &hitObjs, std::vector<Vec3> &hitPts)
{
  assert(0); //Shouldn't be using default version.
}

void Object::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  assert(0); //Shouldn't be using default version.
}