#include "Object.h"

Object::Object(Vec3& loc, Vec3& dim, Rgb& color)
{
  loc3 = loc;
  dim3 = dim;
  rgb = color;
}