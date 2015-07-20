#include "TestWorld.h"

TestWorld::TestWorld(Object* obj, int nObjs, Object* src, int nSrcs, Vec3& ey, Image* image)
{
  std::cout << "TestWorld Constructor\n";

  objects = obj;
  nObj = nObjs;
  sources = src;
  nSrc = nSrcs;
  eye = ey;
  img = image;
}

void TestWorld::runTest(void)
{
  /* Location of active pixel. */
  Vec3 pLoc;

  int pw, ph;
  for (pw = 0; pw < img->width; pw++)
  {
    for (ph = 0; ph < img->height; ph++)
    {
      img->getPixLoc(pLoc, pw, ph);

      Vec3 rayVec;
      rayVec.x = pLoc.x - eye.x;
      rayVec.y = pLoc.y - eye.y;
      rayVec.z = pLoc.z - eye.z;

      Pixel* activePixel = img->getPix(pw, ph);
      activePixel->loc3 = pLoc;

      Ray activeRay = Ray(pLoc, rayVec);

      traceRay(activeRay, activePixel->rgb, sources, nSrc);
    }
  }
}