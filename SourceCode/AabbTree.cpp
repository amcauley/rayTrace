#include "AabbTree.hpp"
#include <assert.h>

Aabb3dNode::Aabb3dNode() :
  left(NULL),
  right(NULL),
  objs(NULL)
{}

Aabb3dNode::~Aabb3dNode()
{
  if (left != NULL) delete left;
  if (right != NULL) delete right;
  if (objs != NULL) delete[] objs;
}

void Aabb3dNode::buildNode(Object** inputObjs, int nObj, int lvl, int maxLvl, int& nNodes)
{
  axis = (AxisEnum)(lvl % (int)NUM_AXIS);
  /* If not at leaf, divide objects into left/right groups and create next level down. If we're down to a single
  object, we'll treat this node as a leaf even if there are more levels available for use. */
  if ((lvl < maxLvl) && (nObj > 1))
  {
    float inf = std::numeric_limits<float>::infinity();
    Vec3 minCoords(inf, inf, inf);
    Vec3 maxCoords(-inf, -inf, -inf);

    for (int k = 0; k < nObj; ++k)
    {
      minCoords.x = std::fminf(minCoords.x, inputObjs[k]->bbox->xl);
      minCoords.y = std::fminf(minCoords.y, inputObjs[k]->bbox->yl);
      minCoords.z = std::fminf(minCoords.z, inputObjs[k]->bbox->zl);
      maxCoords.x = std::fmaxf(maxCoords.x, inputObjs[k]->bbox->xh);
      maxCoords.y = std::fmaxf(maxCoords.y, inputObjs[k]->bbox->yh);
      maxCoords.z = std::fmaxf(maxCoords.z, inputObjs[k]->bbox->zh);
    }

    /* For now, just split the extrema in half. Later we may want to consider splitting at the median location. */
    switch (axis)
    {
      case AXIS_X_ENUM:
      {
        leftBox.xl = minCoords.x;
        if (maxCoords.x == inf)
        {
          /* Currently the only way we'll get infinity in the max coord is from an (infinite) plane, which should also have
             an infinite extent in the negative direction. Set the split point at coord 0, since float math with infinites
             won't give this answer. */
          assert(minCoords.x == -inf);
          leftBox.xh = 0.0f;
        }
        else leftBox.xh = minCoords.x + (maxCoords.x - minCoords.x)*0.5f;

        rightBox.xl = leftBox.xh;
        rightBox.xh = maxCoords.x;

        leftBox.yl = rightBox.yl = minCoords.y;
        leftBox.yh = rightBox.yh = maxCoords.y;

        leftBox.zl = rightBox.zl = minCoords.z;
        leftBox.zh = rightBox.zh = maxCoords.z;

        break;
      }
      case AXIS_Y_ENUM:
      {
        leftBox.yl = minCoords.y;
        if (maxCoords.y == inf)
        {
          /* Currently the only way we'll get infinity in the max coord is from an (infinite) plane, which should also have
          an infinite extent in the negative direction. Set the split point at coord 0, since float math with infinites
          won't give this answer. */
          assert(minCoords.y == -inf);
          leftBox.yh = 0.0f;
        }
        else leftBox.yh = minCoords.y + (maxCoords.y - minCoords.y)*0.5f;
        
        rightBox.yl = leftBox.yh;
        rightBox.yh = maxCoords.y;

        leftBox.xl = rightBox.xl = minCoords.x;
        leftBox.xh = rightBox.xh = maxCoords.x;

        leftBox.zl = rightBox.zl = minCoords.z;
        leftBox.zh = rightBox.zh = maxCoords.z;

        break;
      }
      case AXIS_Z_ENUM:
      {
        leftBox.zl = minCoords.z;
        if (maxCoords.z == inf)
        {
          /* Currently the only way we'll get infinity in the max coord is from an (infinite) plane, which should also have
          an infinite extent in the negative direction. Set the split point at coord 0, since float math with infinites
          won't give this answer. */
          assert(minCoords.z == -inf);
          leftBox.zh = 0.0f;
        }
        else leftBox.zh = minCoords.z + (maxCoords.z - minCoords.z)*0.5f;
        
        rightBox.zl = leftBox.zh;
        rightBox.zh = maxCoords.z;

        leftBox.xl = rightBox.xl = minCoords.x;
        leftBox.xh = rightBox.xh = maxCoords.x;

        leftBox.yl = rightBox.yl = minCoords.y;
        leftBox.yh = rightBox.yh = maxCoords.y;

        break;
      }
      default:
      {
        assert(0);
      }
    }

    /* Now we'll work on forming the left and right object arrays. */
    int nCurr = 0, nLeft = 0, nRight = 0;
    /* Destination array - 0 if object becomes part of current node, 1 for left node, 2 for right. */
    char* destArray = new char[nObj];
    Object* currObj = NULL;
    Object** leftObjs, **rightObjs;

    /* Find the left node objects. */
    for (int k = 0; k < nObj; ++k)
    {
      currObj = inputObjs[k];
      if (leftBox.intersects(*(currObj->bbox)))
      {
        /* If we also overlap the rightBox, we'll store this object in the current node instead
           of pushing it down to both left and right leaves. */
        if (rightBox.intersects(*(currObj->bbox)))
        {
          destArray[k] = 0;
          ++nCurr;
        }
        else /* Object is only part of left branch. */
        {
          destArray[k] = 1;
          ++nLeft;
        }
      }
      else /* Object isn't part of left (or current) branch, so must be part of right: */
      {
        destArray[k] = 2;
        ++nRight;
      }
    }

    /* Copy into actual current/left/right object arrays. */
    objs = leftObjs = rightObjs = NULL;
    if(nCurr) objs = new Object*[nCurr];
    if(nLeft) leftObjs = new Object*[nLeft];
    if(nRight) rightObjs = new Object*[nRight];
    int currIdx = 0, leftIdx = 0, rightIdx = 0;
    for (int k = 0; k < nObj; ++k)
    {
      switch (destArray[k])
      {
        case 0:
        {
          objs[currIdx++] = inputObjs[k];
          break;
        }
        case 1:
        {
          leftObjs[leftIdx++] = inputObjs[k];
          break;
        }
        case 2:
        {
          rightObjs[rightIdx++] = inputObjs[k];
          break;
        }
        default:
          assert(0);
      }
    }

    delete[] destArray;

    numObj = nCurr;

    /* Objects are sorted into appropriate arrays. Recurse to form lower levels of the tree. This
    dynamic memory will be delete during this current node's destructor. */
    if (nLeft > 0)
    {
      left = new Aabb3dNode;
      left->buildNode(leftObjs, nLeft, lvl + 1, maxLvl, ++nNodes);
    }
    if (nRight > 0)
    {
      right = new Aabb3dNode;
      right->buildNode(rightObjs, nRight, lvl + 1, maxLvl, ++nNodes);
    }

    /* Done with remaining local dynamic memory for this node. */
    delete[] leftObjs;
    delete[] rightObjs;
  }
  /* Else, we're at a leaf node, so store the objects. Maybe just use memcopy. */
  else
  {
    numObj = nObj;
    objs = new Object*[nObj];
    for (int k = 0; k < nObj; ++k)
    {
      objs[k] = inputObjs[k];
    }
  }
}

