#pragma once
#ifndef DBGLOG_H
#define DBGLOG_H

#include <vector>
#include "Rgb.h"
#include "Params.h"
#include "Object.h"
#include "TestWorld.h"

typedef enum
{
  RAY_TYPE_SHADOW = 0,
  RAY_TYPE_MIRROR,
  RAY_TYPE_GLASS,
  RAY_TYPE_AMBIENT,
  NUM_RAY_TYPES
} RayTypes_t;

class LogNode {
public:
  Rgb pixRgb;
  const Object* hitObj; /* The object that is producing this pixRgb val. */
  LogNode();
};

class DbgPixLog {
  private:
    int nLvls;
    int curLvl;
    int curBase;
    int* lvlSizeFactors;
    int* path;
    LogNode* vals;
  public:
    bool isEn; /* Flag that logging is currently active (for current pixel). */
    DbgPixLog(int nLvls);
    ~DbgPixLog();
    void nextLvl(RayTypes_t nextPath);
    void restoreLvl(void);
    void storeInfo(const Object* obj, Rgb& rgbValue);
    void exportDbg(const TestWorld* environment, const std::string fileName);
    void writeDbgEntry(std::ofstream& fs, LogNode& logNode);
    void writeLogLayer(std::ofstream& fs, int depth, int& idx);
};

#ifdef DEBUG_GEN_PIXEL_REPORT
extern DbgPixLog dbgPixLog;
#endif

#endif //DBGLOG_H
