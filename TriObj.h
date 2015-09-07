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

    /* The closest object/triangle discovered during the previous call to checkRayHit. This prevents us
       from needing to search the kd triangle tree twice - once during checkRayHit and then later during
       traceRay (so we can get the normal vector for the hit triangle). The assumption here is that checkRayHit
       will always be called just before traceRay for that same triangle. */
    Triangle* hitTriPtr;

    TriObj();
    TriObj(std::string fileName, Vec3& a, Rgb& c, float i, ScaleParams s);
    ~TriObj();

    virtual void checkRayHit(Ray& ray, Vec3** hitPtr);
    virtual void traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc);
};

#endif //TRIOBJ_H
