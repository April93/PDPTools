#include "lodepng.h"
#include "BFF2.h"
#include <iostream>

#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <tgmath.h>

using namespace std;

u8* decompress(u8* imgbuff, int imgsize, int width, int height, int colorSize);

void outputBytes(u8* bytes, int size)
{
  //cout << "0x";
  for(int i = 0; i < size; i++)
  {
    printf("%02x", bytes[i]);
    if(i%2 == 1){ cout << " ";}
  }
}

int bytesToInt(u8* bytes, int length=4)
{
  int Int32 = 0;

  for(int l = 0; l < length; l++)
    Int32 = (Int32 << 8) + bytes[l];
  /*Int32 = (Int32 << 8) + bytes[1];
  Int32 = (Int32 << 8) + bytes[2];
  Int32 = (Int32 << 8) + bytes[3];*/
  return Int32;
}

u8* intToBytes(int n)
{
  u8* bytes = new u8[4];
  bytes[0] = (char)(n >> 24) & 0xFF;
  bytes[1] = (char)(n >> 16) & 0xFF;
  bytes[2] = (char)(n >> 8) & 0xFF;
  bytes[3] = (char)n & 0xFF;
  //cout << (int)bytes[0] << (int)bytes[1] << (int)bytes[2] << (int)bytes[3] << endl;
  return bytes;
}
u8* intToBytesBE(int n)
{
  u8* bytes = new u8[4];
  bytes[3] = (char)(n >> 24) & 0xFF;
  bytes[2] = (char)(n >> 16) & 0xFF;
  bytes[1] = (char)(n >> 8) & 0xFF;
  bytes[0] = (char)n & 0xFF;
  //cout << (int)bytes[0] << (int)bytes[1] << (int)bytes[2] << (int)bytes[3] << endl;
  return bytes;
}

int bytesToIntBE(u8* bytes, int length=4)
{
  int Int32 = 0;

  for(int l = 1; l <= length; l++)
    Int32 = (Int32 << 8) + bytes[length-l];
  /*Int32 = (Int32 << 8) + bytes[1];
  Int32 = (Int32 << 8) + bytes[2];
  Int32 = (Int32 << 8) + bytes[3];*/
  return Int32;
}


//g++ lodepng.cpp example_encode.cpp -ansi -pedantic -Wall -Wextra -O3
void decodeOneStep(const char* filename, std::vector<unsigned char>& out, unsigned& width, unsigned& height)
{
  unsigned error = lodepng::decode(out, width, height, filename);
  //if there's an error, display it
  if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

//Encode from raw pixels to disk with a single function call
//The image argument has width * height RGBA pixels or width * height * 4 bytes
void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height)
{
  //Encode the image
  unsigned error = lodepng::encode(filename, image, width, height);
  //if there's an error, display it
  if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}




