#include "TriObj.h"
#include "DbgLog.h"
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

  hitTriPtr = NULL;
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
    float minDist2Tri2 = (hitPtrs[0] - ray.loc3).mag2();
    hitTriPtr = dynamic_cast<Triangle*>(currTriPtr);
    **hitPtr = hitPtrs[0];
    /* A source can't occlude itself from this sphere. List is NULL terminated. */
    while (currTriPtr != NULL)
    {
      /* Return the closest point to the ray. */
      float dist2Tri2 = (hitPtrs[n] - ray.loc3).mag2();
      if (dist2Tri2 < minDist2Tri2)
      {
        minDist2Tri2 = dist2Tri2;
        **hitPtr = hitPtrs[n];
        hitTriPtr = dynamic_cast<Triangle*>(currTriPtr);
      }
      currTriPtr = hitObjPtrArrayPtr[++n];
    }

    //std::cout << "TriObj hit\n";
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

  if (hitTriPtr == NULL)
  {
    assert(0); //We should have set hitTriPtr during a previous call to checkRayHit.
  }
  Vec3 triNorm = hitTriPtr->norm;
  //std::cout << "triNorm " << triNorm.x << " " << triNorm.y << " " << triNorm.z << std::endl;

  /* If we're at max recursion depth, just exit now with the ambiant rgb value. */
  if (ray.depth >= MAX_RAY_DEPTH)
  {
    return;
  }

  Rgb tempRgb;

  /*~~~~~~~~~~ Shadow Ray Proc ~~~~~~~~~~*/
  if (sParams.shadowScale > 0.0f)
  {
    Vec3 shadowDir = (srcList[0]->loc3 - ray.loc3);
    Ray shadowRay = Ray(ray.loc3, shadowDir, ray.depth + 1, 0.0f);

    Object** objList = NULL;
    Vec3* objHitPts = NULL;

    /* Generate list of objects hit by the shadow ray and the coordinates of intersections. */
    callingObj.CheckRayHitExt(shadowRay, &objList, &objHitPts);

    if (objList != NULL)
    {
      int n = 0;
      Object* currObjPtr = objList[0];
      /* Check for occlusions. */
      while (currObjPtr != NULL)
      {
        /* A source can't occlude itself. */
        if (currObjPtr == srcList[0])
        {
          currObjPtr = objList[++n];
          continue;
        }
        /* While checking for occlusions, we'll disregard shadow ray intersections with
        current object itself if the intersection isn't different (within tolerances)
        of the initial ray/object intersection. */
        if (currObjPtr != this)
        {
          Vec3 dist2Src = srcList[0]->loc3 - ray.loc3;
          Vec3 dist2Obj = objHitPts[n] - ray.loc3;
          if (dist2Src.mag2() > dist2Obj.mag2())
          {
            delete[] objList; delete[] objHitPts;
            return;
          }
        }
        else /* Ray hit ourself */
        {
          delete[] objList; delete[] objHitPts;
          return;
        }
        currObjPtr = objList[++n];
      }
      delete[] objList; delete[] objHitPts;
    }

    /* No obstructions on the shadow ray. Calculate angle between normal vec and shadow
    ray, then scale rgb intensity accordingly (closer to normal = more intense). Normal
    direction is from center of sphere to the impact ray intersection pt. Angle is in radians,
    from 0 to Pi, but since we don't expect angles greater than Pi/2 (otherwise would have
    occluded ourself), scale by ((Pi/2)-angle). */

    /* We want these to be hittable from either face, so flip the normal vector locally if needed. */
    Vec3 tempNorm = triNorm;
    if (triNorm.dot(shadowDir) < 0.0f)
    {
      tempNorm = triNorm*(-1.0f);
    }

    float angle = shadowDir.getAngle(tempNorm);
    float scale = 1.0f - (angle / (float)M_PI_2);

    //std::cout << "ray loc " << ray.loc3.x << " " << ray.loc3.y << " " << ray.loc3.z << std::endl;
    //std::cout << "ray dir " << ray.vec3.x << " " << ray.vec3.y << " " << ray.vec3.z << std::endl;
    //std::cout << "shadowDir " << shadowDir.x << " " << shadowDir.y << " " << shadowDir.z << std::endl;
    //std::cout << "angle " << angle << " scale " << scale << std::endl;

    /* Due to float math, we might have a few boundary cases of negative scaling. Set them to
    be occluded. */
    if (scale < 0)
    {
      return;
    }
    else if (scale > 1.0f) /* Guard against rounding issues by clamping max scale factor. */
    {
      scale = 1.0f;
    }

#ifdef DEBUG_GEN_PIXEL_REPORT
    dbgPixLog.nextLvl(RAY_TYPE_SHADOW);
#endif

    srcList[0]->traceRay(shadowRay, tempRgb, callingObj, srcList, 0);

#ifdef DEBUG_GEN_PIXEL_REPORT
    dbgPixLog.storeInfo(this, tempRgb);
    dbgPixLog.restoreLvl();
#endif

    outRgb = outRgb + tempRgb*(sParams.shadowScale*scale);
  }
}