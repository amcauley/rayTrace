#include "AABB.h"
#include <math.h>
#include <limits>
#include <assert.h>

AABB::AABB()
{}

AABB::AABB(float x1, float xh, float yl, float yh, float zl, float zh) :
  xl(x1),
  xh(xh),
  yl(yl),
  yh(yh),
  zl(zl),
  zh(zh)
{
}

/* Box/Ray intersection algorithm based on https://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm */
bool AABB::intersects(const AABB& box2)
{
  float leftVal;
  bool intersects = true;

  /* Find the left-most box and declare intersection, at least for this axis, if the leftmost
     box's rightmost index is greater than the rightmost box's left index. */
  leftVal = fminf(xl, box2.xl);
  if (leftVal == xl) intersects &= xh > box2.xl;
  else intersects &= box2.xh > xl;
  if (!intersects) return false;

  leftVal = fminf(yl, box2.yl);
  if (leftVal == yl) intersects &= yh > box2.yl;
  else intersects &= box2.yh > yl;
  if (!intersects) return false;

  leftVal = fminf(zl, box2.zl);
  if (leftVal == zl) intersects &= zh > box2.zl;
  else intersects &= box2.zh >zl;
  return intersects;
}

void AABB::checkRayHit(Ray& ray, Vec3** hitPtr)
{

  Vec3* tempVecPtr = *hitPtr;
  *hitPtr = NULL;

  float tNear, tFar, t1, t2, temp;
  tNear = -std::numeric_limits<float>::infinity();
  tFar = std::numeric_limits<float>::infinity();

  /* X plane sandwich. */
  if (ray.vec3.x == 0.0f)
  {
    if ((ray.loc3.x < xl) || (ray.loc3.x > xh)) return;
  }
  else
  {
    t1 = (xl - ray.loc3.x) / ray.vec3.x;
    t2 = (xh - ray.loc3.x) / ray.vec3.x;

    if (t1 > t2)
    {
      temp = t2;
      t2 = t1;
      t1 = temp;
    }
    tNear = std::fmaxf(t1, tNear);
    tFar = std::fminf(t2, tFar);
    if ((tNear > tFar) || (tFar < 0.0f)) return;
  }

  /* Y plane sandwich. */
  if (ray.vec3.y == 0.0f)
  {
    if ((ray.loc3.y < yl) || (ray.loc3.y > yh)) return;
  }
  else
  {
    t1 = (yl - ray.loc3.y) / ray.vec3.y;
    t2 = (yh - ray.loc3.y) / ray.vec3.y;

    if (t1 > t2)
    {
      temp = t2;
      t2 = t1;
      t1 = temp;
    }
    tNear = std::fmaxf(t1, tNear);
    tFar = std::fminf(t2, tFar);
    if ((tNear > tFar) || (tFar < 0.0f)) return;
  }

  /* Z plane sandwich. */
  if (ray.vec3.z == 0.0f)
  {
    if ((ray.loc3.z < zl) || (ray.loc3.z > zh)) return;
  }
  else
  {
    t1 = (zl - ray.loc3.z) / ray.vec3.z;
    t2 = (zh - ray.loc3.z) / ray.vec3.z;

    if (t1 > t2)
    {
      temp = t2;
      t2 = t1;
      t1 = temp;
    }
    tNear = std::fmaxf(t1, tNear);
    tFar = std::fminf(t2, tFar);
    if ((tNear > tFar) || (tFar < 0.0f)) return;
  }

  *hitPtr = tempVecPtr;
  /* We hit the box, populate hit point as tNear. */
  **hitPtr = ray.loc3 + ray.vec3*tNear;
}