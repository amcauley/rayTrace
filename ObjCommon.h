#pragma once
#ifndef OBJCOMMON_H
#define OBJCOMMON_H

#include "Object.h"

/* Some routines commonly used by multiple objects. */

/* Shadow ray tracing. This function will compute the outRgb for the shadowRay, pre-scaling, after tracing that ray. 
   Returns the scale value, which will be negative if it's invalid. */
float commonShadowTrace(Object** srcList, Ray& ray, Object& callingObj, Vec3& normVec, Rgb& outRgb);

/* Reflection/Refraction RGB calculations. Return the R value (used to determine reflection/refraction ratio). */
float commonReflRefr(bool runRefl, bool runRefr, Object** srcList, Ray& ray, Object& callingObj, Vec3& normVec, float ior, Rgb& outRgbRefl, Rgb& outRgbRefr);

#endif //OBJCOMMON_H
