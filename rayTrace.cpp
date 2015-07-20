#include <iostream>

#include "LightSources.h"
#include "TestWorld.h"

int main()
{
  /* Image will by 2x2 world units, 10x10 pixels. */
  Vec3 imgLoc = Vec3(-1.0f, 0.5f, 0.0f);
  Vec3 imgNorm = Vec3(0.0f, 0.0f, 1.0f);

  int w = 500, h = 250;
  float pw = 0.004f, ph = 0.004f;

  Image img = Image(imgLoc, imgNorm, w, h, pw, ph);

  /* Eye location. */
  Vec3 eye = Vec3(0.0f, 0.0f, -1.0f);

  Vec3 objLoc = Vec3(1.5f, 0.0f, 2.0f);
  float objRad = 0.5f;
  Rgb objRgb = Rgb(0.4f, 0.2f, 0.8f);

  Object* obj = new Sphere(objLoc, objRad, objRgb);
  
  Vec3 srcLoc = Vec3(-2.0f, 4.0f, 0.0f);
  float srcRad = 1.0f;
  Rgb srcRgb = Rgb(.9f, .9f, .9f);

  Object* src = new SimpleSource(srcLoc, srcRad, srcRgb);

  TestWorld world = TestWorld(obj, 1, src, 1, eye, &img);
  world.runTest();

  img.exportBitmap("rayTraceOutput.bmp");

  std::cout << "End\n";
}