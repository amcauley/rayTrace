#include <iostream>

#include "LightSources.h"
#include "TestWorld.h"

/* TOP LEVEL TODO (7/25):

    - Multiple sources

    - Cleaner debug framework, maybe some #define to enable debug vars, e.g. temp_globalTestWorld

    - Beer's law

    - FOV as a parameter sintead of manually setting image/eye params.

    - Write parser so a scene can be input as a separate file listing sources/objects 

    - Simple animations (put out a series of images, can use GIMP or something to turn into a GIF).

    - Maybe use const for passing some elements, especially rays, to handling functions that don't really
      need to modify the original. Maybe not, though, might unleash a constpocalypse.

    - Introduce rectangles/boxes (6 rectangles) in addition to spheres. Low prio since this sounds like a 
      decent amount of work and I'm not excited about it at the moment.

    - Look into possible memory leaks.
*/

TestWorld* temp_globalTestWorld;

int main()
{
  /* Image definition. */
  Vec3 imgNorm = Vec3(0.0f, 0.0f, 1.0f);

  /* Eye location. */
  Vec3 eye = Vec3(1.5f, 0.0f, -10.0f);

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

  Object** obj = new Object*[4];

  /* Big Sphere */
  //Vec3 objLoc = Vec3(2.0f, 0.0f, 2.0f);
  Vec3 objLoc = Vec3(2.0f, 0.0f, 2.0f);
  Rgb objRgb = Rgb(86 / 255.0f, 126 / 255.0f, 219 / 255.0f);
  obj[0] = new Sphere(objLoc, 1.0f, objRgb, 1.5f);

  /* Medium Sphere */
  objLoc = Vec3(0.8f, 0.0f, 0.8f);
  objRgb = Rgb(180 / 255.0f, 211 / 255.0f, 217 / 255.0f);
  obj[1] = new Sphere(objLoc, 0.2f, objRgb, 1.8f);

  /* Small Sphere */
  objLoc = Vec3(1.4f, 0.4f, 0.0f);
  objRgb = Rgb(255 / 255.0f, 0 / 255.0f, 0 / 255.0f);
  obj[2] = new Sphere(objLoc, 0.2f, objRgb, 1.3f);

  Vec3 srcLoc = Vec3(-5.0f, 0.0f, 11.0f);
  Rgb srcRgb = Rgb(214 / 255.0f, 227 / 255.0f, 190 / 255.0f);
  Object* src = new SimpleSource(srcLoc, 3.0f, srcRgb, 1.0f);

  /* Source itself can be an object. */
  obj[3] = src;

  TestWorld world = TestWorld(obj, 4, &src, 1, eye, &img, 1.0f);
  temp_globalTestWorld = &world; //debugging variable
  world.runTest();

  delete[] obj;

  img.autoScale();
  img.exportBitmap("rayTraceOutput.bmp");

  std::cout << "End\n";
  return 0;
}