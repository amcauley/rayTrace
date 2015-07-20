#include <fstream>
#include <iostream>
#include "Bitmap.h"

void writeBytes(std::ofstream& fs, uint32_t dataBytes, int numBytes)
{
  int currByte;
  char tempChar;
  for (currByte = 0; currByte < numBytes; currByte++)
  {
    tempChar = (dataBytes >> (currByte * 8)) & 0xFF;
    fs.write(&tempChar, 1);
  }
}

void writePixelsToBmp24(const char* fileName, Pixel* pixArray, int w, int h)
{
  /* Take input array of pixels and format them into a bitmap. Write to file. */
  std::ofstream fs;
  fs.open(fileName, std::ios::out | std::ios::binary);

  BMP_HEADER head;
  BMP_INFO_HEADER info;

  /* Bitmap rows are padded to be multiples of 4 bytes. */
  int rowPadBytes = 0;
  int rowPadMod = (w * 3) % 4;
  if (rowPadMod)
  {
    rowPadBytes = 4 - rowPadMod;
  }

  int rowSizeBytes = w*sizeof(BMP_PIXEL) + rowPadBytes;


  /* Compiler is forcing struct members to be word-aligned. Write bytes individually
  to get around this. Could also prob use some compiler directive/pragma to force alignment
  behavior, but that doesn't sound very portable. */

  head.type = 19778;
  head.size = 54 + rowSizeBytes*h;
  head.reserved1 = 0;
  head.reserved2 = 0;
  head.offset = 54;

  info.headersize = 40;
  info.width = w;
  info.height = h;
  info.colorplanes = 1;
  info.bitsperpixel = 24;
  info.compression = 0;
  info.bmpsize = 0;
  info.horizontalres = 0;
  info.verticalres = 0;
  info.colors = 0;
  info.importantcolors = 0;

  writeBytes(fs, (uint32_t)head.type, 2);
  writeBytes(fs, (uint32_t)head.size, 4);
  writeBytes(fs, (uint32_t)head.reserved1, 2);
  writeBytes(fs, (uint32_t)head.reserved2, 2);
  writeBytes(fs, (uint32_t)head.offset, 4);

  writeBytes(fs, (uint32_t)info.headersize, 4);
  writeBytes(fs, (uint32_t)w, 4);
  writeBytes(fs, (uint32_t)info.height, 4);
  writeBytes(fs, (uint32_t)info.colorplanes, 2);
  writeBytes(fs, (uint32_t)info.bitsperpixel, 2);
  writeBytes(fs, (uint32_t)info.compression, 4);
  writeBytes(fs, (uint32_t)info.bmpsize, 4);
  writeBytes(fs, (uint32_t)info.horizontalres, 4);
  writeBytes(fs, (uint32_t)info.verticalres, 4);
  writeBytes(fs, (uint32_t)info.colors, 4);
  writeBytes(fs, (uint32_t)info.importantcolors, 4);

  int y;
  for (y = 0; y < h; y++)
  {
    int x;
    for (x = 0; x < w; x++)
    {
      int pixIdx = y*w + x;

      //std::cout << (y*w + x);
      //std::cout << ": ";

      writeBytes(fs, (uint32_t)(pixArray[pixIdx].rgb.b *255), 1);
      writeBytes(fs, (uint32_t)(pixArray[pixIdx].rgb.g *255), 1);
      writeBytes(fs, (uint32_t)(pixArray[pixIdx].rgb.r *255), 1);
    }
    /* Handle padding. */
    if (rowPadBytes)
    {
      BMP_PIXEL pix;
      pix.blue = 0;
      int p;
      for (p = 0; p < rowPadBytes; p++)
      {
        writeBytes(fs, (uint32_t)pix.blue, 1);
      }
    }
  }

  fs.close();
}