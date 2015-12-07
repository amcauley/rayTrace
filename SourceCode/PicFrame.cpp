#include "PicFrame.hpp"
#include <assert.h>
#include "ObjCommon.hpp"


PicFrame::PicFrame() {}

/* Assume that the points aren't colinear - don't want to spend the cycles checking. */
PicFrame::PicFrame(Vec3 pt1, Vec3 pt2, Vec3 pt3, std::string bmpName) :
  pts{ pt1, pt2, pt3 },
  bmpName(bmpName)
{
  
  /* Find the fourth vertex of the frame given the first 3 (which are given in cw or ccw order). */
  Vec3 pt4 = pt3 + (pt1 - pt2);

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
}

void PicFrame::checkRayHit(Ray& ray, Vec3** hitPtr)
{
  /* To check if PicFrame is hit, check if either of the triangles that it's made of are hit. */
}

void PicFrame::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  /* Given hit point, return RGB corresponding to the pixel in rgbVals that's hit. */
}