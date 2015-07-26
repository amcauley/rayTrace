
#include <assert.h>
#include "Object.h"
#include "Params.h"
#include "TestWorld.h"

extern TestWorld* temp_globalTestWorld;

Object::Object(Vec3& loc, Rgb& color, float i):
  loc3(loc),
  rgb(color),
  ior(i)         
{}

void Object::checkRayHit(Ray& ray, Vec3** hitPtr)
{
  /* Derived classes will implement this for themselves. */
  *hitPtr = NULL;
}

void Object::CheckRayHitExt(Ray& ray, Object*** hitObjPtrArrayPtr, Vec3** hitPtr)
{
  /* Similar to checkRayHit. This takes an input ray and returns any objects this
  object deems it to hit. Use case: lower level object, ex. sphere, calls it's
  parent object (TestWorld) to see if a shadow ray hits anything. */

  assert(0); //method called for an object without an implementation defined.
}

void Object::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  assert(0); //method called for an object without an implementation defined.
}
