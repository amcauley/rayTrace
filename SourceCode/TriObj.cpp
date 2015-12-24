#include "TriObj.hpp"
#include "DbgLog.hpp"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <string>
#include "ObjCommon.hpp"
#include <queue>
#include "Util.hpp"

/* Helper function to get the name of the triObj from the input filename. If the input filename is something like
   x/y/z/<name>.txt, we'll return <name>. */
std::string getTriObjName(std::string fileName)
{
  std::string::iterator it;
  int charCnt = 0;
  /* Count backwards from end of string until we find a slash, then go forward until we hit a period. */
  for (it = fileName.end()-1; it != fileName.begin(); --it)
  {
    if ((*it == '\\') || (*it == '/'))
    {
      /* We found the slash, increment the iterator to point to the next character (start of triObjName). */
      ++it;
      break;
    }

  }

  std::string triObjName;

  while (*it != '.')
  {
    triObjName += *it;
    ++it;
  }

  return triObjName;
}

TriObj::TriObj() {};

TriObj::TriObj(std::string fileName, Vec3& a, Rgb& c, float i, ScaleParams s) :
  Object(a, c, i, s)
{

  std::string cachedFileName;
  //cachedFileName = "Z:/Documents/Visual Studio 2015/Projects/Project1/Project1/Objects/Cached/test.tcf";
  cachedFileName = "Z:/Documents/Visual Studio 2015/Projects/Project1/Project1/Objects/Cached/" +
                    getTriObjName(fileName) + ".tcf";

#ifdef USE_CACHE_BETA                   
  isFromCache = fileExists(cachedFileName);
#else
  isFromCache = false;
#endif
    
  if (isFromCache)
  {
    /* Use the cached version of the file. */
    import(cachedFileName);
  }
  else /* Cached filed doesn't exist. Build the object from scratch and also cache for future use. */
  {
    std::ifstream fs(fileName);
    std::string curLine;

    if (std::getline(fs, curLine))
    {
      std::cout << "Parsing TriObj: " << fileName << std::endl;
    }
    else
    {
      std::cout << "Error reading TriObj: " << fileName << std::endl;
      assert(0);
    }

    fs.clear();
    fs.seekg(0, std::ios::beg);

    int lineNum = 0;

    /* Start by counting the number of objects, sources, etc. */
    while (std::getline(fs, curLine))
    {
      ++lineNum;
    }

    /* Allocate room for all of the Triangle pointers. */
    tris = new Object*[lineNum];

    /* Go back to the beginning of the file. */
    fs.clear();
    fs.seekg(0, std::ios::beg);
    lineNum = 0;

    float minX, maxX, minY, maxY, minZ, maxZ;

    while (std::getline(fs, curLine))
    {
      std::istringstream lineStream(curLine);

      float x1, y1, z1, x2, y2, z2, x3, y3, z3;
      lineStream >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3;

      if (lineNum == 0)
      {
        minX = maxX = x1;
        minY = maxY = y1;
        minZ = maxZ = z1;
      }

      tris[lineNum++] = new Triangle(Vec3(x1 + loc3.x, y1 + loc3.y, z1 + loc3.z),
        Vec3(x2 + loc3.x, y2 + loc3.y, z2 + loc3.z),
        Vec3(x3 + loc3.x, y3 + loc3.y, z3 + loc3.z),
        rgb, sParams);

      minX = fminf(tris[lineNum - 1]->bbox->xl, minX);
      minY = fminf(tris[lineNum - 1]->bbox->yl, minY);
      minZ = fminf(tris[lineNum - 1]->bbox->zl, minZ);

      maxX = fmaxf(tris[lineNum - 1]->bbox->xh, maxX);
      maxY = fmaxf(tris[lineNum - 1]->bbox->yh, maxY);
      maxZ = fmaxf(tris[lineNum - 1]->bbox->zh, maxZ);
    }

    nTri = lineNum;

    bbox = new AABB(minX, maxX,
      minY, maxY,
      minZ, maxZ);

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

#ifdef USE_CACHE_BETA     
    /* Cache the file for future usage. */
    exportTriObj(cachedFileName);
#endif    
  } //End non-cached case 

#ifndef EN_PARALLEL_PROC
  hitTriPtr = NULL;
#endif
}

