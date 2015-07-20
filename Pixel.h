#pragma once
#ifndef PIXEL_H
#define PIXEL_H

#include "Vec3.h"
#include <iostream>

class Rgb {
  public:
    float r, g, b;
    Rgb();
    Rgb(float, float, float);
};

class Pixel {
  public:
    /* Pixel location */
    Vec3 loc3;
    /* Pixel HSV values */
    Rgb rgb;

    Pixel() {};
    Pixel(float, float, float, float, float, float);
};

class Image {
  public:
    /* Location of top left corner of image. */
    Vec3 loc3;
    /* Image dimensions (in pixel units). */
    int width, height;
    /* dimensions of each pixel. */
    float pWidth, pHeight;
    /* Image normal. */
    Vec3 norm;
    /* Pixels. */
    Pixel* pixels;

    Image() {};
    Image(Vec3&, Vec3&, int, int, float, float);
    void getPixLoc(Vec3& pLoc, int wIdx, int hIdx);
    Pixel* getPix(int wIdx, int hIdx);
    void exportBitmap(const char* fileName);
};

#endif //PIXEL_H