#include "LightSources.h"

void SimpleSource::traceRay(Ray ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  /* For simple source, just return its color. Don't care about other sources. */
  outRgb = rgb;
}