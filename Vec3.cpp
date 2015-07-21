#include "Vec3.h"

Vec3::Vec3()
{
  x = y = z = 0;
}

Vec3::Vec3(float ah, float as, float av)
{
  x = ah;
  y = as;
  z = av;
}

float Vec3::mag2(void)
{
  return x*x + y*y + z*z;
}

Vec3 Vec3::operator+ (const Vec3& b)
{
  return Vec3(this->x + b.x, this->y + b.y, this->z + b.z);
}

Vec3 Vec3::operator- (const Vec3& b)
{
  return Vec3(this->x - b.x, this->y - b.y, this->z - b.z);
}

bool Vec3::operator== (const Vec3& b)
{
  bool xComp, yComp, zComp;
  xComp = ((b.x - VEC3_EQ_FLOAT_TOLERANCE <= x) && (x <= b.x + VEC3_EQ_FLOAT_TOLERANCE));
  yComp = ((b.y - VEC3_EQ_FLOAT_TOLERANCE <= y) && (y <= b.y + VEC3_EQ_FLOAT_TOLERANCE));
  zComp = ((b.z - VEC3_EQ_FLOAT_TOLERANCE <= z) && (z <= b.z + VEC3_EQ_FLOAT_TOLERANCE));
  
  return xComp && yComp && zComp;
}

bool Vec3::operator!= (const Vec3& b)
{
  return !(operator==(b));
}