TriObj::~TriObj()
{
  /* If we created this TriObj from a cache, then the tree memory was allocated all at once instead of
     progressively as we added nodes. */
  if (isFromCache)
  {
    /* Set all left/right to NULL so we don't double free them during each node's individual destructor. Left/Right
       memory is freed when we delete nodeArrayPtr. */
    for (int k = 0; k < triTree.nNodes; ++k)
    {
      Aabb3dNode* currNode = &nodeArrayPtr[k];
      currNode->left = NULL;
      currNode->right = NULL;
    }
    delete[] nodeArrayPtr;
  }
  else
  {
    triTree.freeTreeMem();
  }

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
#ifdef EN_PARALLEL_PROC
    ray.lastHitObj = ray.lastHitSubObj = currTriPtr;
#else
    hitTriPtr = dynamic_cast<Triangle*>(currTriPtr);
#endif
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
#ifdef EN_PARALLEL_PROC
        ray.lastHitSubObj = currTriPtr;
#else
        hitTriPtr = dynamic_cast<Triangle*>(currTriPtr);
#endif
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

  /* If we're at max recursion depth, just exit now with the ambiant rgb value. */
  if (ray.depth >= MAX_RAY_DEPTH)
  {
    return;
  }

#ifdef EN_PARALLEL_PROC
  Vec3 triNorm = (dynamic_cast<Triangle*>(ray.lastHitSubObj))->norm;
#else
  /* Latch the norm before we recurse into any other rays (which could call checkHitRay and
  overwrite hitTriPtr. */
  Vec3 triNorm = hitTriPtr->norm;
#endif

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
    if (scale > 0.0f)
    {
      outRgb = outRgb + tempRgb*(sParams.shadowScale*scale);
    }
  }
}


/* Serialize this TriObj so we can quickly read it back in later program instances. */
void TriObj::exportTriObj(std::string fileName)
{
  std::cout << "Exporting " << fileName << "...\n";
  /* .tcf = TriObj Cached File */
  std::fstream fp(fileName, std::ios::out | std::ios::binary);

  fp
    //<< "\nloc3: "
    << loc3.x << " "
    << loc3.y << " "
    << loc3.z << " "
    //<< "\nbbox: "
    << bbox->xl << " "
    << bbox->xh << " "
    << bbox->yl << " "
    << bbox->yh << " "
    << bbox->zl << " "
    << bbox->zh << " "
    //<< rgb.r << " "
    //<< rgb.g << " "
    //<< rgb.b << " "
    //<< ior << " "
    //<< sParams.ambientScale << " "
    //<< sParams.glassScale << " "
    //<< sParams.mirrorScale << " "
    //<< sParams.shadowScale << " "
    //<< sParams.totalScale << " "
    //<< "\nnTri: "
    << nTri << " "
    //<< "\nnNodes: "
    << triTree.nNodes << " "
    //<< "\nnLvls: "
    << triTree.nLvls << " ";

  /* Breadth first traversal of nodes to get node info then triangle info in that node:
     per node storage: [<numObj> <leftPtr Idx> <rightPtr Idx> <tri[0]> ... <tri[numObj-1]>] */
  std::queue<Aabb3dNode*> nodeQ;
  nodeQ.push(triTree.root);

  int nodeIdx = 1; // counting the number of nodes we find. Every time we enqueue a node, increment this count.

  while (!nodeQ.empty())
  {
    //fp << "\n\nnode: ";
    Aabb3dNode* currNode = nodeQ.front();
    nodeQ.pop();

    //fp << "\nnumObj: ";
    fp << currNode->numObj << " ";
    //fp << "\nleft/right nodes: ";

    /* Left node */
    if (currNode->left != NULL)
    {
      fp << nodeIdx << " ";
      nodeQ.push(currNode->left);
      ++nodeIdx;
    }
    else
    {
      /* No left node. */
      fp << -1 << " ";
    }

    /* Right node */
    if (currNode->right != NULL)
    {
      fp << nodeIdx << " ";
      nodeQ.push(currNode->right);
      ++nodeIdx;
    }
    else
    {
      /* No right node. */
      fp << -1 << " ";
    }

    fp
      //<< "\naxis: "
      << (int)currNode->axis << " "
      //<< "\nleftBox: "
      << currNode->leftBox.xl << " "
      << currNode->leftBox.xh << " "
      << currNode->leftBox.yl << " "
      << currNode->leftBox.yh << " "
      << currNode->leftBox.zl << " "
      << currNode->leftBox.zh << " "
      //<< "\nrightBox: "
      << currNode->rightBox.xl << " "
      << currNode->rightBox.xh << " "
      << currNode->rightBox.yl << " "
      << currNode->rightBox.yh << " "
      << currNode->rightBox.zl << " "
      << currNode->rightBox.zh << " ";

    /* Store triangle info in this node. */
    for (int k = 0; k < currNode->numObj; ++k)
    {
      Triangle* currTri = dynamic_cast<Triangle*>(currNode->objs[k]);

      fp
        //<< "\nTri info\nloc3: "
        << currTri->loc3.x << " "
        << currTri->loc3.y << " "
        << currTri->loc3.z << " "
        //<< "\nnorm: "
        << currTri->norm.x << " "
        << currTri->norm.y << " "
        << currTri->norm.z << " "
        //<< "\nd: "
        << currTri->d << " "
        //<< "\nmajAxis: "
        << (int)currTri->normMajorAxis << " "
        //<< "\npts: "
        << currTri->pts[0].x << " "
        << currTri->pts[0].y << " "
        << currTri->pts[0].z << " "
        << currTri->pts[1].x << " "
        << currTri->pts[1].y << " "
        << currTri->pts[1].z << " "
        << currTri->pts[2].x << " "
        << currTri->pts[2].y << " "
        << currTri->pts[2].z << " "
        //<< "\nbbox: "
        << currTri->bbox->xl << " "
        << currTri->bbox->xh << " "
        << currTri->bbox->yl << " "
        << currTri->bbox->yh << " "
        << currTri->bbox->zl << " "
        << currTri->bbox->zh << " ";
    }

  }

  fp.close();
  std::cout << "Done exporting " << fileName << "\n";
}

