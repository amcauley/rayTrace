#pragma once
#ifndef SCENEPARSER_H
#define SCENEPARSER_H

#include "Object.h"

extern void sceneParser(std::string fileName, Object*** objs, int* nObj, Object*** srcs, int* nSrc, Image** img, Vec3** eye);

#endif //SCENEPARSER_H