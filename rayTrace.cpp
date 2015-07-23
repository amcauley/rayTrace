#include <iostream>

#include "LightSources.h"
#include "TestWorld.h"

/* TOP LEVEL TODO (7/22):

    - FOV as a parameter sintead of manually setting image/eye params.

    - Write parser so a scene can be input as a separate file listing sources/objects 

    - Maybe use const for passing some elements, especially rays, to handling functions that don't really
      need to modify the original. Maybe not, though, might unleash a constpocalypse.

    - Introduce boxes in addition to spheres. Low prio since this sounds like a decent amount of work and I'm
      not excited about it at the moment.

    - Look into possible memory leaks.
*/

int main()
{
  /* Image definition. */
  Vec3 imgNorm = Vec3(0.0f, 0.0f, 1.0f);

  /* Eye location. */
  Vec3 eye = Vec3(1.375f, 0.0f, -10.0f);

#if 1
  /* Higher Res */
  Vec3 imgLoc = Vec3(0.0f, 1.0f, -2.0f);
  int w = 600, h = 400;
  float pw = 0.005f, ph = 0.005f;
#else
  /* Low Res, mainly for testing. */
  Vec3 imgLoc = Vec3(-0.5f, 0.5f, 0.0f);
  int w = 10, h = 10;
  float pw = 0.1f, ph = 0.1f;
#endif

  Image img = Image(imgLoc, imgNorm, w, h, pw, ph);

  Object** obj = new Object*[3];

  /* Big Sphere */
  Vec3 objLoc = Vec3(2.0f, 0.0f, 2.0f);
  Rgb objRgb = Rgb(255 / 255.0f, 140 / 255.0f, 0 /255.0f);
  obj[0] = new Sphere(objLoc, 1.0f, objRgb);

  /* Medium Sphere */
  objLoc = Vec3(0.8f, 0.0f, 0.8f);
  objRgb = Rgb(0 / 255.0f, 255 / 255.0f, 0 / 255.0f);
  obj[1] = new Sphere(objLoc, 0.2f, objRgb);

  /* Small Sphere */
  objLoc = Vec3(0.5f, 0.2f, 0.5f);
  objRgb = Rgb(160 / 255.0f, 200 / 255.0f, 200 / 255.0f);
  obj[2] = new Sphere(objLoc, 0.1f, objRgb);

  Vec3 srcLoc = Vec3(-5.0f, 0.0f, -5.0f);
  Rgb srcRgb = Rgb(255 / 255.0f, 255 / 255.0f, 255 / 255.0f);
  Object* src = new SimpleSource(srcLoc, 0.1f, srcRgb);

  TestWorld world = TestWorld(obj, 3, &src, 1, eye, &img);
  world.runTest();

  img.exportBitmap("rayTraceOutput.bmp");

  std::cout << "End\n";
  return 0;
}