#include "Params.h"
#include <stdlib.h>

#ifdef DEBUG_MEM_LEAK_REPORT
#include <crtdbg.h>
#endif

#include <iostream>

#ifdef DEBUG_MEM_LEAK_REPORT
#define _CRTDBG_MAP_ALLOC
#endif

#include "LightSources.h"
#include "TestWorld.h"
#include "Sphere.h"
#include "Plane.h"

/* TOP LEVEL TODO (7/26):

    - add logging capability to print out all info for a particular pixel and write out to a dbg file.
      will be useful for debugging some issues. For example, some pixels at the edge of the sphere are too
      dim/bright depending on float point tolerance. Example file:

      pix xyz
      (0) rgb, scale
        (1 shadow) rgb, scale
          (2 shadow) rgb, scale
          (2 mirror) rgb, scale
          (2 glass) rgb, scale
          (2 ambient) rgb, scale
        (1 mirror) rgb, scale
        (1 glass) rgb, scale
          ...
        (1 ambient) rgb, scale
          ...

    - FOV as a parameter sintead of manually setting image/eye params.

    - Write parser so a scene can be input as a separate file listing sources/objects

    - Multiple sources

    - Cleaner debug framework, maybe some #define to enable debug vars, e.g. temp_globalTestWorld

    - Pictures - let planes display bitmap images.

    - Introduce rectangles/boxes (6 rectangles) in addition to spheres.
*/

TestWorld* temp_globalTestWorld;

