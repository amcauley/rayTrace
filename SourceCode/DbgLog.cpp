#include "DbgLog.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>

LogNode::LogNode() :
  hitObj(0)
{
}

DbgPixLog::DbgPixLog(int nLvls) :
  nLvls(nLvls),
  curLvl(0),
  isEn(false)
{
  path = new int[nLvls]; /* value at idx n = path taken to enter level n. */
  path[0] = 0;
  /* Normally geometric summation would have a +1 in the numerator, but we neglect it to avoid
     float -> int conversion flooring the result to a too-small value. */ 
  int nVals = (int)(pow((int)NUM_RAY_TYPES, nLvls) / (NUM_RAY_TYPES - 1));
  vals = new LogNode[nVals];

  lvlSizeFactors = new int[nLvls]; /* value at idx n = size of each subtree of lvl n-1. */
  int m, summ = 1, prod = 1;
  for (m = nLvls - 1; m >= 0; m--)
  {
    lvlSizeFactors[m] = summ;
    prod *= (int)NUM_RAY_TYPES;
    summ += prod;
  }
}

DbgPixLog::~DbgPixLog()
{
  delete[] path;
  delete[] lvlSizeFactors;
  delete[] vals;
}

/* Call when entering a new lvl of recursion. */
void DbgPixLog::nextLvl(RayTypes_t nextPath)
{
  curLvl++;
  if (curLvl >= nLvls) return;
  curBase += ((int)nextPath) * lvlSizeFactors[curLvl] + 1; /* +1 for cost of leaving base. */
  path[curLvl] = (int)nextPath;
}

/* Call to restore state from recursive call. */
void DbgPixLog::restoreLvl(void)
{
  curLvl--;
  if (curLvl >= nLvls - 1) return;
  if (curLvl < 0)
  {
    std::cout << "curLvl < 0\n";
    assert(0);
  }
  curBase -= path[curLvl+1] * lvlSizeFactors[curLvl+1] + 1;
  /* Path will get overwritten later. */
}

void DbgPixLog::storeInfo(const Object* obj, Rgb& rgbValue)
{
  if ((!isEn) || (curLvl >= nLvls))
  {
    return;
  }

  LogNode* valPtr = &vals[curBase];

  valPtr->pixRgb = rgbValue;
  valPtr->hitObj = obj;
}

void DbgPixLog::writeDbgEntry(std::ofstream& fs, LogNode& logNode)
{
  std::string str = "(";
  str += std::to_string(logNode.pixRgb.r) + ", " +
    std::to_string(logNode.pixRgb.g) + ", " +
    std::to_string(logNode.pixRgb.b) + ") : " +
    std::to_string((uintptr_t)logNode.hitObj);

  fs << str << std::endl;
}

void DbgPixLog::writeLogLayer(std::ofstream& fs, int depth, int& idx)
{
  std::string str = "";
  str += "(" + std::to_string(depth) + ", " + std::to_string(idx) + ")";
  int k;
  for (k = 0; k < depth; k++)
  {
    str += "-----";
  }
  fs << str;

  /* Write out current node info, then recurse. */
  writeDbgEntry(fs, vals[idx++]);

  if (depth < nLvls - 1)
  {
    /* Recurse over all constituent rays. */
    for (k = 0; k < (int)NUM_RAY_TYPES; k++)
    {
      writeLogLayer(fs, depth + 1, idx);
    }
  }
}

void DbgPixLog::exportDbg(const TestWorld* environment, const std::string fileName)
{
  if (!isEn)
  {
    return;
  }

  /* Take dbg info and write it, formatted, to dbg log. */
  std::ofstream fs;
  fs.open(fileName, std::ios::out);

  /* Pixel info. */
  fs << "Pixel (X, Y):\n(" << DEBUG_PIXEL_REPORT_X << ", " << DEBUG_PIXEL_REPORT_Y << ")\n";

  /* Print Test World pointer/identifier. */
  fs << "\nTest World:\n";
  fs << std::to_string((uintptr_t)environment) << "\n";

  /* Print obj/src pointer/identifier info. */
  int k;
  fs << "\nSources:\n";
  for (k = 0; k < environment->nSrc; k++)
  {
    fs << std::to_string(k) << " --> " << std::to_string((uintptr_t)environment->sources[k]) << "\n";
  }

  fs << "\nObjects:\n";
  for (k = 0; k < environment->nObj; k++)
  {
    fs << std::to_string(k) << " --> " << std::to_string((uintptr_t)environment->objects[k]) << "\n";
  }
  fs << "\n==========================\n\n";

  /* Recursively print out formatted log entries. */
  int writeIdx = 0;
  writeLogLayer(fs, 0, writeIdx);

  fs.close();
}

#ifdef DEBUG_GEN_PIXEL_REPORT
/* Global dbg log. */
DbgPixLog dbgPixLog(MAX_RAY_DEPTH+1);
#endif