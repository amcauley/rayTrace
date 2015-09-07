#include "Triangle.h"
#include "DbgLog.h"
#include <assert.h>

Triangle::Triangle() {}

/* Assume that the points aren't colinear - don't want to spend the cycles checking. */
Triangle::Triangle(Vec3 pt1, Vec3 pt2, Vec3 pt3, Rgb& c, ScaleParams& s):
  pts{pt1, pt2, pt3}
{
  sParams = s;
  rgb = c;

  /* Compute and store centroid. */
  loc3 = (pt1 + pt2 + pt3)*(1.0f / 3.0f);

  /* Find the normal vector. Note that the sign can later flip in the checkRayHit method. */
  norm = (pt2 - pt1).cross(pt3 - pt1);
  norm.normalize();

  d = norm.dot(pt1)*(-1.0f);

  float maxDim2;
  if (norm.x*norm.x >= norm.y*norm.y)
  {
    normMajorAxis = AXIS_X_ENUM;
    maxDim2 = norm.x*norm.x;
  }
  else
  {
    normMajorAxis = AXIS_Y_ENUM;
    maxDim2 = norm.y*norm.y;
  }

  normMajorAxis = norm.z*norm.z > maxDim2 ? AXIS_Z_ENUM : normMajorAxis;

  /* Bounding box is the cube enclosing this triangle. */
  float minX, maxX, minY, maxY, minZ, maxZ;

  minX = fminf(fminf(pt1.x, pt2.x), pt3.x);
  minY = fminf(fminf(pt1.y, pt2.y), pt3.y);
  minZ = fminf(fminf(pt1.z, pt2.z), pt3.z);

  maxX = fmaxf(fmaxf(pt1.x, pt2.x), pt3.x);
  maxY = fmaxf(fmaxf(pt1.y, pt2.y), pt3.y);
  maxZ = fmaxf(fmaxf(pt1.z, pt2.z), pt3.z);

  bbox = new AABB(minX, maxX, minY, maxY, minZ, maxZ);

}

static void checkRayTriSide2D( Vec3** hitPtr,
                        float cen1, float cen2,
                        float hit1, float hit2,
                        float vertP1, float vertP2, 
                        float vertQ1, float vertQ2)
{
  float m, b, den, t = -1.0f, cenRay1, cenRay2;

  cenRay1 = hit1 - cen1;
  cenRay2 = hit2 - cen2;

  den = vertQ1 - vertP1;
  if (den)
  {
    m = (vertQ2 - vertP2) / den;
    b = vertP2 - m*vertP1;

    den = cenRay2 - m*cenRay1;
    if (den)
    {
      t = (m*cen1 + b - cen2) / den;
    }
    /* Triangle edge and centroid->hitPt ray are parallel. */
  }
  else
  {
    if (cenRay1 != 0)
    {
      t = (vertP1 - cen1) / cenRay1;
    }
    /* else, centroid->HitPoint ray is parallel to the triangle edge we're checking against. There will be no collision (ignore degenerate
    case of overlapping segments). */
  }

  /* If the ray hits the triangle side, we need to check if the hit point is between the triangle vertices that define it. */
  if (t > 0)
  {
    float side1, side2;
    side1 = cen1 + cenRay1*t;
    side2 = cen2 + cenRay2*t;
    /* If the centroid-sideHit ray is longer than the centroid-hitPt ray, the hitPt could be in the triangle (there are cases where it isn't).
    We'll have to check the other sides to confirm. */
    if (Vec3(cenRay1, cenRay2, 0.0f).mag2() > Vec3(side1 - cen1, side2 - cen2, 0.0f).mag2())
    {
      *hitPtr = NULL;
    }
  }
}

