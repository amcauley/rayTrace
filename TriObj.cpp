#include "TriObj.h"
#include "DbgLog.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <string>
#include "ObjCommon.h"

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
                                     rgb, sParams );

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

  /* Force all triangles to have outward-facing normals. We do this by checking how many faces each
     normal passes through (the face itself is excluded from this count). If it's even (0, 2, 4, ...),
     then it's outward facing (even numbers arise from passing into and then out of the object). If the number
     is odd, then we're exiting the object (and posibly into/out multiple times). */

  for (int k = 0; k < nTri; ++k)
  {
    Triangle* triPtr = dynamic_cast<Triangle*>(tris[k]);

    Object** hitObjPtrArrayPtr = NULL;
    Vec3* hitPtrs = NULL;

    //int objIdx = 0;
    //triTree.root->getHitObjects(Ray(triPtr->loc3, triPtr->norm, 0, 0), &hitObjPtrArrayPtr, &hitPtrs, &objIdx, nTri);
    std::vector<Object*> hitObjs;
    std::vector<Vec3> hitPts;
    triTree.root->getHitObjects(Ray(triPtr->loc3, triPtr->norm, 0, 0), hitObjs, hitPts);
    /* If inward facing, flip direction. */
    if (hitObjs.size() % 2 == 0)
    {
      triPtr->norm = triPtr->norm * (-1.0f);
      triPtr->d *= -1.0f;
    }
  }

  hitTriPtr = NULL;
}

TriObj::~TriObj()
{
  triTree.freeTreeMem();
  delete[] tris;
}

void TriObj::checkRayHit(Ray& ray, Vec3** hitPtr)
{
  std::vector<Object*> hitObjs;
  std::vector<Vec3> hitPts;
  triTree.root->getHitObjects(ray, hitObjs, hitPts);

  int nObj = hitObjs.size();
  if (nObj)
  {
    Object* currTriPtr = hitObjs[0];
    float minDist2Tri2 = (hitPts[0] - ray.loc3).mag2();
    hitTriPtr = dynamic_cast<Triangle*>(currTriPtr);
    **hitPtr = hitPts[0];
    /* A source can't occlude itself from this sphere. List is NULL terminated. */
    for (int n = 1; n < nObj; ++n)
    {
      currTriPtr = hitObjs[n];
      /* Return the closest point to the ray. */
      float dist2Tri2 = (hitPts[n] - ray.loc3).mag2();
      if (dist2Tri2 < minDist2Tri2)
      {
        minDist2Tri2 = dist2Tri2;
        **hitPtr = hitPts[n];
        hitTriPtr = dynamic_cast<Triangle*>(currTriPtr);
      }
    }
    return;
  }

  /* No hit detected, so set hitPtr to NULL. */
  *hitPtr = NULL;
}

void TriObj::traceRay(Ray& ray, Rgb& outRgb, Object& callingObj, Object** srcList, int nSrc)
{
  /* Start with ambient contribution. */
  outRgb = rgb*sParams.ambientScale;

  /* Latch the norm before we recurse into any other rays (which could call checkHitRay and
     overwrite hitTriPtr. */
  Vec3 triNorm = hitTriPtr->norm;

  /* If we're at max recursion depth, just exit now with the ambiant rgb value. */
  if (ray.depth >= MAX_RAY_DEPTH)
  {
    return;
  }

  /*~~~~~~~~~~ Reflection (Mirror) / Refraction (Glass) Ray Proc ~~~~~~~~~~*/
  float mScale, gScale;
  mScale = sParams.mirrorScale;
  gScale = sParams.glassScale;

  bool runM, runG;
  runM = mScale > 0.0f;
  runG = gScale > 0.0f;

  if (runM || runG)
  {
    float R;
    float distanceScaling = powf(2.0f, -sqrt(ray.dist2)*0.25f);
    Rgb reflRgb, refrRgb;

    R = commonReflRefr(runM, runG, srcList, ray, callingObj, triNorm, ior, reflRgb, refrRgb);

    outRgb = outRgb + reflRgb*R*mScale + refrRgb*(1.0f - R)*gScale;
  }

  /*~~~~~~~~~~ Shadow Ray Proc ~~~~~~~~~~*/
  if (sParams.shadowScale > 0.0f)
  {
    Rgb tempRgb;
    float scale;
    scale = commonShadowTrace(srcList, ray, callingObj, triNorm, tempRgb);
    outRgb = outRgb + tempRgb*(sParams.shadowScale*scale);
  }
}