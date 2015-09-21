#pragma once
#ifndef AABBTREE_H
#define AABBTREE_H

#include "AABB.h"

class Aabb3dNode {
public:
  Aabb3dNode();
  ~Aabb3dNode();
  void buildNode(Object** inputObjs, int nObj, int lvl, int maxLvl, int& nNodes);
  /* Pointers to the "left" and "right" children of this node. */
  Aabb3dNode* left;
  Aabb3dNode* right;
  /* Bounding AABB boxes bounding the left/right children. */
  AABB leftBox;
  AABB rightBox;
  /* Pointers to objects contained at this node, if any. The node can have objects if it's a leaf of the tree
     or if an object would have been in both the left and right subtree (stored here instead to avoid duplication). */
  Object** objs;
  /* Number of pointers to objects in the objs array. */
  int numObj;
  /* Axis we're splitting for this node. */
  AxisEnum axis;

  /* Recursive method for finding closest hit to input ray. */
  Object* getClosestObj(Ray& ray, Vec3& closestHit, float* dist2out);
  /* Recursive method for getting all objects hit by input ray. */
  void getHitObjects(Ray& ray, std::vector<Object*> &hitObjs, std::vector<Vec3> &hitPts);
};

class Aabb3dTree {
public:
  int nLvls;
  int nNodes;
  Aabb3dNode* root;
  Aabb3dTree();
  Aabb3dTree(Object** inputObjs, int nObj, int nLvls);
  void freeTreeMem(void);
  ~Aabb3dTree();
};

#endif //AABBTREE_H