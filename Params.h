#pragma once
#ifndef PARAMS_H
#define PARAMS_H

/* Max recursion depth for rays. */
#define MAX_RAY_DEPTH (5)

/* Linear supersampling factor. Ex. 3 means that for each pixel, we'll take 3x3 = 9
ray samples and average them to get the final pixel value. >= 1. */
#define PARAM_LIN_SUPERSAMPLE_FACTOR (3)

/* Depth of top level object kd-tree. Should be >= 1.*/
#define TOP_LEVEL_OBJECT_TREE_DEPTH (2)

/* Print out processing progress info. */
#define STATUS_PRINTS_ENABLED

/* Tolerance within each x,y,z Vec3 component must match those of a different
Vec3 for the vectors to be considered equal. Accounts for floating point precision
issues. */
#define VEC3_EQ_FLOAT_TOLERANCE ((float)1.0e-4)

/* Enable to create an output log tracing the RGB values leading to pixel (x,y)'s value. */
//#define DEBUG_GEN_PIXEL_REPORT

#define DEBUG_PIXEL_REPORT_X (198)  //Pixel width coord
#define DEBUG_PIXEL_REPORT_Y (180)  //Pixel height coord

/* Enable to mark the debug pixel white for easier locating. */
//#define MARK_DEBUG_PIXEL

/* Disable image autoscaling. */
//#define SKIP_IMAGE_AUTOSCALE

/* Enable to create memory leak report at end of program. */
#define DEBUG_MEM_LEAK_REPORT

#ifdef DEBUG_MEM_LEAK_REPORT
  //#define DEBUG_MEM_LEAK_ALLOC_CRASH
  #ifdef DEBUG_MEM_LEAK_ALLOC_CRASH
    #define DEBUG_MEM_LEAK_ALLOC_ADDR (166)
  #endif //DEBUG_MEM_LEAK_ALLOC_CRASH
#endif //DEBUG_MEM_LEAK_REPORT

/* Default overall scale factor (pre autoScale, if any). */
#define PARAM_TOTAL_SCALE (1.0f)

/* Default strength of shadow ray contribution. Fraction from 0.0f to 1.0f. */
#define PARAM_SHADOW_SCALE (0.2f)

/* Default strength of ambiant lighting contribution to object coloring. Fraction from 0.0f to 1.0f. */
#define PARAM_AMBIENT_SCALE (0.1f)

/* Default strength of reflections. */
#define PARAM_REFLECTION_SCALE (0.4f)

/* Default strength of refraction/glass rays. */
#define PARAM_REFRACTION_SCALE (0.4f)

/* Misc Params. */
/* Inlcude math defines such as */
#define _USE_MATH_DEFINES


/* ~~~~~~~~~~ Param Related Structs/Classes ~~~~~~~~~~ */
class ScaleParams
{
public:
  float totalScale;
  float shadowScale;
  float mirrorScale;
  float glassScale;
  float ambientScale;

  ScaleParams():
    totalScale(PARAM_TOTAL_SCALE), shadowScale(PARAM_SHADOW_SCALE), mirrorScale(PARAM_REFLECTION_SCALE),
    glassScale(PARAM_REFRACTION_SCALE), ambientScale(PARAM_AMBIENT_SCALE)
  {};

  ScaleParams(float totalScale, float shadowScale, float mirrorScale,
              float glassScale, float ambientScale):
    totalScale(totalScale), shadowScale(shadowScale), mirrorScale(mirrorScale),
    glassScale(glassScale), ambientScale(ambientScale)
  {};
};

#endif //PARAMS_H