//saves image to filename given as argument. Warning, this overwrites the file without warning!
int main(int argc, char *argv[])
{
  const char* infilename = argc > 1 ? argv[1] : "test.bif";
  //BIF stuff
  FILE* inFile = fopen(infilename, "rb");

  //Get file size
  fseek(inFile, 0, SEEK_END);
  int size = ftell(inFile);
  fseek(inFile, 0, SEEK_SET);
  //Variable to hold data
  u8* buff = new u8[size];
  //Load the data
  fread(buff, 1, size, inFile);
  //Done with the file, so close
  fclose(inFile);
  int index = 0;

  //3FIB check
  u8* temp = new u8[4];
  for(int i = 0; i < 4; i++) { temp[i] = buff[index]; index++; }
  if(strcmp((char*)temp, "3FIB") != 0)
  {
    printf("Not BIF file");
    outputBytes(temp, 4);
    return 0;
  }

  int compressed = buff[5];
  cout << (compressed==0?"Not ":"") << "Compressed." << endl;

  //Not sure what this is...
  u8* bifHeaderStuff = new u8[8];
  for(int i = 0; i < 8; i++) { bifHeaderStuff[i] = buff[index]; index++; }
  //index+=8;

  for(int i = 0; i < 4; i++) { temp[i] = buff[index]; index++; }
  int imagenum = bytesToIntBE(temp);
  std::cout << "BFF2s: " << imagenum << std::endl;

  for(int i = 0; i < 4; i++) { temp[i] = buff[index]; index++; }
  int namelength = bytesToIntBE(temp);
  std::cout << "Name Length: " << namelength << std::endl;

  char* name = new char[namelength+1];
  for(int i = 0; i < namelength; i++) { name[i] = (char)buff[index]; cout << (char)name[i]; index++; }
  cout << endl;

  int headerLength = index;
  cout << "Header Length: " << headerLength << endl;


  int offnum = imagenum;
  int offsets[offnum];
  int pnt = index;
  int ofs = 0;
  while (pnt < size - 8)
  {
    for(int i = 0; i < 4; i++) { temp[i] = buff[pnt]; pnt++; }
    if(strcmp((char*)temp, "BFF2") == 0) {
      offsets[(ofs++)] = (pnt - 4 - 12);
      //cout << pnt-4-12 << endl;
    }
    pnt -= 3;
  }
  //cout << ofs << " " << offnum << endl;
  offsets[offnum] = size;
  //offsets[ofs] = size;

  std::vector<BFF2> bff2s;

  for(int b = 0; b < offnum; b++)
  {
    BFF2* firstimg = new BFF2();
    //BFF2 stuffs
    index = offsets[b];
    index+=12; 
    for(int i = 0; i < 4; i++) { temp[i] = buff[index]; index++; }
    if(strcmp((char*)temp, "BFF2") != 0)
    {
      printf("Not BFF2");
      return 0;
    }
    //int bff2type = buff[index+3];
    firstimg->bff2type = buff[index+3];
    firstimg->printType();

    index+=8;

    u8* bff2header = new u8[24];
    for(int i = 0; i < 24; i++) { bff2header[i] = buff[index-24+i]; }
    firstimg->header = bff2header;

    //outputBytes(firstimg->header, 24);

    for(int i = 0; i < 4; i++) { temp[i] = buff[index]; index++; }
    int height = bytesToInt(temp);
    for(int i = 0; i < 4; i++) { temp[i] = buff[index]; index++; }
    int width = bytesToInt(temp);
    int colorSize = 1;
    if(firstimg->bff2type == 0x55) //RGBA8888
    { width = width/4; colorSize = 4; }
    if(firstimg->bff2type == 0x54) //RGBA5551
    { width = width/2; colorSize = 2; }
    if(firstimg->bff2type == 0x24) //LA8
    {width = width/2; colorSize = 2; }

    firstimg->height = height;
    firstimg->width = width;
    firstimg->colorSize = colorSize;

    cout << "Width/Height: W:" << firstimg->width << " H:" << firstimg->height << endl;


    for(int i = 0; i < 4; i++) { temp[i] = buff[index]; index++; }
    int namelength2 = bytesToInt(temp);
    std::cout << "Name Length2: " << namelength2 << std::endl;

    char* name2 = new char[namelength2+1];
    for(int i = 0; i < namelength2; i++) { name2[i] = (char)buff[index]; cout << (char)name2[i]; index++; }
    firstimg->name = name2;
    firstimg->namelength = namelength2;
    cout << endl;

    //Init colors if needed
    color* colors;
    int colornum = 0;
    if(firstimg->bff2type == 0x33)
    {
      for(int i = 0; i < 4; i++) { temp[i] = buff[index]; index++; }
      colornum = bytesToInt(temp);

      colors = new color[colornum];

      for(int i = 0; i < colornum; i++)
      {
        color temp;
        temp.red = (int)buff[index];
        temp.green = (int)buff[index+1];
        temp.blue = (int)buff[index+2];
        temp.alpha = (int)buff[index+3];
        colors[i] = temp;
        index+=4;
      }

      firstimg->colors = colors;
      firstimg->colornum = colornum;

      std::cout << "Colors: " << colornum << std::endl;
    }

    //generate some image

    int isize = offsets[b+1]-index;
    u8* imgtmpbuf = new u8[isize];
    for(int x = index; x < offsets[b+1]; x++){ imgtmpbuf[x-index] = buff[x]; }

    firstimg->compressed = compressed;
    firstimg->imgsize = isize;
    firstimg->imgbuf = imgtmpbuf;

    bff2s.push_back(*firstimg);
    //firstimg->importImage((char*)"test.png");

    //firstimg->exportImage((char*)"test.png");
  }

  string cmd = "";

  while(cmd != "quit")
  {
    cin >> cmd;
    if(cmd == "exportall")
    {
      for(int b = 0; b < offnum; b++)
      {
        string s = string(bff2s[b].name)+".png";
        cout << "exporting " << s << endl;
        bff2s[b].exportImage((char*)s.c_str());
      }
    }
    if(cmd == "import")
    {
      int b = 0;
      string infile = "";
      cin >> b >> infile;
      cout << b << "=" << infile << endl;
      bff2s[b].importImage((char*)infile.c_str());
    }
    if(cmd == "save")
    {
      string outfile = "";
      cin >> outfile;
      cout << "Saving to " << outfile << endl;
      //bff2s[b].importImage((char*)infile.c_str());
      ofstream sfile(outfile.c_str());
      sfile << "3FIB";

      for(int w = 0; w < 8; w++)
        sfile << bifHeaderStuff[w];
      u8* toph = intToBytesBE(imagenum);
      for(int w = 0; w < 4; w++)
        sfile << toph[w];
      toph = intToBytesBE(namelength);
      for(int w = 0; w < 4; w++)
        sfile << toph[w];
      for(int w = 0; w < namelength; w++)
        sfile << name[w];

      for(int b = 0; b < (int)bff2s.size(); b++)
      {
        for(int byte = 0; byte < 24; byte++)
          sfile << bff2s[b].header[byte];

        u8* h = intToBytes(bff2s[b].height);
        for(int w = 0; w < 4; w++)
          sfile << h[w];
        h = intToBytes(bff2s[b].width*bff2s[b].colorSize);
        for(int w = 0; w < 4; w++)
          sfile << h[w];
        h = intToBytes(bff2s[b].namelength);
        for(int w = 0; w < 4; w++)
          sfile << h[w];
        for(int w = 0; w < bff2s[b].namelength; w++)
          sfile << bff2s[b].name[w];

        if(bff2s[b].colornum > 0)
        {
          h = intToBytes(bff2s[b].colornum);
          for(int w = 0; w < 4; w++)
            sfile << h[w];

          for(int i = 0; i < bff2s[b].colornum; i++)
          {
            sfile << (char)bff2s[b].colors[i].red;
            sfile << (char)bff2s[b].colors[i].green;
            sfile << (char)bff2s[b].colors[i].blue;
            sfile << (char)bff2s[b].colors[i].alpha;
          }
        }

        for(int byte = 0; byte < bff2s[b].imgsize; byte++)
          sfile << bff2s[b].imgbuf[byte];
      } 
    }
  }

  
  //exportImage(outfilename, imgtmpbuf, isize, width, height, colorSize, compressed, bff2type, colors);

}




