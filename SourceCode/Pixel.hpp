#pragma once
#ifndef PIXEL_H
#define PIXEL_H

#include <iostream>
#include "Vec3.hpp"
#include "Rgb.hpp"

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
    ~Image();
    Image(Vec3&, Vec3&, int, int, float, float);
    void getPixLoc(Vec3& pLoc, int wIdx, int hIdx);
    Pixel* getPix(int wIdx, int hIdx);
    void exportBitmap(std::string fileName);
    void autoScale(void);
};

#endif //PIXEL_H