#include "Pixel.h"
#include "Bitmap.h"
#include <algorithm>

Pixel::Pixel(float ax, float ay, float az,
  float ah, float as, float av)
{
  loc3.x = ax;
  loc3.y = ay;
  loc3.z = az;

  rgb.r = ah;
  rgb.g = as;
  rgb.b = av;
}

Image::Image(Vec3& loc, Vec3& n, int w, int h, float pw, float ph) :
  loc3(loc),
  norm(n),
  width(w),
  height(h),
  pWidth(pw),
  pHeight(ph)
{
  pixels = new Pixel[w*h];
}

Image::~Image()
{
  delete pixels;
}

void Image::autoScale(void)
{
  float maxVal = pixels[0].rgb.r;

  int pW, pH;
  /* First pass - find min/max vals. */
  for (pH = 0; pH < height; pH++)
  {
    for (pW = 0; pW < width; pW++)
    {
      Rgb* pixVal = (Rgb*)&pixels[pH*width + pW].rgb;
      maxVal = std::max(pixVal->r, maxVal);
      maxVal = std::max(pixVal->g, maxVal);
      maxVal = std::max(pixVal->b, maxVal);
    }
  }

  /* Don't quite scale to max to help prevent potential overflows. */
  float scaleVal = (254.0f / 255.0f)/maxVal;
  std::cout << "Image autoscale by " << scaleVal << "\n";

  /* Second pass - scale values. */
  for (pH = 0; pH < height; pH++)
  {
    for (pW = 0; pW < width; pW++)
    {
      int pixIdx = pH*width + pW;
      pixels[pixIdx].rgb = pixels[pixIdx].rgb*scaleVal;

      if (pixels[pixIdx].rgb.r < 0.0f)
      {
        std::cout << "scale issue\n";
      }
    }
  }
}

void Image::getPixLoc(Vec3& pLoc, int wIdx, int hIdx)
{
  /* Get the x,y,z location in the image corresponding to the pixel
  at "width" index wIdx and "height" index hIdx. */
  
  //TODO: 3d orientation not yet supported. Code assumes image is in x,y plane.

  pLoc.x = loc3.x + pWidth*wIdx;
  pLoc.y = loc3.y - pHeight*hIdx;
  pLoc.z = loc3.z;
}

Pixel* Image::getPix(int wIdx, int hIdx)
{
  return &pixels[hIdx*width + wIdx];
}

void Image::exportBitmap(const char* fileName)
{
  writePixelsToBmp24(fileName, pixels, width, height);
}