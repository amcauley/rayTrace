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

void TestWorld::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object* srcList, int nSrc)
{
  //TODO: should test for each object (optimize later w/kd tree or something similar)

  /* hitPtr is set to NULL within checkRayHit to indicate no intersection. Otherwise hitPt will be
     modified to hold the closest intersection point. */
  Vec3 hitPt;
  Vec3* hitPtr = &hitPt;
  objects[0].checkRayHit(ray, &hitPtr);

  if (hitPtr != NULL)
  {
    ray.loc3 = hitPt;
    objects[0].traceRay(ray, outRgb, *this, srcList, 1);
  }
  else /* Miss: */
  {
  }
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

      Vec3 rayVec = (pLoc - eye);

      Pixel* activePixel = img->getPix(pw, ph);
      activePixel->loc3 = pLoc;

      Ray activeRay = Ray(pLoc, rayVec);

      traceRay(activeRay, activePixel->rgb, *this, sources, nSrc);
    }
  }
}

/* Triple pointer :( TODO: just set first element of array to NULL. Then we can use
   pass an array of pointers as input, instead of a pointer to an array of pointers. */
void TestWorld::CheckRayHitExt(Ray ray, Object*** hitObjPtrArrayPtr, Vec3** hitPtr)
{
  /* Similar to checkRayHit. This takes an input ray and returns any objects this
     object deems it to hit. Use case: lower level object, ex. sphere, calls it's
     parent object (TestWorld) to see if a shadow ray hits anything. */

  /* For TestWorld, we'll check all known non-source objects for a collision. */
  int n, q = 0;

  for (n = 0; n < nObj; n++)
  {
    /* hitPtr is set to NULL within checkRayHit to indicate no intersection. Otherwise hitPt will be
    modified to hold the closest intersection point. */
    Vec3 tempPt;
    Vec3* tempPtr = &tempPt;
    objects[n].checkRayHit(ray, &tempPtr);

    if (tempPtr != NULL)
    {    
      if (*hitObjPtrArrayPtr == NULL)
      {
        /* Once we know there's going to hits, allocate array to store results. NULL terminate the obj ptr array. */
        *hitObjPtrArrayPtr = (Object**)malloc((nObj+1)*sizeof(Object*));
        *hitPtr = (Vec3*)malloc(sizeof(Vec3));
      }

      /* TODO: Consider auto-sizing array. */
      /* Populate array of pointers to hit objects and their (first) hit location. */
      (*hitObjPtrArrayPtr)[q] = &objects[n];
      (*hitPtr)[q] = tempPt;
      q++;
    }
  }

  /* If no hits are detected, set the object/location arrays to NULL. */
  if (q == 0)
  {
    *hitObjPtrArrayPtr = NULL;
    *hitPtr = NULL;
  }
  else /* NULL-terminate array. */
  {
    (*hitObjPtrArrayPtr)[q] = NULL;
  }
}