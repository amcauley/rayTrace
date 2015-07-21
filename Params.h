#pragma once
#ifndef PARAMS_H
#define PARAMS_H

/* Tolerance within each x,y,z Vec3 component must match those of a different
   Vec3 for the vectors to be considered equal. Accounts for floating point precision
   issues. */
#define VEC3_EQ_FLOAT_TOLERANCE (1.0e-3)

#endif //PARAMS_H