void Triangle::checkRayHit(Ray& ray, Vec3** hitPtr)
{
  float tt, denomDotProd;

  /* We want these to be hittable from either face, so flip the normal vector (and offset direction) if needed. */
  Vec3 tempNorm = norm;
  float tempD = d;
  if (norm.dot(ray.vec3) > 0.0f)
  {
    tempNorm = norm*(-1.0f);
    tempD = -d;
  }

  denomDotProd = tempNorm.dot(ray.vec3);
  if (denomDotProd == 0.0f)
  {
    *hitPtr = NULL;
    return;
  }

  tt = (tempNorm.dot(ray.loc3)*(-1.0f) - tempD) / denomDotProd;

  if (tt >= 0)
  {
    (*hitPtr)->x = ray.loc3.x + ray.vec3.x*tt;
    (*hitPtr)->y = ray.loc3.y + ray.vec3.y*tt;
    (*hitPtr)->z = ray.loc3.z + ray.vec3.z*tt;

    /* Doesn't count if we hit our starting point (due to floating point precision issues. */
    if ((**hitPtr) != ray.loc3)
    {
      /* Now we know that the ray intersects the plane of the triangle. Check if the intersection lies within the triangle
         by projecting all points onto a 2D plane by eliminating one of the coords (the one corresponding to the largest
         component of the normal vector, which avoid cramming the projected points too close together). Then check if the
         ray segment from the projected centroid to the projected hit point crosses one of the projected triangle sides. If
         not, then the hit point is within the triangle. If it does hit a side, it's outside of the triangle, so no hit. */

      /* Vertex locations (2 coords per vertex - could have also defined a 2D vector class). */
      float a1, a2, b1, b2, c1, c2, cen1, cen2, hit1, hit2;

      switch (normMajorAxis)
      {
        case AXIS_X_ENUM:
        {
          hit1 = (*hitPtr)->y; hit2 = (*hitPtr)->z;
          cen1 = loc3.y; cen2 = loc3.z;
          a1 = pts[0].y; a2 = pts[0].z;
          b1 = pts[1].y; b2 = pts[1].z;
          c1 = pts[2].y; c2 = pts[2].z;
          break;
        }
        case AXIS_Y_ENUM:
        {
          hit1 = (*hitPtr)->x; hit2 = (*hitPtr)->z;
          cen1 = loc3.x; cen2 = loc3.z;
          a1 = pts[0].x; a2 = pts[0].z;
          b1 = pts[1].x; b2 = pts[1].z;
          c1 = pts[2].x; c2 = pts[2].z;
          break;
        }
        case AXIS_Z_ENUM:
        {
          hit1 = (*hitPtr)->x; hit2 = (*hitPtr)->y;
          cen1 = loc3.x; cen2 = loc3.y;
          a1 = pts[0].x; a2 = pts[0].y;
          b1 = pts[1].x; b2 = pts[1].y;
          c1 = pts[2].x; c2 = pts[2].y;
          break;
        }
        default:
          assert(0);
      }

      /* Check if we're within side ab. */
      checkRayTriSide2D(hitPtr, cen1, cen2, hit1, hit2, a1, a2, b1, b2);
      if (*hitPtr == NULL) return;

      /* Check if we're within side ac. */
      checkRayTriSide2D(hitPtr, cen1, cen2, hit1, hit2, a1, a2, c1, c2);
      if (*hitPtr == NULL) return;

      /* Check if we're within side bc. */
      checkRayTriSide2D(hitPtr, cen1, cen2, hit1, hit2, b1, b2, c1, c2);

      return;
    }
  }

  *hitPtr = NULL;
}

void Triangle::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  /* Start with ambient contribution. */
  outRgb = rgb*sParams.ambientScale;

  /* If we're at max recursion depth, just exit now with the ambiant rgb value. */
  if (ray.depth >= MAX_RAY_DEPTH)
  {
    return;
  }

  Rgb tempRgb;

  /*~~~~~~~~~~ Shadow Ray Proc ~~~~~~~~~~*/
  if (sParams.shadowScale > 0.0f)
  {
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
      /* Check for occlusions. */
      while (currObjPtr != NULL)
      {
        /* A source can't occlude itself. */
        if (currObjPtr == srcList[0])
        {
          currObjPtr = objList[++n];
          continue;
        }
        /* While checking for occlusions, we'll disregard shadow ray intersections with
        current object itself if the intersection isn't different (within tolerances)
        of the initial ray/object intersection. */
        if (currObjPtr != this)
        {
          Vec3 dist2Src = srcList[0]->loc3 - ray.loc3;
          Vec3 dist2Obj = objHitPts[n] - ray.loc3;
          if (dist2Src.mag2() > dist2Obj.mag2())
          {
            delete[] objList; delete[] objHitPts;
            return;
          }
        }
        else /* Ray hit ourself */
        {
          delete[] objList; delete[] objHitPts;
          return;
        }
        currObjPtr = objList[++n];
      }
      delete[] objList; delete[] objHitPts;
    }

    /* No obstructions on the shadow ray. Calculate angle between normal vec and shadow
    ray, then scale rgb intensity accordingly (closer to normal = more intense). Normal
    direction is from center of sphere to the impact ray intersection pt. Angle is in radians,
    from 0 to Pi, but since we don't expect angles greater than Pi/2 (otherwise would have
    occluded ourself), scale by ((Pi/2)-angle). */

    /* We want these to be hittable from either face, so flip the normal vector locally if needed. */
    Vec3 tempNorm = norm;
    if (norm.dot(shadowDir) < 0.0f)
    {
      tempNorm = norm*(-1.0f);
    }

    float angle = shadowDir.getAngle(tempNorm);
    float scale = 1.0f - (angle / (float)M_PI_2);

    //std::cout << angle << " " << scale << std::endl;

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
}