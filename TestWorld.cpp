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

  /* Form the object search kd tree. */
  objTree = Aabb3dTree(obj, nObjs, TOP_LEVEL_OBJECT_TREE_DEPTH);
}

TestWorld::~TestWorld()
{
  /* Delete kd-tree memory. */
  objTree.freeTreeMem();
}

/* Trace the ray's progress. Use kd-tree to find closest hit object. */
void TestWorld::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  Vec3 closestHit;
  float dist2;
  Object* closestObjPtr = objTree.root->getClosestObj(ray, closestHit, &dist2);

  if (closestObjPtr != NULL) /* We actually hit something. */
  {
    Ray tempRay = ray;
    tempRay.loc3 = closestHit;
    tempRay.dist2 = dist2;
    closestObjPtr->traceRay(tempRay, outRgb, *this, srcList, 1);
    outRgb = outRgb*closestObjPtr->sParams.totalScale;
  }
  else /* Miss: */
  {
    outRgb.r = outRgb.g = outRgb.b = 0.0f;
  }
}

void TestWorld::runTest(void)
{
  /* Location of active pixel. */
  Vec3 pLoc, pixLoc;

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
      pixLoc = pLoc;

      Pixel* activePixel = img->getPix(pw, ph);
      activePixel->loc3 = pLoc;

      /* Anti-aliasing supersampling loops over x and y supersample coords. The supersampling happens near the original
         pixel location - biased to the left/bottom a little, but not a big deal. */
      for (int sy = -(PARAM_LIN_SUPERSAMPLE_FACTOR - 1) / 2; sy < (PARAM_LIN_SUPERSAMPLE_FACTOR + 1) / 2; ++sy)
      {
        pixLoc.y = pLoc.y + (sy*(img->pHeight)) / PARAM_LIN_SUPERSAMPLE_FACTOR;
        for (int sx = -(PARAM_LIN_SUPERSAMPLE_FACTOR - 1) / 2; sx < (PARAM_LIN_SUPERSAMPLE_FACTOR + 1) / 2; ++sx)
        {
          pixLoc.x = pLoc.x + (sx*(img->pWidth)) / PARAM_LIN_SUPERSAMPLE_FACTOR;
          Vec3 rayVec = (pixLoc - eye);

#ifdef DEBUG_GEN_PIXEL_REPORT
          dbgPixLog.isEn = ((pw == DEBUG_PIXEL_REPORT_X) && (ph == DEBUG_PIXEL_REPORT_Y) &&
            (sx == 0) && (sy == 0));
#endif

          Ray activeRay = Ray(pLoc, rayVec, 0, 0.0f);
          Rgb tempRgb;
          traceRay(activeRay, tempRgb, *this, sources, nSrc);
          activePixel->rgb = activePixel->rgb + tempRgb;
        }
      }
      /* Average over all supersample anti-alias values. */
      activePixel->rgb = (activePixel->rgb*PARAM_TOTAL_SCALE)*(1.0f/(PARAM_LIN_SUPERSAMPLE_FACTOR*PARAM_LIN_SUPERSAMPLE_FACTOR));

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

