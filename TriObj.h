#pragma once
#ifndef TRIOBJ_H
#define TRIOBJ_H

#include "Triangle.h"
#include "AabbTree.h"

class TriObj : public Object {
  public:
    /* Number of triangles in this object. */
    int nTri;
    /* Array of triangles. */
    Object** tris;
    /* Triangle pointers arranged in a nice kd tree for quick access. */
    Aabb3dTree triTree;

    TriObj();
    TriObj(std::string fileName, Vec3& a, Rgb& c, float i, ScaleParams s);
    ~TriObj();

    virtual void checkRayHit(Ray& ray, Vec3** hitPtr);
    virtual void traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc);
};

#endif //TRIOBJ_H
