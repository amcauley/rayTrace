#include "Plane.hpp"
#include "DbgLog.hpp"
#include "ObjCommon.hpp"

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

  /*~~~~~~~~~~ Reflection (Mirror) / Refraction (Glass, not applicable to plane) Ray Proc ~~~~~~~~~~*/
  float mScale;
  mScale = sParams.mirrorScale;

  bool runM;
  runM = mScale > 0.0f;

  if (runM)
  {
    float R;
    Rgb reflRgb, refrRgb;

    R = commonReflRefr(runM, false, srcList, ray, callingObj, loc3, ior, reflRgb, refrRgb);

    outRgb = outRgb + reflRgb*R*mScale;
  }

  /*~~~~~~~~~~ Shadow Ray Proc ~~~~~~~~~~*/
  if (sParams.shadowScale > 0.0f)
  {
    Rgb tempRgb;
    float scale;
    scale = commonShadowTrace(srcList, ray, callingObj, loc3, tempRgb);
    if (scale > 0.0f)
    {
      outRgb = outRgb + tempRgb*(sParams.shadowScale*scale);
    }
  }
}