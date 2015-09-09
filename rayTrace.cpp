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
#include "SceneParser.h"

/* TOP LEVEL TODO (9/8):
   
    - Modify AabbTree to use std::vector so it'll auto-resize (instead of allocating max memory - too large for TriObj use).

    - Try other Stanford Dragon OBJ files, maybe lower poly. There are a few dim spots in testing that seem to be due
      to small sharp angles. Maybe a different poly will give different results.

    - Put shadow ray computations (and maybe mirror/glass) into functions since they're very similar
      between different objects.

    - Optimize Triangle routines

    - GetHitObjects prob doesn't need to append NULL ptr now that it returns number of hits (objIdx) 

    - Gate shadow processing if shadow scaling is 0

    - Test Triangle/TriObj debug logging / check for mem leaks

    - Look into compiler optimization flags

    - FOV as a parameter instead of manually setting image/eye params.

    - Multiple sources

    - Pictures - let planes display bitmap images.

    - Implement triangle class, then triObj (a 3d object composed of a collection of triangles)

    - Create OBJ data reader that parses OBJ (ones that only use 3-point polygons) files into a triObj.
        OBJ format info: http://paulbourke.net/dataformats/obj/
        OBJ data files:  http://people.sc.fsu.edu/~jburkardt/data/obj/obj.html

    - Remove obj list from test world / debug class. It's been made obsolete by the object kd tree.
*/

int main()
{
#ifdef DEBUG_MEM_LEAK_ALLOC_CRASH
  _CrtSetBreakAlloc(DEBUG_MEM_LEAK_ALLOC_ADDR);
#endif

  {

    Image* img;
    Vec3* eye;
    int nObj = 0, nSrc = 0;
    Object **obj = NULL, **src = NULL;

    sceneParser("Scenes/TestScene.txt", &obj, &nObj, &src, &nSrc, &img, &eye);
    //sceneParser("Scenes/SphereArray.txt", &obj, &nObj, &src, &nSrc, &img, &eye);

    /* ~~~ Start rendering. ~~~ */
    std::cout << "Creating test world\n";
    TestWorld world = TestWorld(obj, nObj, src, nSrc, *eye, img, 1.0f);
    world.runTest();

    /* Delete allocated objects and sources. */
    /* First delete objects. */
    for (int j = 0; j < nObj; j++)
    {
      bool skipThisObj = false;
      /* Don't delete if we're going to delete it in the sources list later. */
      for (int k = 0; k < nSrc; k++)
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

    for (int j = 0; j < nSrc; j++)
    {
      std::cout << "Del src " << j << "\n";
      delete src[j];
    }
    /* delete the array itself. */
    delete[] src;

    img->autoScale();
    img->exportBitmap("Output/rayTraceOutput.bmp");

    delete img;
    delete eye;

    std::cout << "End\n";
  }

#ifdef DEBUG_MEM_LEAK_REPORT
  _CrtDumpMemoryLeaks();
#endif

  return 0;
}