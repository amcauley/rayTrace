#include <iostream>

#include "LightSources.h"
#include "TestWorld.h"
#include "Sphere.h"
#include "Plane.h"

/* TOP LEVEL TODO (7/26):

    - Check for memory leaks (https://msdn.microsoft.com/en-us/library/x98tx3cf%28v=VS.100%29.aspx)

    - Individual params (reflectance scaling, etc.) for each object.

    - FOV as a parameter sintead of manually setting image/eye params.

    - Write parser so a scene can be input as a separate file listing sources/objects

    - Multiple sources

    - Cleaner debug framework, maybe some #define to enable debug vars, e.g. temp_globalTestWorld

    - Beer's law 

    - Pictures - let planes display bitmap images.

    - Simple animations (put out a series of images, can use GIMP or something to turn into a GIF).

    - Maybe use const for passing some elements, especially rays, to handling functions that don't really
      need to modify the original. Maybe not, though, might unleash a constpocalypse.

    - Introduce rectangles/boxes (6 rectangles) in addition to spheres.
*/

TestWorld* temp_globalTestWorld;

int main()
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

  Object** obj = new Object*[7];

  /* Big Sphere */
  //Vec3 objLoc = Vec3(2.0f, 0.0f, 2.0f);
  Vec3 objLoc = Vec3(0.5f, -.5f, 3.5f);
  Rgb objRgb = Rgb(86 / 255.0f, 126 / 255.0f, 219 / 255.0f);
  obj[0] = new Sphere(objLoc, 1.0f, objRgb, 1.5f);

  /* Bottom Plane */
  objLoc = Vec3(0.0f, 1.0f, 0.0f); //For a plane, this is actually the normal vector
  objRgb = Rgb(255 / 255.0f, 0 / 255.0f, 0 / 255.0f);
  obj[1] = new Plane(objLoc, 1.5f, objRgb, 10.0f);

  /* Left Plane */
  objLoc = Vec3(1.0f, 0.0f, 0.0f);
  objRgb = Rgb(0 / 255.0f, 255 / 255.0f, 0 / 255.0f);
  obj[2] = new Plane(objLoc, 5.0f, objRgb, 1.8f);

  /* Right Plane */
  objLoc = Vec3(-1.0f, 0.0f, 0.0f);
  objRgb = Rgb(0 / 255.0f, 0 / 255.0f, 255 / 255.0f);
  obj[3] = new Plane(objLoc, 5.0f, objRgb, 1.8f);

  /* Top Plane */
  objLoc = Vec3(0.0f, -1.0f, 0.0f);
  objRgb = Rgb(255 / 255.0f, 255 / 255.0f, 0 / 255.0f);
  obj[4] = new Plane(objLoc, 5.0f, objRgb, 1.8f);

  /* Back Plane */
  objLoc = Vec3(0.0f, 0.0f, -1.0f);
  objRgb = Rgb(0 / 255.0f, 255 / 255.0f, 255 / 255.0f);
  obj[5] = new Plane(objLoc, 50.0f, objRgb, 1.8f);

  Vec3 srcLoc = Vec3(-5.0f, 5.0f, 0.0f);
  Rgb srcRgb = Rgb(214 / 255.0f, 227 / 255.0f, 190 / 255.0f);
  Object* src = new SimpleSource(srcLoc, 0.5f, srcRgb, 1.0f);

  /* Source itself can be an object. */
  obj[6] = src;

  TestWorld world = TestWorld(obj, 7, &src, 1, eye, &img, 1.0f);
  temp_globalTestWorld = &world; //debugging variable
  world.runTest();

  delete[] obj;

  img.autoScale();
  img.exportBitmap("rayTraceOutput.bmp");

  std::cout << "End\n";
  return 0;
}