Aabb3dTree::Aabb3dTree() {}

Aabb3dTree::Aabb3dTree(Object** inputObjs, int nObj, int nLvls) :
  nLvls(nLvls)
{
  std::cout << "Building top lvl kd-tree\n";
  root = new Aabb3dNode;
  nNodes = 1; //number of nodes in the tree. Initialize to 1 (for the root).
  root->buildNode(inputObjs, nObj, 0, nLvls, nNodes);
}


/* The default destructor won't free memory, or the temporary object created to
   initialize the tree will just delete it immediately after. Instead, use the
   freeTreeMem method to explicitly delete the tree. */
Aabb3dTree::~Aabb3dTree() {}

void Aabb3dTree::freeTreeMem(void)
{
  std::cout << "Deleting top lvl kd-tree mem\n";
  if (root != NULL) delete root;
}

/* Recursive method for finding closest hit to input ray. */
Object* Aabb3dNode::getClosestObj(Ray& ray, Vec3& closestHit, float* dist2out)
{
  Object *closestObjPtr = NULL, *tempObj = NULL;
  Vec3 hitPt;
  Ray tempRay, closeRay;
  float minDist2 = -1.0f, dist2;

  /* Get the closest object looking only at the current node. */
  for (int k = 0; k < numObj; ++k)
  {
    Vec3* hitPtr = &hitPt;
    objs[k]->checkRayHit(ray, &hitPtr);

    if (hitPtr != NULL)
    {
      Vec3 diffVec = ray.loc3 - hitPt;

      dist2 = diffVec.mag2();

      if ((dist2 < minDist2) || (minDist2 < 0))
      {
        minDist2 = dist2;
        closestObjPtr = objs[k];
        closestHit = hitPt;
      }
    }
  }

  /* Get the closest node from the left path, if we hit it (and it exists). */
  if (left != NULL)
  {
    Vec3* hitPtr = &hitPt;
    leftBox.checkRayHit(ray, &hitPtr);

    if (hitPtr != NULL) /* We hit the left box, so recurse and get the closest point in it, if any objects are hit. */
    {
      Vec3 closeHit;
      tempObj = left->getClosestObj(ray, closeHit, &dist2);

      if ((tempObj != NULL) && ((dist2 < minDist2) || (minDist2 < 0)))
      {
        minDist2 = dist2;
        closestObjPtr = tempObj;
        closestHit = closeHit;
      }
    }
  }

  /* Get closest point from the right subtree. */
  if (right != NULL)
  {
    Vec3* hitPtr = &hitPt;
    rightBox.checkRayHit(ray, &hitPtr);

    if (hitPtr != NULL) /* We hit the right box, so recurse and get the closest point in it. */
    {
      Vec3 closeHit;
      tempObj = right->getClosestObj(ray, closeHit, &dist2);

      if ((tempObj != NULL) && ((dist2 < minDist2) || (minDist2 < 0)))
      {
        minDist2 = dist2;
        closestObjPtr = tempObj;
        closestHit = closeHit;
      }
    }
  }

  *dist2out = minDist2;
  return closestObjPtr;
}