int main()
{
#ifdef DEBUG_MEM_LEAK_ALLOC_CRASH
  _CrtSetBreakAlloc(DEBUG_MEM_LEAK_ALLOC_ADDR);
#endif

  {
    /* Image definition. */
    Vec3 imgNorm = Vec3(0.0f, 0.0f, 1.0f);

    /* Eye location. */
    Vec3 eye = Vec3(0.0f, 0.0f, -10.0f);

  #if 1
    /* Higher Res */
    Vec3 imgLoc = Vec3(-1.0f, 1.0f, -2.0f);
    int w = 400, h = 400;
    float pw = 0.005f, ph = 0.005f;
  #else
    /* Low Res, mainly for testing. */
    Vec3 imgLoc = Vec3(-0.5f, 0.5f, 0.0f);
    int w = 10, h = 10;
    float pw = 0.1f, ph = 0.1f;
  #endif

    Image img = Image(imgLoc, imgNorm, w, h, pw, ph);


    int nObj = 10, nSrc = 1;
    Object** obj = new Object*[nObj];
    Object** src = new Object*[nSrc];

    /* ~~~ Source ~~~ */
    Vec3 srcLoc = Vec3(-5.0f, 5.0f, 0.0f);
    Rgb srcRgb = Rgb(214 / 255.0f, 227 / 255.0f, 190 / 255.0f);
    src[0] = new SimpleSource(srcLoc, 0.5f, srcRgb, 1.0f, ScaleParams(1.0f, 0.0f, 0.0f, 0.0f, 0.0f));
    /* Source is also an object. */
    obj[nObj - 1] = src[0];

    /* ~~~ Objects ~~~ */
    /* Bottom Plane */
    Vec3 objLoc = Vec3(0.0f, 1.0f, 0.0f); //For a plane, this is actually the normal vector
    Rgb objRgb = Rgb(255 / 255.0f, 0 / 255.0f, 0 / 255.0f);
    obj[0] = new Plane(objLoc, 1.5f, objRgb, 10.0f, ScaleParams(1.05f, 0.6f, 0.0f, 0.0f, 0.3f));

    /* Left Plane */
    objLoc = Vec3(1.0f, 0.0f, 0.0f);
    objRgb = Rgb(0 / 255.0f, 255 / 255.0f, 0 / 255.0f);
    obj[1] = new Plane(objLoc, 5.0f, objRgb, 1.8f, ScaleParams(1.05f, 0.6f, 0.0f, 0.0f, 0.3f));

    /* Right Plane */
    objLoc = Vec3(-1.0f, 0.0f, 0.0f);
    objRgb = Rgb(0 / 255.0f, 0 / 255.0f, 255 / 255.0f);
    obj[2] = new Plane(objLoc, 5.0f, objRgb, 1.8f, ScaleParams(1.05f, 0.6f, 0.0f, 0.0f, 0.3f));

    /* Top Plane */
    objLoc = Vec3(0.0f, -1.0f, 0.0f);
    objRgb = Rgb(255 / 255.0f, 255 / 255.0f, 0 / 255.0f);
    obj[3] = new Plane(objLoc, 5.0f, objRgb, 1.8f, ScaleParams(1.05f, 0.6f, 0.0f, 0.0f, 0.3f));

    /* Back Plane */
    objLoc = Vec3(0.0f, 0.0f, -1.0f);
    objRgb = Rgb(0 / 255.0f, 255 / 255.0f, 255 / 255.0f);
    obj[4] = new Plane(objLoc, 50.0f, objRgb, 1.8f, ScaleParams(1.05f, 0.6f, 0.0f, 0.0f, 0.3f));

    /* Front (behind the eye) Plane, seen in reflections */
    objLoc = Vec3(0.0f, 0.0f, 1.0f);
    objRgb = Rgb(255 / 255.0f, 0 / 255.0f, 255 / 255.0f);
    obj[5] = new Plane(objLoc, 50.0f, objRgb, 1.8f, ScaleParams(1.05f, 0.6f, 0.0f, 0.0f, 0.3f));

    /* Sphere 1 */
    objLoc = Vec3(0.5f, -0.5f, 3.5f);
    objRgb = Rgb(255 / 255.0f, 255 / 255.0f, 255 / 255.0f);
    obj[6] = new Sphere(objLoc, 1.0f, objRgb, 1.5f, ScaleParams(1.2f, 0.00f, 1.0f, 1.0f, 0.01f));

    /* Sphere 2 */
    objLoc = Vec3(0.05f, 1.0f, 6.5f);
    objRgb = Rgb(255 / 255.0f, 255 / 255.0f, 0 / 255.0f);
    obj[7] = new Sphere(objLoc, 1.0f, objRgb, 1.5f, ScaleParams(1.3f, 0.00f, 1.0f, 1.0f, 0.01f));

    /* Sphere 3 */
    objLoc = Vec3(-1.0f, -0.5f, 10.0f);
    objRgb = Rgb(0 / 255.0f, 255 / 255.0f, 0 / 255.0f);
    obj[8] = new Sphere(objLoc, 1.0f, objRgb, 1.5f, ScaleParams(1.3f, 0.00f, 1.0f, 1.0f, 0.01f));


    /* ~~~ Start rendering. ~~~ */
    TestWorld world = TestWorld(obj, nObj, src, nSrc, eye, &img, 1.0f);
    temp_globalTestWorld = &world; //debugging variable
    world.runTest();

    /* Delete allocated objects and sources. */
    int j;
    for (j = 0; j < nObj; j++)
    {
      bool skipThisObj = false;
      int k;
      /* Don't delete again if we already deleted as part of objects list. */
      for (k = 0; k < nObj; k++)
      {
        if (obj[j] == src[k])
        {
          skipThisObj = true;
          break;
        }
      }
      if (skipThisObj)
      {
        continue;
      }
      /* No repeat in source list, go ahead and delete. */
      std::cout << "Del obj " << j << "\n";
      delete obj[j];
    }
    /* delete the array itself. */
    delete[] obj;

    for (j = 0; j < nSrc; j++)
    {
      std::cout << "Del src " << j << "\n";
      delete src[j];
    }
    /* delete the array itself. */
    delete[] src;

    img.autoScale();
    img.exportBitmap("rayTraceOutput.bmp");

    std::cout << "End\n";
  }

#ifdef DEBUG_MEM_LEAK_REPORT
  _CrtDumpMemoryLeaks();
#endif

  return 0;
}