#include "Params.hpp"
#include <stdlib.h>

#ifdef DEBUG_MEM_LEAK_REPORT
#include <crtdbg.h>
#endif

#include <iostream>

#ifdef DEBUG_MEM_LEAK_REPORT
#define _CRTDBG_MAP_ALLOC
#endif

#include "LightSources.hpp"
#include "TestWorld.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "SceneParser.hpp"

/* TOP LEVEL TODO:

    - Update % done message logic for parallelized code

    - Look into streaking in tilted horizontal plane near far edge with reflections - maybe rounding error, possibly kill rays all together
      if they get too far away from eye.
     
    - Caching doesn't take into account changing locations / tree depths / etc. between runs.

    - Have root project directory as a config file parameter to help avoid hardcoding file paths

    - Test Triangle/TriObj debug logging / check for mem leaks

    - Look into compiler optimization flags

    - FOV as a parameter instead of manually setting image/eye params.

    - Multiple sources
*/

#ifdef EN_PARALLEL_PROC
  TestWorld *par_proc_testWorld;
#endif

int main()
{
#ifdef DEBUG_MEM_LEAK_ALLOC_CRASH
  _CrtSetBreakAlloc(DEBUG_MEM_LEAK_ALLOC_ADDR);
#endif

  {

    Image* img = NULL;
    Vec3* eye = NULL;
    int nObj = 0, nSrc = 0;
    Object **obj = NULL, **src = NULL;

    std::cout << "Calling cfg parser." << std::endl;
    getParams(CONFIG_FILE_PATH);

    sceneParser(SCENE_PATH, &obj, &nObj, &src, &nSrc, &img, &eye);

    /* ~~~ Start rendering. ~~~ */
    std::cout << "Creating test world\n";
    TestWorld world = TestWorld(obj, nObj, src, nSrc, *eye, img, 1.0f);
#ifdef EN_PARALLEL_PROC
    par_proc_testWorld = &world;
#endif
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
    img->exportBitmap(OUTPUT_IMG);

    delete img;
    delete eye;

    std::cout << "End\n";
  }

#ifdef DEBUG_MEM_LEAK_REPORT
  _CrtDumpMemoryLeaks();
#endif

  return 0;
}