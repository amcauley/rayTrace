#include "ObjCommon.hpp"

float commonReflRefr(bool runRefl, bool runRefr, Object** srcList, Ray& ray, Object& callingObj, Vec3& normVec, float ior, Rgb& outRgbRefl, Rgb& outRgbRefr)
{
  /* Determine if this ray is from inside the sphere (internal ray) or external. Internal rays
     originate from a refracted ray traveling inside the sphere. */
  bool isInternal = (ray.vec3.dot(normVec) > 0.0f);

  /*~~~~~~~~~~ Reflection (Mirror) Ray Proc ~~~~~~~~~~*/
  Vec3 mirrorVec, *glassVec = NULL;
  ray.vec3.normalize();

  float R;
  if (isInternal)
  {
    physRefraction(ray.vec3, normVec*(-1.0f), ior, callingObj.ior, mirrorVec, &glassVec, R);
  }
  else
  {
    physRefraction(ray.vec3, normVec, callingObj.ior, ior, mirrorVec, &glassVec, R);
  }

  if (runRefl)
  {
    Ray mirrorRay = Ray(ray.loc3, mirrorVec, ray.depth + 1, 0.0f);

#ifdef DEBUG_GEN_PIXEL_REPORT
    dbgPixLog.nextLvl(RAY_TYPE_MIRROR);
#endif

    callingObj.traceRay(mirrorRay, outRgbRefl, callingObj, srcList, 1);

#ifdef DEBUG_GEN_PIXEL_REPORT
    //dbgPixLog.storeInfo(this, tempRgb); //Needs testing/modification for ObjCommon implementation
    dbgPixLog.restoreLvl();
#endif
  }

  if (glassVec == NULL)
  {
    /* TIR: no transmitted/glass wave. */
    R = 1.0f;
  }

  /*~~~~~~~~~~ Refraction Ray Proc ~~~~~~~~~~*/

  /* Proceed with glassy calculations if no total internal reflection. Note that if this is an
  internal ray, the "glassy" ray is actually in the non-sphere-glass medium, so the name can
  be a little confusing in some situations. */
  if (glassVec != NULL)
  {
    if (runRefr)
    {
#ifdef DEBUG_GEN_PIXEL_REPORT
      dbgPixLog.nextLvl(RAY_TYPE_GLASS);
#endif

      Ray glassRay = Ray(ray.loc3, *glassVec, ray.depth + 1, 0.0f);
      callingObj.traceRay(glassRay, outRgbRefr, callingObj, srcList, 1);

#ifdef DEBUG_GEN_PIXEL_REPORT
      //dbgPixLog.storeInfo(this, tempRgb); //Needs testing/modification for ObjCommon implementation
      dbgPixLog.restoreLvl();
#endif
    }
    delete glassVec;
  }

  return R;
}

float commonShadowTrace(Object** srcList, Ray& ray, Object& callingObj, Vec3& normVec, Rgb& outRgb)
{
  Vec3 shadowDir = (srcList[0]->loc3 - ray.loc3);
  Ray shadowRay = Ray(ray.loc3, shadowDir, ray.depth + 1, 0.0f);

  /* Generate list of objects hit by the shadow ray and the coordinates of intersections. */
  std::vector<Object*> hitObjs;
  std::vector<Vec3> hitPts;
  callingObj.CheckRayHitExt(shadowRay, hitObjs, hitPts);

  int nObj = hitObjs.size();

  if(nObj)
  {
    Object* currObjPtr;
    /* Check for occlusions. */
    for (int n = 0; n < nObj; ++n)
    {
      currObjPtr = hitObjs[n];//objList[n];

      /* A source can't occlude itself. */
      if (currObjPtr == srcList[0])
      {
        continue;
      }
      /* While checking for occlusions, we'll disregard shadow ray intersections with
      current object itself if the intersection isn't different (within tolerances)
      of the initial ray/object intersection. */
      Vec3 dist2Src = srcList[0]->loc3 - ray.loc3;
      Vec3 dist2Obj = hitPts[n] - ray.loc3;//objHitPts[n] - ray.loc3;
      if (dist2Src.mag2() > dist2Obj.mag2())
      {
        return -1.0f;
      }
    }
  }

  /* No obstructions on the shadow ray. Calculate angle between normal vec and shadow
  ray, then scale rgb intensity accordingly (closer to normal = more intense). Normal
  direction is from center of sphere to the impact ray intersection pt. Angle is in radians,
  from 0 to Pi, but since we don't expect angles greater than Pi/2 (otherwise would have
  occluded ourself), scale by ((Pi/2)-angle). */

  /* We want these to be hittable from either face, so flip the normal vector locally if needed. */
  Vec3 tempNorm = normVec;
  if (tempNorm.dot(shadowDir) < 0.0f)
  {
    tempNorm = tempNorm*(-1.0f);
  }

  float angle = shadowDir.getAngle(tempNorm);
  float scale = 1.0f - (angle / (float)M_PI_2);

  /* Due to float math, we might have a few boundary cases of negative scaling. Set them to
  be occluded. */
  if (scale < 0)
  {
    return scale;
  }
  else if (scale > 1.0f) /* Guard against rounding issues by clamping max scale factor. */
  {
    scale = 1.0f;
  }

#ifdef DEBUG_GEN_PIXEL_REPORT
  dbgPixLog.nextLvl(RAY_TYPE_SHADOW);
#endif

  srcList[0]->traceRay(shadowRay, outRgb, callingObj, srcList, 0);

  return scale;
}