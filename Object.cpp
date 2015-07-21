#include <math.h>
#include <assert.h>
#include "Object.h"

Object::Object(Vec3& loc, Rgb& color)
{
  loc3 = loc;
  rgb = color;
}

void Object::checkRayHit(Ray ray, Vec3** hitPtr)
{
  /* Derived classes will implement this for themselves. */
  *hitPtr = NULL;
}

void Object::CheckRayHitExt(Ray ray, Object*** hitObjPtrArrayPtr, Vec3** hitPtr)
{
  /* Similar to checkRayHit. This takes an input ray and returns any objects this
  object deems it to hit. Use case: lower level object, ex. sphere, calls it's
  parent object (TestWorld) to see if a shadow ray hits anything. */

  assert(0); //method called for an object without an implementation defined.
}

void Object::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object* srcList, int nSrc)
{
  assert(0); //method called for an object without an implementation defined.
}

void Sphere::checkRayHit(Ray ray, Vec3** hitPtr)
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

  float dd = bb*bb - 4 * aa*cc;

  if (dd >= 0)
  {
    float tt = (-bb - sqrt(dd)) / (2.0f*aa);

    if (tt <= 0)
    {
      tt = (-bb + sqrt(dd)) / (2.0f*aa);
    }     

    if (tt > 0)
    {
      (*hitPtr)->x = ray.loc3.x + ray.vec3.x*tt;
      (*hitPtr)->y = ray.loc3.y + ray.vec3.y*tt;
      (*hitPtr)->z = ray.loc3.z + ray.vec3.z*tt;
    }
    else
    {
      *hitPtr = NULL;
    }
  }
  else
  {
    *hitPtr = NULL;
  }
}

void Sphere::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object* srcList, int nSrc)
{
  //TODO: should mix object color and incoming ray color based on incidence angle

  Vec3 shadowDir = (srcList[0].loc3 - ray.loc3);
  Ray shadowRay = Ray(ray.loc3, shadowDir);

  Object** objList = NULL;
  Vec3* objHitPts = NULL;

  /* Generate list of objects hit by the shadow ray and the coordinates of intersections. */
  callingObj.CheckRayHitExt(shadowRay, &objList, &objHitPts);

  /* Default rgb is from the source. Overwrite w/0,0,0 if we detect that the source is blocked
     by an object (including this object itself). */
  srcList[0].traceRay(shadowRay, outRgb, callingObj, NULL, 0);

  if (objList != NULL)
  {
    int n = 0;
    Object* currObjPtr = objList[0];
    while (currObjPtr != NULL)
    {
      /* While checking for occlusions, we'll disregard shadow ray intersections with
         current object itself if the intersection isn't different (within tolerances) 
         of the initial ray/object intersection. */
      if ((currObjPtr != this) || (objHitPts[n] != ray.loc3))
      {
        Vec3 dist2Src = srcList[0].loc3 - ray.loc3;
        Vec3 dist2Obj = objHitPts[n] - ray.loc3;
        if (dist2Src.mag2() > dist2Obj.mag2())
        {
          outRgb.b = outRgb.g = outRgb.r = 0.0f;
          return;
        }
      }
      currObjPtr = objList[++n];

    }
  }
}