void Aabb3dNode::getHitObjects(Ray& ray, std::vector<Object*> &hitObjs, std::vector<Vec3> &hitPts)
{
  /* Similar to checkRayHit. This takes an input ray and returns any objects this
  object deems it to hit. Use case: lower level object, ex. sphere, calls its
  parent object (TestWorld) to see if a shadow ray hits anything. */

  /* For TestWorld, we'll search the kd tree for object collisions. */
  Vec3 hitPt;

  for (int k = 0; k < numObj; ++k)
  {
    Vec3* tempPtr = &hitPt;
    objs[k]->checkRayHit(ray, &tempPtr);

    if (tempPtr != NULL)
    {

      hitObjs.push_back(objs[k]);
      hitPts.push_back(hitPt);
    }
  }

  /* Recursively call for left/right sub-trees. */
  if (left != NULL)
  {
    Vec3* tempHitPtr = &hitPt;
    leftBox.checkRayHit(ray, &tempHitPtr);

    if (tempHitPtr != NULL) /* We hit the left box, so recurse. */
    {
      left->getHitObjects(ray, hitObjs, hitPts);
    }
  }

  if (right != NULL)
  {
    Vec3* tempHitPtr = &hitPt;
    rightBox.checkRayHit(ray, &tempHitPtr);

    if (tempHitPtr != NULL) /* We hit the left box, so recurse. */
    {
      right->getHitObjects(ray, hitObjs, hitPts);
    }
  }
}