#include "Params.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <assert.h>

/* Define params and set to default values. Can override in config.txt if desired. */
std::string SCENE_PATH = SCENE_PATH_DEFAULT;
unsigned int MAX_RAY_DEPTH = MAX_RAY_DEPTH_DEFAULT;
std::string OUTPUT_IMG = OUTPUT_IMG_DEFAULT;
unsigned int TOP_LEVEL_OBJECT_TREE_DEPTH = TOP_LEVEL_OBJECT_TREE_DEPTH_DEFAULT;
unsigned int TRI_TREE_DEPTH = TRI_TREE_DEPTH_DEFAULT;
unsigned int PARAM_LIN_SUPERSAMPLE_FACTOR = PARAM_LIN_SUPERSAMPLE_FACTOR_DEFAULT;

void getParams(std::string fileName)
{
  std::ifstream fs(fileName);
  std::string curLine;

  if (std::getline(fs, curLine))
  {
    std::cout << "Parsing cfg: " << fileName << std::endl;
  }
  else
  {
    std::cout << "Error reading cfg: " << fileName << std::endl;
    assert(0);
  }

  fs.clear();
  fs.seekg(0, std::ios::beg);

  /* Start by counting the number of objects, sources, etc. */
  while (std::getline(fs, curLine))
  {
    std::istringstream lineStream(curLine);
    std::string curWord;

    lineStream >> curWord;

    if (curWord == "SCENE_PATH")
    {
      while (lineStream.peek() == ' ') lineStream.get(); //eat leading whitespace
      std::getline(lineStream, SCENE_PATH);
      std::cout << "SCENE_PATH: " << SCENE_PATH << std::endl;
    }
    else if (curWord == "MAX_RAY_DEPTH")
    {
      lineStream >> MAX_RAY_DEPTH;
      std::cout << "MAX_RAY_DEPTH: " << MAX_RAY_DEPTH << std::endl;
    }
    else if (curWord == "OUTPUT_IMG")
    {
      while (lineStream.peek() == ' ') lineStream.get(); //eat leading whitespace
      std::getline(lineStream, OUTPUT_IMG);
      std::cout << "OUTPUT_IMG: " << OUTPUT_IMG << std::endl;
    }
    else if (curWord == "TOP_LEVEL_OBJECT_TREE_DEPTH")
    {
      lineStream >> TOP_LEVEL_OBJECT_TREE_DEPTH;
      std::cout << "TOP_LEVEL_OBJECT_TREE_DEPTH: " << TOP_LEVEL_OBJECT_TREE_DEPTH << std::endl;
    }
    else if (curWord == "TRI_TREE_DEPTH")
    {
      lineStream >> TRI_TREE_DEPTH;
      std::cout << "TRI_TREE_DEPTH: " << TRI_TREE_DEPTH << std::endl;
    }
    else if (curWord == "PARAM_LIN_SUPERSAMPLE_FACTOR")
    {
      lineStream >> PARAM_LIN_SUPERSAMPLE_FACTOR;
      std::cout << "PARAM_LIN_SUPERSAMPLE_FACTOR: " << PARAM_LIN_SUPERSAMPLE_FACTOR << std::endl;
    }
    else /* Default case: */
    {
      continue;
    }
  }
}