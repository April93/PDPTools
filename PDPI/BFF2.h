#ifndef BFF2_H
#define BFF2_H

#include "lodepng.h"
#include <iostream>

#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <tgmath.h>

typedef unsigned char u8;

typedef struct
{
  int red;
  int green;
  int blue;
  int alpha;
} color;

class BFF2 {
 public:
    BFF2();          // public constructor
    u8* decompress();
    void exportImage(char* outfilename);
    void importImage(char* filename);
    void printType();
    int encodeCommandByte(int bytesAffected, int extraTimesDisplayed);
    u8* compress(u8* imageb, int size, int& compSize);
    //BFF2(const S&);  // public copy constructor
    //virtual ~S(); // public virtual destructor
 //private:
    u8* header;
    int namelength;
    char* name;
 	u8* imgbuf; //Image byte buffer - raw
    int compressed; //Whether the byte buffer and the BFF2 is compressed.
    int imgsize; //Size of the byte buffer.
    int width; //Width of the image
    int height; //Height of the image
    int colorSize; //byte size of the color type
    int bff2type; //the color type
    color* colors; //pallete of colors if needed
    int colornum;
};

#endif
