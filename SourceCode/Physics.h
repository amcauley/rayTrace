#pragma once
#ifndef PHYSICS_H
#define PHYSICS_H

#include "Ray.h"

extern void physRefraction(Vec3& inVecNormd, Vec3& norm, float nIn, float nOut, Vec3& mirrorVec, Vec3** glassVec, float& R);

#endif //PHYSICS_H
