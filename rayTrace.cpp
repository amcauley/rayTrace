#include <iostream>

#include "LightSources.h"
#include "TestWorld.h"

/* TOP LEVEL TODO (7/21):

    1. Modify code, probably Sphere::CheckRayHit, to ignore intersections at the very
       beginning of the ray. This essentially moves the Vec3::operator!= check from Sphere::traceRay
       during shadowRay tracing into an earlier part of the processing. In case we hit the ray origin,
       we should still check for a later intersection.

    2. TestWorld::CheckRayHitExt can be modified to avoid the need of triple pointers. See notes in TestWorld.cpp.
*/

int main()
{
  /* Image will by 2x2 world units, 10x10 pixels. */
  Vec3 imgLoc = Vec3(-1.0f, 1.0f, 0.0f);
  Vec3 imgNorm = Vec3(0.0f, 0.0f, 1.0f);

  int w = 400, h = 400;
  float pw = 0.005f, ph = 0.005f;

  Image img = Image(imgLoc, imgNorm, w, h, pw, ph);

  /* Eye location. */
  Vec3 eye = Vec3(0.0f, 0.0f, -1.0f);

  Vec3 objLoc = Vec3(0.0f, 0.0f, 2.0f);
  float objRad = 1.5f;
  Rgb objRgb = Rgb(0.1f, 0.1f, 0.9f);

  Object* obj = new Sphere(objLoc, objRad, objRgb);
  
  Vec3 srcLoc = Vec3(-3.0f, 3.0f, -1.0f);
  float srcRad = 1.0f;
  Rgb srcRgb = Rgb(.9f, .1f, .1f);

  Object* src = new SimpleSource(srcLoc, srcRad, srcRgb);

  TestWorld world = TestWorld(obj, 1, src, 1, eye, &img);
  world.runTest();

  img.exportBitmap("rayTraceOutput.bmp");

  std::cout << "End\n";
}