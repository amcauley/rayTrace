#pragma once
#ifndef PICFRAME_H
#define PICFRAME_H

#include "Triangle.hpp"
#include "Vec3.hpp"
#include "Ray.hpp"
#include "Rgb.hpp"

class PicFrame : public Object {
public:
  /* 3 (out of 4) vertices of the PicFrame rectangle, must be listed in clockwise order starting from what we consider
     the top left corner of the bitmap. */
  Vec3 pts[3];

  /* BMP filename. */
  std::string bmpName;

  /* Image dimensions (in pixels). */
  int bmpWidth, bmpHeight;

  /* Image dimensions (normal world units, not pixels). */
  float imgWidth, imgHeight;

  /* Array of RGB values corresponding to the image. */
  Rgb *rgbVals;

  /* Triangles used to create the rectangular frame. The two triangles correspond to those created by splitting the rectangular
     frame from one vertex to the opposite. */
  Triangle tris[2];

  /* Vectors defining the top edge (horizontal) and right edge (vertical) of the frame. */
  Vec3 widthVec, heightVec;

  PicFrame();
  ~PicFrame();
  PicFrame(std::string bmpName, Vec3 pt1, Vec3 pt2, Vec3 pt3);

  virtual void checkRayHit(Ray& ray, Vec3** hitPtr);
  virtual void traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc);
};

#endif //PICFRAME_H
