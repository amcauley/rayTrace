#include <fstream>
#include <iostream>
#include "Bitmap.hpp"
#include <algorithm>
#include <assert.h>

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

void writePixelsToBmp24(std::string fileName, Pixel* pixArray, int w, int h)
{
  /* Take input array of pixels and format them into a bitmap. Write to file. */
  std::ofstream fs;
  fs.open(fileName, std::ios::out | std::ios::binary);

  if (fs.is_open())
  {
    std::cout << "Opened output file for writing: " << fileName << std::endl;
  }
  else
  {
    std::cout << "Failed to open output file: " << fileName << std::endl;
    assert(0);
  }

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

  head.type = BMP_FILE_ID;
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
  /* Bitmap pixel order is start from bottom left of image, then left to right within a row, then row by row
     towards the top of the image. */
  for (y = h-1; y >= 0; y--)
  {
    int x;
    for (x = 0; x < w; x++)
    {
      int pixIdx = y*w + x;

      /* Debug section, useful to highlight pixels that meet certain params. */
#if 0
      if ((x == 252) && (y == 135))
      {
        std::cout << pixArray[pixIdx].rgb.r << " "
                  << pixArray[pixIdx].rgb.g << " "
                  << pixArray[pixIdx].rgb.b << "\n";
        //pixArray[pixIdx].rgb.b = 1.0f;
        //pixArray[pixIdx].rgb.g = 1.0f;
        //pixArray[pixIdx].rgb.r = 1.0f;
      }
#endif
      /* End debug section. */

      writeBytes(fs, (uint32_t)(pixArray[pixIdx].rgb.b * 255), 1);
      writeBytes(fs, (uint32_t)(pixArray[pixIdx].rgb.g * 255), 1);
      writeBytes(fs, (uint32_t)(pixArray[pixIdx].rgb.r * 255), 1);
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


void readPixelsFromBmp24(const char* file, Rgb **RGBarray, int *width, int *height)
{
  /* Take in 24bit bmp and output array of Rgb values. This code adapted from an earlier C project. Would probably be
     good to convert this to C++ later just for consistency with the rest of this codebase. */

  /* declare variables */
  FILE *fpin;           //file pointer
  BMP_HEADER header;    //file header
  BMP_INFO_HEADER info; //info header
  BMP_PIXEL pixel;      //pixel struct
  long int i, n;   //loop counters

  /* open input file in binary mode */
  if ((fpin = fopen(file, "rb")) == NULL) {
    printf("Cannot open file %s.\n", file);
    return;
  }

  /* read and display header info */
  /* NOTE: Since some compilers force BMP_HEADER struct to be 16 bytes
     (multiple of 4) instead of 14 bytes, read data into each FILE_HEADER
     element individually. */

  fread(&header.type, sizeof(header.type), 1, fpin);
  fread(&header.size, sizeof(header.size), 1, fpin);
  fread(&header.reserved1, sizeof(header.reserved1), 1, fpin);
  fread(&header.reserved2, sizeof(header.reserved2), 1, fpin);
  fread(&header.offset, sizeof(header.offset), 1, fpin);
  fread(&info, sizeof(BMP_HEADER), 1, fpin);

  /* make sure file is a 24 bit bitmap */
  if (header.type != BMP_FILE_ID)
  {
    printf("\nFile is not a 24 bit bitmap.\n");
    return;
  }

  /* allocate memory for data - must be unallocated by calling function. */
  *RGBarray = new Rgb[info.height*info.width];

  /* begin reading data. */
  fseek(fpin, header.offset, SEEK_SET); //set file position to beginning of image data

  for (i = info.height-1; i >= 0; i--)       //iterate through pixel rows keeping in mind BMP data starts lower left corner of image
  {
    for (n = 0; n < info.width; n++)        //read each pixel in a row
    {
      fread(&pixel, sizeof(pixel), 1, fpin);
      (*RGBarray)[i*info.width + n].r = pixel.red;
      (*RGBarray)[i*info.width + n].g = pixel.green;
      (*RGBarray)[i*info.width + n].b = pixel.blue;
    }

    if ((info.width * 3) % 4 != 0)        //absorb padding bytes at the end of each row
    {
      for (n = 0; n < 4 - (info.width * 3) % 4; n++)
      {
        fread(&pixel.blue, sizeof(pixel.blue), 1, fpin);  //pixel.blue used because it is the correct size			
      }
    }
  }

  *width = info.width;
  *height = info.height;

  /* close file */
  fclose(fpin);
}