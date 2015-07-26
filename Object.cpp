
#include <assert.h>
#include "Object.h"
#include "Params.h"
#include "TestWorld.h"
#include <math.h> //should go after Params.h to get the benefit of _USE_MATH_DEFINES
#include "Physics.h"

extern TestWorld* temp_globalTestWorld;

Object::Object(Vec3& loc, Rgb& color, float i):
  loc3(loc),
  rgb(color),
  ior(i)         
{}

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

void Object::traceRay(Ray ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
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
    Vec3 tempVec;

    (*hitPtr)->x = ray.loc3.x + ray.vec3.x*tt;
    (*hitPtr)->y = ray.loc3.y + ray.vec3.y*tt;
    (*hitPtr)->z = ray.loc3.z + ray.vec3.z*tt;

    if ((tt <= 0) || ((**hitPtr) == ray.loc3))
    {
      tt = (-bb + sqrt(dd)) / (2.0f*aa);
    }

    (*hitPtr)->x = ray.loc3.x + ray.vec3.x*tt;
    (*hitPtr)->y = ray.loc3.y + ray.vec3.y*tt;
    (*hitPtr)->z = ray.loc3.z + ray.vec3.z*tt;

    if ((tt > 0) && ((**hitPtr) != ray.loc3))
    {
      return;
    }
  }

  *hitPtr = NULL;
}

void Sphere::traceRay(Ray ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  /* Default rgb to ambiant. */
  outRgb = rgb*PARAM_AMBIANT_SCALE;

  /* If we're at max recursion depth, just exit now with the ambiant rgb value. */
  if (ray.depth >= MAX_RAY_DEPTH)
  {
    return;
  }

  /* Compute outward facing normal vector at ray hit location. */
  Vec3 norm = ray.loc3 - loc3;
  norm.normalize();

  /* Determine if this ray is from inside the sphere (internal ray) or external. Internal rays
     originate from a refracted ray traveling inside the sphere. */
  bool isInternal = (ray.vec3.dot(norm) > 0.0f);

  /*~~~~~~~~~~ Reflection (Mirror) Ray Proc ~~~~~~~~~~*/
  Vec3 mirrorVec, *glassVec = NULL;
  ray.vec3.normalize();

  float R;
  if (isInternal)
  {
    physRefraction(ray.vec3, norm*(-1.0f), ior, callingObj.ior, mirrorVec, &glassVec, R);
  }
  else
  {
    physRefraction(ray.vec3, norm, callingObj.ior, ior, mirrorVec, &glassVec, R);
  }

  Ray mirrorRay = Ray(ray.loc3, mirrorVec, ray.depth + 1);

  Rgb tempRgb;
  callingObj.traceRay(mirrorRay, tempRgb, callingObj, srcList, 1);

  if (glassVec == NULL)
  {
    /* TIR: no transmitted/glass wave. */
    R = 1.0f;
  }

  outRgb = outRgb + tempRgb*R*PARAM_REFLECTION_SCALE;

  /*~~~~~~~~~~ Refraction Ray Proc ~~~~~~~~~~*/

  /* Proceed with glassy calculations if no total internal reflection. Not that if this is an
     internal ray, the "glassy" ray is actually in the non-sphere-glass medium, so the name can
     be a little confusing in some situations. */
  if (glassVec != NULL)
  {
    Ray glassRay = Ray(ray.loc3, *glassVec, ray.depth + 1);
    callingObj.traceRay(glassRay, tempRgb, callingObj, srcList, 1);

    outRgb = outRgb + tempRgb*(1.0f-R)*PARAM_REFRACTION_SCALE;
  }
  delete glassVec;

  /*~~~~~~~~~~ Shadow Ray Proc ~~~~~~~~~~*/
  Vec3 shadowDir = (srcList[0]->loc3 - ray.loc3);
  Ray shadowRay = Ray(ray.loc3, shadowDir, ray.depth+1);

  Object** objList = NULL;
  Vec3* objHitPts = NULL;

  /* Generate list of objects hit by the shadow ray and the coordinates of intersections. */
  callingObj.CheckRayHitExt(shadowRay, &objList, &objHitPts);

  if (objList != NULL)
  {
    int n = 0;
    Object* currObjPtr = objList[0];
    /* A source can't occlude itself from this sphere. */
    while ((currObjPtr != NULL) && (currObjPtr != srcList[0]))
    {
      /* While checking for occlusions, we'll disregard shadow ray intersections with
         current object itself if the intersection isn't different (within tolerances) 
         of the initial ray/object intersection. */
      if (currObjPtr != this)
      {
        Vec3 dist2Src = srcList[0]->loc3 - ray.loc3;
        Vec3 dist2Obj = objHitPts[n] - ray.loc3;
        if (dist2Src.mag2() > dist2Obj.mag2())
        {
          delete objList; delete objHitPts;
          return;
        }
      }
      else /* Ray hit ourself */
      {
        delete objList; delete objHitPts;
        return;
      }
      currObjPtr = objList[++n];
    }
    delete objList; delete objHitPts;
  }
 
  /* No obstructions on the shadow ray. Calculate angle between normal vec and shadow
  ray, then scale rgb intensity accordingly (closer to normal = more intense). Normal 
  direction is from center of sphere to the impact ray intersection pt. Angle is in radians,
  from 0 to Pi, but since we don't expect angles greater than Pi/2 (otherwise would have
  occluded ourself), scale by ((Pi/2)-angle). */
  float angle = shadowDir.getAngle(norm);
  float scale = 1.0f - (angle/(float)M_PI_2);

  /* Due to float math, we might have a few boundary cases of negative scaling. Set them to
     be occluded. */
  if (scale < 0)
  {
    return;
  }
  else if (scale > 1.0f) /* Guard against rounding issues by clamping max scale factor. */
  {
    scale = 1.0f;
  }

  srcList[0]->traceRay(shadowRay, tempRgb, callingObj, srcList, 0);

  outRgb = outRgb + tempRgb*(PARAM_SHADOW_SCALE*scale);
}