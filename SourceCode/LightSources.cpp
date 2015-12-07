#include "LightSources.hpp"
#include "DbgLog.hpp"

void SimpleSource::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  /* For simple source, just return its color. Don't care about other sources. */
  outRgb = rgb;

#ifdef DEBUG_GEN_PIXEL_REPORT
  dbgPixLog.nextLvl(RAY_TYPE_AMBIENT);
  dbgPixLog.storeInfo(this, rgb);
  dbgPixLog.restoreLvl();
#endif
}