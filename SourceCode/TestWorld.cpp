#include "TestWorld.hpp"
#include "DbgLog.hpp"

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
  float lastCompPercent = -6.0f, compPercent;
#endif

  int pw, ph;
#ifndef REVERSE_PIX_ORDER
  for (ph = 0; ph < img->height; ph++)
#else
  for (ph = img->height-1; ph >= 0; ph--)
#endif
  {
#ifdef STATUS_PRINTS_ENABLED
    compPercent = 100.0f*(float)ph / (float)img->height;
    if (compPercent >= lastCompPercent + 5.0f)
    {
      lastCompPercent = compPercent;
      std::cout << "runTest " << compPercent << "% complete...\n";
    }
#endif

#ifndef REVERSE_PIX_ORDER
    for (pw = 0; pw < img->width; pw++)
#else
    for (pw = img->width-1; pw >= 0; pw--)
#endif
    {
      //std::cout << "pw " << pw << " ph " << ph << std::endl;

      img->getPixLoc(pLoc, pw, ph);
      pixLoc = pLoc;

      Pixel* activePixel = img->getPix(pw, ph);
      activePixel->loc3 = pLoc;

      /* Anti-aliasing supersampling loops over x and y supersample coords. The supersampling happens near the original
         pixel location - biased to the left/bottom a little, but not a big deal. */
      for (int sy = -((int)PARAM_LIN_SUPERSAMPLE_FACTOR - 1) / 2; sy < ((int)PARAM_LIN_SUPERSAMPLE_FACTOR + 1) / 2; ++sy)
      {
        pixLoc.y = pLoc.y + (sy*(img->pHeight)) / PARAM_LIN_SUPERSAMPLE_FACTOR;
        for (int sx = -((int)PARAM_LIN_SUPERSAMPLE_FACTOR - 1) / 2; sx < ((int)PARAM_LIN_SUPERSAMPLE_FACTOR + 1) / 2; ++sx)
        {
          pixLoc.x = pLoc.x + (sx*(img->pWidth)) / PARAM_LIN_SUPERSAMPLE_FACTOR;
          Vec3 rayVec = (pixLoc - eye);

#ifdef DEBUG_GEN_PIXEL_REPORT
          dbgPixLog.isEn = ( (pw == DEBUG_PIXEL_REPORT_X) && (ph == DEBUG_PIXEL_REPORT_Y) );
          if (dbgPixLog.isEn)
          {
            std::cout << "Dbg pix logging enabled\n";
          }
#endif

          Ray activeRay = Ray(pLoc, rayVec, 0, 0.0f);
          Rgb tempRgb;
          traceRay(activeRay, tempRgb, *this, sources, nSrc);
          activePixel->rgb = activePixel->rgb + tempRgb;
        }
      }
      /* Average over all supersample anti-alias values. */
      activePixel->rgb = (activePixel->rgb*PARAM_TOTAL_SCALE)*(1.0f/(PARAM_LIN_SUPERSAMPLE_FACTOR*PARAM_LIN_SUPERSAMPLE_FACTOR));


#ifdef MARK_DEBUG_PIXEL
#if DEBUG_GEN_PIXEL_REPORT
      if (dbgPixLog.isEn)
#else
      if ( (pw == DEBUG_PIXEL_REPORT_X) && (ph == DEBUG_PIXEL_REPORT_Y) )
#endif
      {
        activePixel->rgb = Rgb(1.0f, 1.0f, 1.0f);
      }
#endif

#ifdef DEBUG_GEN_PIXEL_REPORT
      dbgPixLog.storeInfo(this, activePixel->rgb);
      dbgPixLog.exportDbg(this, "Output/dbgPixLog.txt");
#endif
    }
  }
#ifdef STATUS_PRINTS_ENABLED
  std::cout << "runTest complete.\n";
#endif
}

/* Search object tree for all hit objects. */
void TestWorld::CheckRayHitExt(Ray& ray, std::vector<Object*> &hitObjs, std::vector<Vec3> &hitPts)
{
  objTree.root->getHitObjects(ray, hitObjs, hitPts);
}
