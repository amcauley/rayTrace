#include "TestWorld.h"
#include "DbgLog.h"

TestWorld::TestWorld(Object** obj, int nObjs, Object** src, int nSrcs, Vec3& ey, Image* image, float i):
  objects(obj),
  nObj(nObjs),
  sources(src),
  nSrc(nSrcs),
  eye(ey),
  img(image)
{
  ior = i;
}

void TestWorld::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  //TODO: optimize later w/kd tree or something similar

  Vec3 closestHit;
  float dist2;
  int n = getClosestObj(ray, closestHit, &dist2); /* Get the closest object, i.e. the one our ray will hit, if any. */

  if (n >= 0) /* We actually hit something. */
  {
    Ray tempRay = ray;
    tempRay.loc3 = closestHit;
    tempRay.dist2 = dist2;
    objects[n]->traceRay(tempRay, outRgb, *this, srcList, 1);
    outRgb = outRgb*objects[n]->sParams.totalScale;
  }
  else /* Miss: */
  {
    outRgb.r = outRgb.g = outRgb.b = 0.0f;
  }
}

void TestWorld::runTest(void)
{
  /* Location of active pixel. */
  Vec3 pLoc;

#ifdef STATUS_PRINTS_ENABLED
  float lastCompPercent = -11.0f, compPercent;
#endif

  int pw, ph;
  for (ph = 0; ph < img->height; ph++)
  {
#ifdef STATUS_PRINTS_ENABLED
    compPercent = 100.0f*(float)ph / (float)img->height;
    if (compPercent >= lastCompPercent + 10.0f)
    {
      lastCompPercent = compPercent;
      std::cout << "runTest " << compPercent << "% complete...\n";
    }
#endif

    for (pw = 0; pw < img->width; pw++)
    {
      img->getPixLoc(pLoc, pw, ph);

      Vec3 rayVec = (pLoc - eye);

      Pixel* activePixel = img->getPix(pw, ph);
      activePixel->loc3 = pLoc;

#ifdef DEBUG_GEN_PIXEL_REPORT
      dbgPixLog.isEn = ((pw == DEBUG_PIXEL_REPORT_X) && (ph == DEBUG_PIXEL_REPORT_Y));
#endif

      Ray activeRay = Ray(pLoc, rayVec, 0, 0.0f);
      traceRay(activeRay, activePixel->rgb, *this, sources, nSrc);
      activePixel->rgb = activePixel->rgb*PARAM_TOTAL_SCALE;

#ifdef DEBUG_GEN_PIXEL_REPORT
      dbgPixLog.storeInfo(this, activePixel->rgb);
      dbgPixLog.export(this, "dbgPixLog.txt");
    #ifdef MARK_DEBUG_PIXEL
      if (dbgPixLog.isEn)
      {
        activePixel->rgb = Rgb(1.0f, 1.0f, 1.0f);
      }
    #endif
#endif
    }
  }
#ifdef STATUS_PRINTS_ENABLED
  std::cout << "runTest complete.\n";
#endif
}

/* Triple pointer :( */
void TestWorld::CheckRayHitExt(Ray& ray, Object*** hitObjPtrArrayPtr, Vec3** hitPtr)
{
  /* Similar to checkRayHit. This takes an input ray and returns any objects this
     object deems it to hit. Use case: lower level object, ex. sphere, calls it's
     parent object (TestWorld) to see if a shadow ray hits anything. */

  /* For TestWorld, we'll check all known non-source objects for a collision. */
  int n, q = 0;

  /* hitPtr is set to NULL within checkRayHit to indicate no intersection. Otherwise hitPt will be
  modified to hold the closest intersection point. */
  Vec3 tempPt;

  for (n = 0; n < nObj; n++)
  {
    Vec3* tempPtr = &tempPt;
    objects[n]->checkRayHit(ray, &tempPtr);

    if (tempPtr != NULL)
    {    
      if (*hitObjPtrArrayPtr == NULL)
      {
        /* Once we know there's going to hits, allocate array to store results. NULL terminate the obj ptr array. */
       // *hitObjPtrArrayPtr = (Object**)malloc((nObj+1)*sizeof(Object*));
        *hitObjPtrArrayPtr = new Object* [nObj+1];
        *hitPtr = new Vec3[nObj];
      }

      /* TODO: Consider auto-sizing array. */
      /* Populate array of pointers to hit objects and their (first) hit location. */
      (*hitObjPtrArrayPtr)[q] = objects[n];
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

int TestWorld::getClosestObj(Ray& ray, Vec3& closestHit, float* dist2out)
{
  int n, closeIdx = -1;
  Vec3 hitPt;
  Ray tempRay, closeRay;
  float minDist2 = -1.0f;

  for (n = 0; n < nObj; n++)
  {
    /* TODO: better to define checkRayHit as not modifying ray using const. */
    tempRay = ray;
    Vec3* hitPtr = &hitPt;
    objects[n]->checkRayHit(tempRay, &hitPtr);

    if (hitPtr != NULL)
    {
      Vec3 diffVec = ray.loc3 - hitPt;

      float dist2 = diffVec.mag2();

      if ((dist2 < minDist2) || (minDist2 < 0))
      {
        minDist2 = dist2;
        closeIdx = n;
        closestHit = hitPt;

      }
    }
  }

  *dist2out = minDist2;
  return closeIdx;
}