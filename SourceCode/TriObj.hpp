#pragma once
#ifndef TRIOBJ_H
#define TRIOBJ_H

#include "Triangle.hpp"
#include "AabbTree.hpp"

class TriObj : public Object {
  public:
    /* Number of triangles in this object. */
    int nTri;
    /* Array of triangles. */
    Object** tris;
    /* Triangle pointers arranged in a nice kd tree for quick access. */
    Aabb3dTree triTree;
    /* Flag indicating whether this TriObj was created from a cache file. If so, the destructor procedure will
       be slightly different. */
    bool isFromCache;
    /* If created from cache, nodeArrayPtr points to the start of the allocated node memory. If non-cached, this
       variable is meaningless. Note: should look into whether it's actually possible that the triTree base ptr is
       different from this address due to C++ padding magic. If not, this entry is redundant. */
    Aabb3dNode* nodeArrayPtr;
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

    /* Export/Import functions for caching the TriObj. Otherwise load times can be rather long. */
    void exportTriObj(std::string fileName);
    void import(std::string fileName);
};

#endif //TRIOBJ_H
