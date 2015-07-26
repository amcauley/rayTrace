#pragma once
#ifndef PARAMS_H
#define PARAMS_H

/* Tolerance within each x,y,z Vec3 component must match those of a different
   Vec3 for the vectors to be considered equal. Accounts for floating point precision
   issues. */
#define VEC3_EQ_FLOAT_TOLERANCE (1.0e-4)

/* Overall scale factor (pre autoScale, if any). */
#define PARAM_TOTAL_SCALE (1.0f)

/* Strength of shadow ray contribution. Fraction from 0.0f to 1.0f. */
#define PARAM_SHADOW_SCALE (0.05f)

/* Strength of ambiant lighting contribution to object coloring. Fraction from 0.0f to 1.0f. */
#define PARAM_AMBIANT_SCALE (0.03f)

/* Strength of reflections. */
#define PARAM_REFLECTION_SCALE (0.70f)

/* Strength of refraction/glass rays. */
#define PARAM_REFRACTION_SCALE (0.70f)

/* Max recursion depth for rays. */
#define MAX_RAY_DEPTH (4)

/* Misc Params. */
/* Inlcude math defines such as */
#define _USE_MATH_DEFINES

#endif //PARAMS_H
