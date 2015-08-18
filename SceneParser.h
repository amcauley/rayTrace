#pragma once
#ifndef SCENEPARSER_H
#define SCENEPARSER_H

#include "Object.h"

extern void sceneParser(const char* fileName, Object*** objs, int* nObj, Object*** srcs, int* nSrc, Image** img, Vec3** eye);

#endif //SCENEPARSER_H