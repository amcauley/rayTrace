#pragma once
#ifndef VEC3_H
#define VEC3_H

#include "Params.hpp"

class Vec3 {
  public:
    float x;
    float y;
    float z;

    Vec3();
    Vec3(float a, float b, float c);
    float mag2(void);
    float dot(const Vec3& b);
    Vec3 cross(const Vec3& b);
    float getAngle(Vec3& b);
    Vec3& normalize(void);
    Vec3 operator+ (const Vec3& b);
    Vec3 operator- (const Vec3& b);
    Vec3 operator* (const float b);
    bool operator== (const Vec3& b);
    bool operator!= (const Vec3& b);
};

#endif //VEC3_H