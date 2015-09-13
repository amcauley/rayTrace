#pragma once
#ifndef PARAMS_H
#define PARAMS_H

#include <string>

#define CONFIG_FILE_PATH "Z:/Documents/Visual Studio 2015/Projects/Project1/Project1/config.txt"

#define SCENE_PATH_DEFAULT "Z:/Documents/Visual Studio 2015/Projects/Project1/Project1/Scenes/SphereArray.txt"
extern std::string SCENE_PATH;

#define OUTPUT_IMG_DEFAULT "Z:/Documents/Visual Studio 2015/Projects/Project1/Project1/Output/rayTraceOutput.bmp"
extern std::string OUTPUT_IMG;

/* Max recursion depth for rays. */
#define MAX_RAY_DEPTH_DEFAULT (5)
extern unsigned int MAX_RAY_DEPTH;

/* Linear supersampling factor. Ex. 3 means that for each pixel, we'll take 3x3 = 9
ray samples and average them to get the final pixel value. >= 1. */
#define PARAM_LIN_SUPERSAMPLE_FACTOR_DEFAULT (1)
extern unsigned int PARAM_LIN_SUPERSAMPLE_FACTOR;

/* Depth of top level object kd-tree. Should be >= 1.*/
#define TOP_LEVEL_OBJECT_TREE_DEPTH_DEFAULT (3)
extern unsigned int TOP_LEVEL_OBJECT_TREE_DEPTH;
/* Tree depth for TriObj objects. */
#define TRI_TREE_DEPTH_DEFAULT (8)
extern unsigned int TRI_TREE_DEPTH ;

/* Print out processing progress info. */
#define STATUS_PRINTS_ENABLED

/* Tolerance within each x,y,z Vec3 component must match those of a different
Vec3 for the vectors to be considered equal. Accounts for floating point precision
issues. */
#define VEC3_EQ_FLOAT_TOLERANCE ((float)1.0e-4)

/* Enable to create an output log tracing the RGB values leading to pixel (x,y)'s value.
   NOTE: debug log is stored globally, which may trigger false memory leak report. */
//#define DEBUG_GEN_PIXEL_REPORT

/* Enable to mark the debug pixel white for easier locating. */
//#define MARK_DEBUG_PIXEL

#define DEBUG_PIXEL_REPORT_X (100)  //Pixel width coord
#define DEBUG_PIXEL_REPORT_Y (100)  //Pixel height coord

/* Reverse order of pixels visited. Useful if debugging one of the later pixels under normal ordering.
   NOTE: Status updates ("x% complete" messages) don't currently work correctly with this debug feature. */
//#define REVERSE_PIX_ORDER

/* Disable image autoscaling. */
//#define SKIP_IMAGE_AUTOSCALE

/* Enable to create memory leak report at end of program.
   NOTE: if DEBUG_GEN_PIXEL_REPORT is defined, it may incorrectly show up as a memory leak because it's stored as a global. */
//#define DEBUG_MEM_LEAK_REPORT

#ifdef DEBUG_MEM_LEAK_REPORT
  //#define DEBUG_MEM_LEAK_ALLOC_CRASH
  #ifdef DEBUG_MEM_LEAK_ALLOC_CRASH
    #define DEBUG_MEM_LEAK_ALLOC_ADDR (274)
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

/* Function Prototypes. */
void getParams(std::string fileName);

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
