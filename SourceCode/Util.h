#pragma once
#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <sys/stat.h>


/* Check if file exists. Based on http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c */ 
inline bool fileExists(const std::string& name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

#endif //UTIL_H