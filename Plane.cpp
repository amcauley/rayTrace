#include "Plane.h"
#include "DbgLog.h"

Plane::Plane() {}

Plane::~Plane() {}

/* The loc3 value will hold the plane normal vector. This saves space, since 
   there's no real location definition for a plane. */
Plane::Plane(Vec3& normV, float offset, Rgb& c, float i, ScaleParams s): 
  Object(normV, c, i, s),
  d(offset)
{
  /* Unless the plane is perpendicular to an axis, it's extent in that axis will be infinite.
     If it's perpendicular, it will only have a constant value in that axis. We give this constant
     value a little padding to help protect against float rounding/precision sensitivity. */
  if ((normV.y == 0.0f) && (normV.z == 0.0f))
  {
    float sign = normV.x > 0.0f ? -1.0f : 1.0f;
    bbox->xl = sign*offset - VEC3_EQ_FLOAT_TOLERANCE;
    bbox->xh = sign*offset + VEC3_EQ_FLOAT_TOLERANCE;
  }
  else
  {
    bbox->xl = -std::numeric_limits<float>::infinity();
    bbox->xh = std::numeric_limits<float>::infinity();
  }
  if ((normV.x == 0.0f) && (normV.z == 0.0f))
  {
    float sign = normV.y > 0.0f ? -1.0f : 1.0f;
    bbox->yl = sign*offset - VEC3_EQ_FLOAT_TOLERANCE;
    bbox->yh = sign*offset + VEC3_EQ_FLOAT_TOLERANCE;
  }
  else
  {
    bbox->yl = -std::numeric_limits<float>::infinity();
    bbox->yh = std::numeric_limits<float>::infinity();
  }
  if ((normV.x == 0.0f) && (normV.y == 0.0f))
  {
    float sign = normV.z > 0.0f ? -1.0f : 1.0f;
    bbox->zl = sign*offset - VEC3_EQ_FLOAT_TOLERANCE;
    bbox->zh = sign*offset + VEC3_EQ_FLOAT_TOLERANCE;
  }
  else
  {
    bbox->zl = -std::numeric_limits<float>::infinity();
    bbox->zh = std::numeric_limits<float>::infinity();
  }


}

void Plane::checkRayHit(Ray& ray, Vec3** hitPtr)
{
  float tt, denomDotProd;

  denomDotProd = loc3.dot(ray.vec3);
  if (denomDotProd == 0.0f)
  {
    *hitPtr = NULL;
    return;
  }

  tt = (loc3.dot(ray.loc3)*(-1.0f) - d) / denomDotProd;

  if (tt >= 0)
  {
    (*hitPtr)->x = ray.loc3.x + ray.vec3.x*tt;
    (*hitPtr)->y = ray.loc3.y + ray.vec3.y*tt;
    (*hitPtr)->z = ray.loc3.z + ray.vec3.z*tt;
    
    /* Doesn't count if we hit our starting point (due to floating point precision issues. */
    if ((**hitPtr) != ray.loc3)
    {
      return;
    }
  }

  *hitPtr = NULL;
}

void Plane::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  /* Default rgb to ambiant. */
  outRgb = rgb*sParams.ambientScale;

#ifdef DEBUG_GEN_PIXEL_REPORT
  dbgPixLog.nextLvl(RAY_TYPE_AMBIENT);
  dbgPixLog.storeInfo(this, rgb);
  dbgPixLog.restoreLvl();
#endif

  /* If we're at max recursion depth, just exit now with the ambiant rgb value. */
  if (ray.depth >= MAX_RAY_DEPTH)
  {
    return;
  }

  /*~~~~~~~~~~ Reflection (Mirror) Ray Proc ~~~~~~~~~~*/
  Vec3 mirrorVec, *glassVec = NULL;
  ray.vec3.normalize();
  Rgb tempRgb;

  if (loc3.dot(ray.vec3) < 0.0f) /* Make sure ray isn't coming from wrong direction due to precision issues. */
  {
    float R = 1.0f;
    physRefraction(ray.vec3, loc3, callingObj.ior, ior, mirrorVec, &glassVec, R);

    if (glassVec != NULL)
    {
      delete glassVec;
    }
    
    Ray mirrorRay = Ray(ray.loc3, mirrorVec, ray.depth + 1, 0.0f);

#ifdef DEBUG_GEN_PIXEL_REPORT
    dbgPixLog.nextLvl(RAY_TYPE_MIRROR);
#endif

    callingObj.traceRay(mirrorRay, tempRgb, callingObj, srcList, 1);

#ifdef DEBUG_GEN_PIXEL_REPORT
    dbgPixLog.storeInfo(this, tempRgb);
    dbgPixLog.restoreLvl();
#endif

    outRgb = outRgb + tempRgb*sParams.mirrorScale;
  }
  /*~~~~~~~~~~ Refraction Ray Proc ~~~~~~~~~~*/
  /* No refraction for planes, which we consider to have either no thickness
     or infinite thickness. */

  /*~~~~~~~~~~ Shadow Ray Proc ~~~~~~~~~~*/
  Vec3 shadowDir = (srcList[0]->loc3 - ray.loc3);
  Ray shadowRay = Ray(ray.loc3, shadowDir, ray.depth + 1, 0.0f);

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
  float angle = shadowDir.getAngle(loc3);
  float scale = 1.0f - (angle / (float)M_PI_2);

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

#ifdef DEBUG_GEN_PIXEL_REPORT
  dbgPixLog.nextLvl(RAY_TYPE_SHADOW);
#endif

  srcList[0]->traceRay(shadowRay, tempRgb, callingObj, srcList, 0);

#ifdef DEBUG_GEN_PIXEL_REPORT
  dbgPixLog.storeInfo(this, tempRgb);
  dbgPixLog.restoreLvl();
#endif

  outRgb = outRgb + tempRgb*(sParams.shadowScale*scale);
}