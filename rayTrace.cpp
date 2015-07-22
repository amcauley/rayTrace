#include <iostream>

#include "LightSources.h"
#include "TestWorld.h"

/* TOP LEVEL TODO (7/22):

    - Get 2 objects working in the same scene

    - Write parser so a scene can be input as a separate file listing sources/objects 
*/

int main()
{
  /* Image definition. */
  Vec3 imgNorm = Vec3(0.0f, 0.0f, 1.0f);

#if 1
  /* Higher Res */
  Vec3 imgLoc = Vec3(-0.75f, 0.75f, 0.0f);
  int w = 300, h = 300;
  float pw = 0.005f, ph = 0.005f;
#else
  /* Low Res, mainly for testing. */
  Vec3 imgLoc = Vec3(-0.5f, 0.5f, 0.0f);
  int w = 10, h = 10;
  float pw = 0.1f, ph = 0.1f;
#endif

  Image img = Image(imgLoc, imgNorm, w, h, pw, ph);

  /* Eye location. */
  Vec3 eye = Vec3(0.0f, 0.0f, -1.0f);

  Vec3 objLoc = Vec3(0.0f, 0.0f, 2.0f);
  float objRad = 1.4f;
  Rgb objRgb = Rgb(144/255.0f, 224/255.0f, 169/255.0f);

  Object* obj = new Sphere(objLoc, objRad, objRgb);

#if 1
  Vec3 srcLoc = Vec3(-1.0f, 1.0f, -1.0f); /* Offset */
#else
  Vec3 srcLoc = Vec3(0.0f, 0.0f, -1.0f); /* Centered */
#endif

  float srcRad = 1.0f;
  Rgb srcRgb = Rgb(220/255.0f, 130/255.0f, 130/255.0f);

  Object* src = new SimpleSource(srcLoc, srcRad, srcRgb);

  TestWorld world = TestWorld(obj, 1, src, 1, eye, &img);
  world.runTest();

  img.exportBitmap("rayTraceOutput.bmp");

  std::cout << "End\n";
}