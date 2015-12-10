#include "PicFrame.hpp"
#include <assert.h>
#include "ObjCommon.hpp"
#include <algorithm>
#include "Bitmap.hpp"

PicFrame::PicFrame() {}

PicFrame::~PicFrame()
{
  /* Free rgbVals array. */
  delete[] rgbVals;

  /* Shouldn't need to do anything for tris array since it's not dynamically allocated. */
}

/* Assume that the points aren't colinear - don't want to spend the cycles checking. */
PicFrame::PicFrame(std::string bmpName, Vec3 pt1, Vec3 pt2, Vec3 pt3, float i, ScaleParams s) :
  Object(Vec3(0, 0, 0), Rgb(0, 0, 0), i, s),
  pts{ pt1, pt2, pt3 },
  bmpName(bmpName)
{
  /* Read in size and pixel info and create pixel array from bitmap image. */
  readPixelsFromBmp24(bmpName.c_str(), &rgbVals, &bmpWidth, &bmpHeight);

  /* Find the fourth vertex of the frame given the first 3 (which are given in clockwise order from "top left" corner). */
  Vec3 pt4 = pt3 + (pt1 - pt2);


  /* Form the rectangle by overlapping two oversized right triangles such that their intersection forms a rectangle.
     Example ASCII art of one such traingle and the rectangular region inside it that it'll be used to form:
      *\
      | \
      |  \
      *---*
      |   | \
      *---*--*
  */

  tris[0] = Triangle(pt1*2 - pt2, pt2, pt3*2 - pt2, Rgb(0, 0, 0), ScaleParams());
  tris[1] = Triangle(pt3*2 - pt4, pt4, pt1*2 - pt4, Rgb(0, 0, 0), ScaleParams());

  float a, b, minX, maxX, minY, maxY, minZ, maxZ;

  a = fminf(pt1.x, pt2.x);
  b = fminf(pt3.x, pt4.x);
  minX = fminf(a, b);

  a = fmaxf(pt1.x, pt2.x);
  b = fmaxf(pt3.x, pt4.x);
  maxX = fmaxf(a, b);

  a = fminf(pt1.y, pt2.y);
  b = fminf(pt3.y, pt4.y);
  minY = fminf(a, b);

  a = fmaxf(pt1.y, pt2.y);
  b = fmaxf(pt3.y, pt4.y);
  maxY = fmaxf(a, b);

  a = fminf(pt1.z, pt2.z);
  b = fminf(pt3.z, pt4.z);
  minZ = fminf(a, b);

  a = fmaxf(pt1.z, pt2.z);
  b = fmaxf(pt3.z, pt4.z);
  maxZ = fmaxf(a, b);

  bbox = new AABB(minX, maxX, minY, maxY, minZ, maxZ);

  widthVec = pts[1] - pts[0];
  heightVec = pts[2] - pts[1];

  imgWidth = sqrt(widthVec.mag2());
  imgHeight = sqrt(heightVec.mag2());

  /* Now normalize vectors so future dot products are simplified. */
  widthVec.normalize();
  heightVec.normalize();
}

void PicFrame::checkRayHit(Ray& ray, Vec3** hitPtr)
{
  /* To check if PicFrame is hit, check if both of the triangles that it's made of are hit. The intersection of these
     Triangles is a rectangle (the PicFrame border). */

  /* First check first triangle. If we hit, then *hitPtr will be non-NULL. Else we'll check the second triangle. */
  tris[0].checkRayHit(ray, hitPtr);

  if (*hitPtr != NULL)
  {
    tris[1].checkRayHit(ray, hitPtr);
  }
}

void PicFrame::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  /* Given hit point, return RGB corresponding to the pixel in rgbVals that's hit. The width is determined
     by projecting the hit point onto the (pts[1] - pts[0]) vector, and height by projecting onto (pts[2] - pts[1]). */
  Vec3 corner2HitVec = ray.loc3 - pts[0];
  float widthPercent = corner2HitVec.dot(widthVec) / imgWidth;
  float heightPercent = corner2HitVec.dot(heightVec) / imgHeight;

  int pW = std::min((int)(widthPercent*bmpWidth), bmpWidth - 1); //take Min to guard against unlikely case of widthPercent >= 1.0f
  int pH = std::min((int)(heightPercent*bmpHeight), bmpHeight - 1); //take Min to guard against unlikely case of widthPercent >= 1.0f


  /* Default rgb to the pixel value. */
  Rgb thisRgb = rgbVals[pH*bmpWidth + pW];
  outRgb = thisRgb*sParams.ambientScale; //Rgb(1.0f, 1.0f, 1.0f);

#ifdef DEBUG_GEN_PIXEL_REPORT
  dbgPixLog.nextLvl(RAY_TYPE_AMBIENT);
  dbgPixLog.storeInfo(this, thisRgb);
  dbgPixLog.restoreLvl();
#endif

  /* If we're at max recursion depth, just exit now with the ambiant rgb value. */
  if (ray.depth >= MAX_RAY_DEPTH)
  {
    return;
  }

  /*~~~~~~~~~~ Reflection (Mirror) / Refraction (Glass, not applicable to PicFrame) Ray Proc ~~~~~~~~~~*/
  /* We want PicFrame to be hittable from either face, so flip the normal vector (and offset direction) if needed. */
  Vec3 tempNorm = tris[0].norm;
  if (tempNorm.dot(ray.vec3) > 0.0f)
  {
    tempNorm = tempNorm*(-1.0f);
  }

  float mScale;
  mScale = sParams.mirrorScale;

  bool runM;
  runM = mScale > 0.0f;

  if (runM)
  {
    float R;
    Rgb reflRgb, refrRgb;

    R = commonReflRefr(runM, false, srcList, ray, callingObj, tempNorm, ior, reflRgb, refrRgb);

    outRgb = outRgb + reflRgb*R*mScale;
  }

  /*~~~~~~~~~~ Shadow Ray Proc ~~~~~~~~~~*/
  if (sParams.shadowScale > 0.0f)
  {
    Rgb tempRgb;
    float scale;
    scale = commonShadowTrace(srcList, ray, callingObj, tempNorm, tempRgb);
    if (scale > 0.0f)
    {
      outRgb = outRgb + tempRgb*(sParams.shadowScale*scale);
    }
  }
}