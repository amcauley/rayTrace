#include "Ray.h"

Ray::Ray(Vec3& loc, Vec3& vec, int d)
{
  loc3 = loc;
  vec3 = vec;
  depth = d;
}