void TriObj::import(std::string fileName)
{
  std::fstream fs(fileName, std::ios::in | std::ios::binary);

  fs >> loc3.x >> loc3.y >> loc3.z;

  float xl, xh, yl, yh, zl, zh;
  fs >> xl >> xh >> yl >> yh >> zl >> zh;
  bbox = new AABB(xl, xh, yl, yh, zl, zh);

  /* Allocate space for the Triangles. */
  fs >> nTri;
  tris = new Object*[nTri];

  /* Allocate space for the tree nodes. We'll populate this then form it into an actual tree type later. */
  int nNodes;
  fs >> nNodes;
  Aabb3dNode* nodeArray = new Aabb3dNode[nNodes];

  fs >> triTree.nLvls;
  triTree.nNodes = nNodes;

  int triIdx = 0;
  /* Loop through cache file, reconstructing nodes and triangles as we go. */
  for (int nodeIdx = 0; nodeIdx < nNodes; ++nodeIdx)
  {
    Aabb3dNode* currNodePtr = &nodeArray[nodeIdx];

    /* Get number of triangles in this node. */
    int nObjs;
    fs >> nObjs;
    currNodePtr->numObj = nObjs;
        
    currNodePtr->objs = new Object*[nObjs];

    int tempIdx;
    fs >> tempIdx;
    /* During construction of nodeArray, ptrs initialized to NULL. */
    if (tempIdx > 0) currNodePtr->left = &nodeArray[tempIdx];
    fs >> tempIdx;
    if (tempIdx > 0) currNodePtr->right = &nodeArray[tempIdx];

    int nodeAxis;
    fs >> nodeAxis;
    currNodePtr->axis = (AxisEnum)nodeAxis;

    fs
      >> currNodePtr->leftBox.xl
      >> currNodePtr->leftBox.xh
      >> currNodePtr->leftBox.yl
      >> currNodePtr->leftBox.yh
      >> currNodePtr->leftBox.zl
      >> currNodePtr->leftBox.zh
      >> currNodePtr->rightBox.xl
      >> currNodePtr->rightBox.xh
      >> currNodePtr->rightBox.yl
      >> currNodePtr->rightBox.yh
      >> currNodePtr->rightBox.zl
      >> currNodePtr->rightBox.zh;

    /* Read in this node's triangles. */
    for (int k = 0; k < nObjs; ++k)
    {
      tris[triIdx++] = new Triangle();
      Triangle* curTri = dynamic_cast<Triangle*>(tris[triIdx-1]);

      /* These Triangle values inherit from TriObj. */
      curTri->ior = ior;
      curTri->rgb = rgb;
      curTri->sParams = sParams;

      float xl, xh, yl, yh, zl, zh;
      int triMajAxis;

      fs
        >> curTri->loc3.x
        >> curTri->loc3.y
        >> curTri->loc3.z
        >> curTri->norm.x
        >> curTri->norm.y
        >> curTri->norm.z
        >> curTri->d
        >> triMajAxis
        >> curTri->pts[0].x
        >> curTri->pts[0].y
        >> curTri->pts[0].z
        >> curTri->pts[1].x
        >> curTri->pts[1].y
        >> curTri->pts[1].z
        >> curTri->pts[2].x
        >> curTri->pts[2].y
        >> curTri->pts[2].z
        >> xl
        >> xh
        >> yl
        >> yh
        >> zl
        >> zh;

      curTri->normMajorAxis = (AxisEnum)triMajAxis;
      curTri->bbox = new AABB(xl, xh, yl, yh, zl, zh);

      currNodePtr->objs[k] = curTri;
    }
  }

  triTree.root = &nodeArray[0];
  nodeArrayPtr = nodeArray;

  fs.close();
}