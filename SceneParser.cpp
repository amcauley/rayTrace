#include "SceneParser.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <string>
#include "LightSources.h"
#include "Plane.h"

void sceneParser(const char* fileName, Object*** objs, int* nObj, Object*** srcs, int* nSrc, Image** img, Vec3** eye)
{
  std::ifstream fs(fileName);

  std::string curLine;

  /* Start by counting the number of objects, sources, etc. */
  while (std::getline(fs, curLine))
  {
    std::istringstream lineStream(curLine);
    std::string curWord;

    lineStream >> curWord;

    if (curWord == "I") /* Image: I locX locY locZ w h pixW pixH normX normY normZ */
    {
      float locX, locY, locZ, pixW, pixH, normX, normY, normZ;
      int w, h;
      lineStream >> locX >> locY >> locZ >> w >> h >> pixW >> pixH >> normX >> normY >> normZ;
      *img = new Image(Vec3(locX, locY, locZ), Vec3(normX, normY, normZ), w, h, pixW, pixH);
      std::cout << "Img initialized \n";
    }
    else if (curWord == "E") /* Eye: E locX locY locZ */
    {
      float locX, locY, locZ;
      lineStream >> locX >> locY >> locZ;
      *eye = new Vec3(locX, locY, locZ);
      std::cout << "Eye initialized \n";
    }
    else if (curWord == "SS") /* Simple Source: SS locX locY locZ rad R G B indRef totScale shadScale mirScale glasScale ambScale */ 
    {
      ++*nObj;
      ++*nSrc;
      std::cout << "Simple Src detected \n";
    }
    else if (curWord == "SP") /* Sphere: SP locX locY locZ rad R G B indRef totScale shadScale mirScale glasScale ambScale */
    {
      ++*nObj;
      std::cout << "Sphere detected \n";
    }
    else if (curWord == "PL") /* Plane: PL normX normY normZ offset R G B indRef totScale shadScale mirScale glasScale ambScale */  
    {
      ++*nObj;
      std::cout << "Plane detected \n";
    }
    else /* Default case: */
    {
      continue;
    }
  }

  /* Go over scene again and populate obj, src arrays. */
  *objs = new Object*[*nObj];
  *srcs = new Object*[*nSrc];
  int curObj = 0, curSrc = 0;

  fs.clear();
  fs.seekg(0, std::ios::beg);

  while (std::getline(fs, curLine))
  {
    std::istringstream lineStream(curLine);
    std::string curWord;

    lineStream >> curWord;

    if (curWord == "SS") /* Simple Source: SS locX locY locZ rad R G B indRef totScale shadScale mirScale glasScale ambScale */
    {
      float locX, locY, locZ, rad, R, G, B, indRef, totScale, shadScale, mirScale, glasScale, ambScale;
      lineStream >> locX >> locY >> locZ >> rad >> R >> G >> B >> indRef >> totScale >> shadScale >> mirScale >> glasScale >> ambScale;
      (*srcs)[curSrc++] = new SimpleSource(Vec3(locX, locY, locZ), rad, Rgb(R, G, B)*(1.0f / 255.0f), indRef, ScaleParams(totScale, shadScale, mirScale, glasScale, ambScale));
      (*objs)[curObj++] = (*srcs)[curSrc - 1];
      std::cout << "Simple Src created \n";
    }
    else if (curWord == "SP") /* Sphere: SP locX locY locZ rad R G B indRef totScale shadScale mirScale glasScale ambScale */
    {
      float locX, locY, locZ, rad, R, G, B, indRef, totScale, shadScale, mirScale, glasScale, ambScale;
      lineStream >> locX >> locY >> locZ >> rad >> R >> G >> B >> indRef >> totScale >> shadScale >> mirScale >> glasScale >> ambScale;
      (*objs)[curObj++] = new Sphere(Vec3(locX, locY, locZ), rad, Rgb(R, G, B)*(1.0f / 255.0f), indRef, ScaleParams(totScale, shadScale, mirScale, glasScale, ambScale));
      std::cout << "Sphere created \n";
    }
    else if (curWord == "PL") /* Plane: PL normX normY normZ offset R G B indRef totScale shadScale mirScale glasScale ambScale */
    {
      float normX, normY, normZ, offset, R, G, B, indRef, totScale, shadScale, mirScale, glasScale, ambScale;
      lineStream >> normX >> normY >> normZ >> offset >> R >> G >> B >> indRef >> totScale >> shadScale >> mirScale >> glasScale >> ambScale;
      (*objs)[curObj++] = new Plane(Vec3(normX, normY, normZ), offset, Rgb(R, G, B)*(1.0f / 255.0f), indRef, ScaleParams(totScale, shadScale, mirScale, glasScale, ambScale));
      std::cout << "Plane created \n";
    }
    else /* Default case: */
    {
      continue;
    }
  }
}