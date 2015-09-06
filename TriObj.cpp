#include "TriObj.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <string>

TriObj::TriObj() {};

TriObj::TriObj(std::string fileName, Vec3& a, Rgb& c, float i, ScaleParams s) :
  Object(a, c, i, s)
{
  std::ifstream fs(fileName);
  std::string curLine;

  int lineNum = 0;

  /* Start by counting the number of objects, sources, etc. */
  while (std::getline(fs, curLine))
  {
    ++lineNum;
  }

  /* Allocate room for all of the Triangle pointers. */
  tris = new Object* [lineNum];

  /* Go back to the beginning of the file. */
  fs.clear();
  fs.seekg(0, std::ios::beg);
  lineNum = 0;

  float minX, maxX, minY, maxY, minZ, maxZ;

  while (std::getline(fs, curLine))
  {
    std::istringstream lineStream(curLine);

    float x1, y1, z1, x2, y2, z2, x3, y3, z3;
    lineStream >> x1 >> y1 >> z1 >> x2 >> y2 >>z2 >>x3 >> y3 >> z3;
    
    if (lineNum == 0)
    {
      minX = maxX = x1;
      minY = maxY = y1;
      minZ = maxZ = z1;
    }

    tris[lineNum++] = new Triangle(  Vec3(x1+loc3.x, y1+loc3.y, z1+loc3.z),
                                     Vec3(x2+loc3.x, y2+loc3.y, z2+loc3.z), 
                                     Vec3(x3+loc3.x, y3+loc3.y, z3+loc3.z), 
                                     rgb, sParams);

    minX = fminf(tris[lineNum - 1]->bbox->xl, minX);
    minY = fminf(tris[lineNum - 1]->bbox->yl, minY);
    minZ = fminf(tris[lineNum - 1]->bbox->zl, minZ);

    maxX = fmaxf(tris[lineNum - 1]->bbox->xh, maxX);
    maxY = fmaxf(tris[lineNum - 1]->bbox->yh, maxY);
    maxZ = fmaxf(tris[lineNum - 1]->bbox->zh, maxZ);
  }

  nTri = lineNum;

  bbox = new AABB( minX, maxX,
                   minY, maxY,
                   minZ, maxZ );

  triTree = Aabb3dTree(tris, nTri, TRI_TREE_DEPTH);
}

TriObj::~TriObj()
{
  triTree.freeTreeMem();
  delete[] tris;
}

void TriObj::checkRayHit(Ray& ray, Vec3** hitPtr)
{

  Object** hitObjPtrArrayPtr = NULL;
  Vec3* hitPtrs = NULL;

  int objIdx = 0;
  triTree.root->getHitObjects(ray, &hitObjPtrArrayPtr, &hitPtrs, &objIdx, nTri);

  if (hitObjPtrArrayPtr != NULL)
  {
    int n = 0;
    Object* currTriPtr = (hitObjPtrArrayPtr)[0];
    float minDist2Tri2 = (currTriPtr->loc3 - ray.loc3).mag2();
    /* A source can't occlude itself from this sphere. List is NULL terminated. */
    while (currTriPtr != NULL)
    {
      /* Return the closest point to the ray. */
      Vec3 dist2Tri2 = hitPtrs[n] - ray.loc3;
      if (dist2Tri2.mag2() < minDist2Tri2)
      {
        **hitPtr = hitPtrs[n];
      }
      currTriPtr = hitObjPtrArrayPtr[++n];
    }
    /* Delete the array of hit triangles/pts. */
    delete[] hitObjPtrArrayPtr; delete[] hitPtrs;
    return;
  }

  /* No hit detected, so set hitPtr to NULL. */
  *hitPtr = NULL;
}

void TriObj::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  /* Start with ambient contribution. */
  outRgb = rgb*sParams.ambientScale;
}