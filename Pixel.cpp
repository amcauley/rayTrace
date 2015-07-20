#include "Pixel.h"
#include "Bitmap.h"

Rgb::Rgb()
{
  r = g = b = 0;
}

Rgb::Rgb(float ah, float as, float av)
{
  r = ah;
  g = as;
  b = av;
}

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

Image::Image(Vec3& loc, Vec3& n, int w, int h, float pw, float ph)
{
  loc3 = loc;
  norm = n;
  width = w;
  height = h;
  pWidth = pw;
  pHeight = ph;
  pixels = new Pixel[w*h];
}

void Image::getPixLoc(Vec3& pLoc, int wIdx, int hIdx)
{
  /* Get the x,y,z location in the image corresponding to the pixel
  at "width" index wIdx and "height" index hIdx. */
  std::cout << "WARNING: Image::getPixLoc currently only works for images in the x,y plane!"; //3d orientation not yet supported

  pLoc.x = loc3.x + pWidth*wIdx;
  pLoc.y = loc3.y - pHeight*hIdx;
  pLoc.z = 0;
}

Pixel* Image::getPix(int wIdx, int hIdx)
{
  return &pixels[hIdx*height + wIdx];
}

void Image::exportBitmap(const char* fileName)
{
  writePixelsToBmp24(fileName, pixels, width, height);
}