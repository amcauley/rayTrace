#pragma once
#ifndef VEC3_H
#define VEC3_H

#include "Params.h"

class Vec3 {
  public:
    float x;
    float y;
    float z;

    Vec3();
    Vec3(float a, float b, float c);
    float mag2(void);
    Vec3 operator+ (const Vec3& b);
    Vec3 operator- (const Vec3& b);
    bool operator== (const Vec3& b);
    bool operator!= (const Vec3& b);
};

#endif //VEC3_H