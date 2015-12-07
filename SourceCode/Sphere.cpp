#include "Sphere.hpp"
#include <assert.h>
#include "DbgLog.hpp"
#include "ObjCommon.hpp"

Sphere::Sphere() {};

Sphere::Sphere(Vec3& a, float b, Rgb& c, float i, ScaleParams s): 
  Object(a, c, i, s), 
  rad(b)
{
  /* Bounding box is just the cube enclosing this sphere. Bounding box memory
     should already have been allocated via new during the Object (Sphere's parent)
     constructor, and will be deleted during Object's destructor. */
  *bbox =    AABB(a.x-rad, a.x+rad,
                  a.y-rad, a.y+rad,
                  a.z-rad, a.z+rad);
}

Sphere::~Sphere() {}

void Sphere::checkRayHit(Ray& ray, Vec3** hitPtr)
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

void Sphere::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  /* Default rgb to ambiant. */
  outRgb = rgb*sParams.ambientScale;

#ifdef DEBUG_GEN_PIXEL_REPORT
  dbgPixLog.nextLvl(RAY_TYPE_AMBIENT);
  dbgPixLog.storeInfo(this, rgb);
  dbgPixLog.restoreLvl();
#endif //DEBUG_GEN_PIXEL_REPORT

  /* If we're at max recursion depth, just exit now with the ambiant rgb value. */
  if (ray.depth >= MAX_RAY_DEPTH)
  {
    return;
  }

  /* Compute outward facing normal vector at ray hit location. */
  Vec3 norm = ray.loc3 - loc3;
  norm.normalize();

  /*~~~~~~~~~~ Reflection (Mirror) / Refraction (Glass) Ray Proc ~~~~~~~~~~*/
  float mScale, gScale;
  mScale = sParams.mirrorScale;
  gScale = sParams.glassScale;

  bool runM, runG;
  runM = mScale > 0.0f;
  runG = gScale > 0.0f;

  if (runM || runG)
  {
    float R;
    float distanceScaling = powf(2.0f, -sqrt(ray.dist2)*0.25f);
    Rgb reflRgb, refrRgb;

    R = commonReflRefr(runM, runG, srcList, ray, callingObj, norm, ior, reflRgb, refrRgb);

    outRgb = outRgb + reflRgb*R*mScale + refrRgb*(1.0f-R)*gScale;
  }

  /*~~~~~~~~~~ Shadow Ray Proc ~~~~~~~~~~*/
  if (sParams.shadowScale > 0.0f)
  {
    Rgb tempRgb;
    float scale;
    scale = commonShadowTrace(srcList, ray, callingObj, norm, tempRgb);
    if (scale > 0.0f)
    {
      outRgb = outRgb + tempRgb*(sParams.shadowScale*scale);
    }
  }
}