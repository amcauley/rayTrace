#include <math.h>
#include <iostream>
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
  return Vec3(x + b.x, y + b.y, z + b.z);
}

Vec3 Vec3::operator- (const Vec3& b)
{
  return Vec3(x - b.x, y - b.y, z - b.z);
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

Vec3 Vec3::operator* (const float b)
{
  return Vec3(x*b, y*b, z*b);
}

float Vec3::dot(const Vec3& b)
{
  return x*b.x + y*b.y + z*b.z;
}

Vec3 Vec3::normalize(void)
{
  float norm = sqrt(mag2());
  x /= norm;
  y /= norm;
  z /= norm;
  return *this;
}

float Vec3::getAngle(Vec3& b)
{
  float cosAng = dot(b) / sqrt(mag2()*b.mag2());
  //std::cout << "dotProd = " << dot(b) << "\n";
  return acosf(cosAng); //in radians
}