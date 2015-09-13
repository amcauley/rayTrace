#include "Ray.h"

Ray::Ray(Vec3& loc, Vec3& vec, int d, float dist2):
  loc3(loc),
  vec3(vec),
  depth(d),
  dist2(dist2)
{
}