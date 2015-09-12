#pragma once
#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include "Pixel.h"

/* Define structures. */
typedef struct {      //file header
  uint16_t type;      //2 bytes
  uint32_t size;      //4 bytes
  uint16_t reserved1;	//2 bytes
  uint16_t reserved2;	//2 bytes
  uint32_t offset;    //4 bytes
} BMP_HEADER;         //TOTAL: 14 bytes

typedef struct {             //info header
  uint32_t headersize;       //4 bytes
  uint32_t width;            //4 bytes
  uint32_t height;           //4 bytes
  uint16_t colorplanes;      //2 bytes
  uint16_t bitsperpixel;     //2 bytes
  uint32_t compression;      //4 bytes
  uint32_t bmpsize;          //4 bytes
  uint32_t horizontalres;    //4 bytes
  uint32_t verticalres;      //4 bytes
  uint32_t colors;           //4 bytes
  uint32_t importantcolors;  //4 bytes
} BMP_INFO_HEADER;           //TOTAL: 40 bytes

typedef struct {  //pixel struct
  uint8_t blue;   //1 byte
  uint8_t green;  //1 byte
  uint8_t red;    //1 byte
} BMP_PIXEL;      //TOTAL: 3 bytes

extern void writePixelsToBmp24(std::string fileName, Pixel* pixArray, int w, int h);

#endif //BITMAP_H