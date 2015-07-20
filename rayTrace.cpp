#include <iostream>

#include "LightSources.h"
#include "TestWorld.h"

int main()
{
  /* Image will by 2x2 world units, 10x10 pixels. */
  Vec3 imgLoc = Vec3(-1, 1, 0);
  Vec3 imgNorm = Vec3(0, 0, 1);

  int w = 20, h = 20;
  float pw = 0.1f, ph = 0.1f;

  Image img = Image(imgLoc, imgNorm, w, h, pw, ph);

  /* Eye location. */
  Vec3 eye = Vec3(0, 0, -1);

  Vec3 objLoc = Vec3(-1, -1, 2);
  Vec3 objDim = Vec3(2, 2, 2);
  Rgb objRgb = Rgb(0, 1.0, 1.0);

  Object* obj = new Cube(objLoc, objDim, objRgb);
  
  Vec3 srcLoc = Vec3(-2, 4, 0);
  Vec3 srcDim = Vec3(1, 1, 1);
  Rgb srcRgb = Rgb(.9, .9, .9);

  Object* src = new SimpleSource(srcLoc, srcDim, srcRgb);

  TestWorld world = TestWorld(obj, 1, src, 1, eye, &img);
  world.runTest();

  img.exportBitmap("rayTraceOutput.bmp");
  std::cout << "End\n";
}