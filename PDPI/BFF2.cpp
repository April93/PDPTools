#include "BFF2.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <cmath>


using namespace std;

BFF2::BFF2()
{
}

//Generate Command Byte
int BFF2::encodeCommandByte(int bytesAffected, int extraTimesDisplayed)
{
  //1 pixel displayed 0 extra times is the minimum
  //int bytesAffected = 1;
  //int extraTimesDisplayed = 0;
  int commandByte = 0x00;

  if(extraTimesDisplayed < 1)
  {
    //cout << "Display multiple pixels once." << endl;
    commandByte = bytesAffected-1;
  }
  else
  {
    //cout << "Loop" << endl;
    if(bytesAffected <= 1)
    {
      //cout << "Loop one pixel multiple times." << endl;
      if(extraTimesDisplayed > 49)
        cout << "Problem! Cannot display a byte more than 49 times." << endl;
      commandByte = (extraTimesDisplayed-1)+0x80;
      if(commandByte > 0xB0)
        cout << "Problem! CommandByte is larger than 0xB0!" << endl;
    }else
    {
      //cout << "Loop multiple pixels multiple tiems." << endl;
      if(extraTimesDisplayed > 8)
        cout << "Problem! Multiple bytes cannot be displayed more than 8 extra times!" << endl;

      int secondNibble = (extraTimesDisplayed-1)+(8*(bytesAffected%2==0?0:1));

      int firstNibble = (0xB+(bytesAffected/2))<<4;

      commandByte = firstNibble+secondNibble;
    }
  }
  return commandByte;
}

u8* BFF2::compress(u8* imageb, int size, int& compSize)
{
	cout << "Starting compression. Size of Raw image: " << size << endl;
	int obcompVal = 0;
	int* bcompCnt = new int[9];
	u8* compImageB = new u8[size];
	int writeIndex = 0;
	for(int i = 0; i < size; i++)
	{
		//Check 1 byte looped multiple times.
		//Done first because best compression.
		//cout << "New CMD." << endl;
		obcompVal = imageb[i];
		bcompCnt[0] = 0;
		for(int j = 1; j < 50; j++)
		{
			if(obcompVal != imageb[i+j])
				break;
			bcompCnt[0]++;
		}
		//if(bcompCnt[0] > 8) //If the match count is high, just encode now.
		//{
			//int cmdbyte = encodeCommandByte(1, bcompCnt[0]);
			//cout << cmdbyte << ":" << obcompVal << ":" << bcompCnt[0] << endl;
			//i+=bcompCnt[0];
		//}else //Otherwise it's time to start searching for a good compression.
		//{
			//We want to check every amount: 1-8 extra
			for(int j = 1; j < 9; j++)
			{
				if(i+j > size)
					break;
				bcompCnt[j] = 0;
				//Fill up a buffer to compare with.
				int* mbcompVals = new int[j+1];
				for(int n = 0; n <= j; n++)
					mbcompVals[n] = imageb[i+n];

				//Now loop up to the max amount of possible 
				//loops (8) to see how many matches
				for(int l = 1; l < 9; l++)
				{
					bool match = true;
					for(int n = 0; n <= j; n++)
					{
						if(mbcompVals[n] != imageb[i+(l*(j+1))+n])
							match = false;
					}
					if(match == false)
						break;
					bcompCnt[j]++;
				}
			}
		//}

		//Check the highest compression
		int highest = 1;
		int ind = 0;
		for(int j = 0; j < 9; j++)
		{
			//cout << "h:"<<highest<<" i:"<<j<<" a:"<<bcompCnt[j]<<" v:"<<(j+1)*bcompCnt[j]<< endl;
			if(((j+1)*bcompCnt[j]) >= highest)
			{
				highest = ((j+1)*bcompCnt[j]);
				ind = j;
			}
		}
		int cmdbyte = encodeCommandByte(ind+1, bcompCnt[ind]);
		//if(ind != 0)
		//cout << i << "." << ind << "=" << hex << cmdbyte << ":" << dec << bcompCnt[ind] << ":";
		if(i+ind>=size)
			break;
		//cout << setfill('0') << setw(2) << hex << cmdbyte << " ";
		compImageB[writeIndex++] = cmdbyte;

		for(int n = 0; n <= ind; n++)
		{
			//cout << setfill('0') << setw(2) << hex << (int)imageb[i+n] << " ";
			compImageB[writeIndex++] = (int)imageb[i+n];
		}
		//cout << " - " << ((ind+1)*bcompCnt[ind]);
		//cout << endl;
		//if(ind == 0 && bcompCnt[ind] == 0)
			//bcompCnt[ind] = 1;
		i+=((ind+1)*(bcompCnt[ind]+1))-1;
	}
	compSize = writeIndex;
	return compImageB;
}

u8* BFF2::decompress()
{
  int imgindex = 0;
  int compcmd = 0;
  int bytecnt = 0;
  int bytemax = -1;
  int loop = -1;
  int tmpcmd = 0;

  int newbsize = width * height * colorSize;
  u8* newb = new u8[newbsize];
  int j = 0;
  while(imgindex < imgsize)
  {
      //If reached end of command, get a new one.
      if(bytecnt >= bytemax)
      {
        if(loop <= 0)
        {
          loop = 0;
          tmpcmd = (int)imgbuf[imgindex];
          bytecnt = 0;
          bytemax = (tmpcmd&0x0F);
          compcmd = (tmpcmd&0xF0)>>4;
          //If first nibble is above 8, loop.
          if(compcmd >= 8)
          {
            //If first nibble is between 8 and 0xB, loop a single pixel (tmpcmd-0x80)+1 times. 
            if(compcmd <= 0xB)
            {
              //Set the amount of times to loop the pixel
              loop = (tmpcmd-0x80)+1;
              //How many pixels looped? 1, obviously.
              bytemax = 1;
            }
            //If the first nibble is above 0xB, loop multiple pixels a certain amount of times.
            else
            {
              //Set the amount of times to loop the set of pixels
              //The amount of times to loop can be between 1 and 8 (represented by values 0-7).
              //This is due to when the value is 9-F, the amount of pixels is increased by 1 (and then loops between 1-8 times)
              loop = (bytemax%8)+1;
              //The amount of pixels to loop. C=2, D=4, E=5, F=6. +1 if cntmax (2nd nibble) is 8 or above.
              bytemax = ((compcmd-0xB)*2)+(bytemax>=8?1:0);
            }
          }else
          {
            //No loop, just draw pixels as normal.
            //Set the amount of pixels to draw normally
            bytemax=tmpcmd+1;
            loop = 0;
          }
          //int cbyte = encodeCommandByte(bytemax, loop);
          //cout << tmpcmd << ", " << cbyte << endl;
          imgindex+=1;
        }else
        {
          loop--;
          bytecnt = 0;
          imgindex-= bytemax;
        }
      }
      if (imgindex > imgsize - 1) {
        break;
      }
      if (j > newbsize - 1) {
        break;
      }
      int pixelbyte = imgbuf[(imgindex++)];
      newb[(j++)] = pixelbyte;
      bytecnt++;
  }
  return newb;
}

void BFF2::exportImage(char* outfilename)
{

  const char* filename = outfilename;
  u8* imageb;
  //int isize;
  if(compressed)
  {
    imageb = decompress();
    //isize = width*height*colorSize;
  }
  else
    imageb = imgbuf;

  int imgin = 0;
  
  std::vector<unsigned char> image;
  image.resize(width * height * 4);
  for(int y = 0; y < height; y++)
  for(int x = 0; x < width; x++)
  {
    //Method 1 - Image extract
    if(bff2type == 0x55) //RGBA8888
    {
      image[4 * width * y + 4 * x + 0] = (int)imageb[imgin+0]; //r
      image[4 * width * y + 4 * x + 1] = (int)imageb[imgin+1]; //g
      image[4 * width * y + 4 * x + 2] = (int)imageb[imgin+2]; //b
      image[4 * width * y + 4 * x + 3] = (int)imageb[imgin+3]; //a
      imgin+=4;
    }
    if(bff2type == 0x54) //RGBA5551
    {
      int val1 = (int)imageb[imgin];
      int val2 = (int)imageb[imgin+1];
      float r = val1>>3;
      float g = ((val1&7)<<2)+((val2&0xC0)>>6);
      float b = (val2&0x3E)>>1;

      r = ((float) r/31)*255;
      g = ((float) g/31)*255;
      b = ((float) b/31)*255;
      image[4 * width * y + 4 * x + 0] = r; //r
      image[4 * width * y + 4 * x + 1] = g; //g
      image[4 * width * y + 4 * x + 2] = b; //b
      image[4 * width * y + 4 * x + 3] = (val2&1)*255; //a
      imgin+=2;
    }
    if(bff2type == 0x33) //8bpp
    {
      image[4 * width * y + 4 * x + 0] = colors[(int)imageb[imgin]].red; //r
      image[4 * width * y + 4 * x + 1] = colors[(int)imageb[imgin]].green; //g
      image[4 * width * y + 4 * x + 2] = colors[(int)imageb[imgin]].blue; //b
      image[4 * width * y + 4 * x + 3] = colors[(int)imageb[imgin]].alpha; //a
      imgin++;
    }
    if(bff2type == 0x32) //L8/Shadow
    {
      int r = (int)imageb[imgin];
      //cout << r << ", " << a << endl;
      image[4 * width * y + 4 * x + 0] = r; //r
      image[4 * width * y + 4 * x + 1] = r; //g
      image[4 * width * y + 4 * x + 2] = r; //b
      image[4 * width * y + 4 * x + 3] = 0xFF; //a
      imgin++;
    }
    if(bff2type == 0x24) //LA8
    {
      int r = (int)imageb[imgin];
      int a = (int)imageb[imgin+1] & 0xFF;
      //cout << r << ", " << a << endl;
      image[4 * width * y + 4 * x + 0] = r; //r
      image[4 * width * y + 4 * x + 1] = r; //g
      image[4 * width * y + 4 * x + 2] = r; //b
      image[4 * width * y + 4 * x + 3] = a; //a
      imgin+=2;
    }
    if(bff2type == 0x23) //Font/1bpp/LA4
    {
      int pixel = (int)imageb[imgin];
      int alpha = (pixel&0x0F);
      int whiteblack = (pixel&0xF0)>>4;
      //Generate Pixel Value from WhiteBlack and Alpha
      //int nPixel = ((whiteblack<<4)&0xF0)+(alpha&0x0F);
      if(imgin >= width * height)
      {
        whiteblack = 0;
        alpha = 0;
      }
      image[4 * width * y + 4 * x + 0] = whiteblack*17; //r
      image[4 * width * y + 4 * x + 1] = whiteblack*17; //g
      image[4 * width * y + 4 * x + 2] = whiteblack*17; //b
      image[4 * width * y + 4 * x + 3] = alpha*17; //a
      imgin++;
    }
  }
  //compress(imageb, width * height * 4);
  lodepng::encode(filename, image, width, height);
  //encodeOneStep(filename, image, width, height);
}

void BFF2::importImage(char* filename)
{
	std::vector<unsigned char> image;
	unsigned width = 0;
	unsigned height = 0;
	string fname = filename;
	lodepng::decode(image, width, height, fname);

	u8* frmtImage = new u8[width*height*colorSize];
	int imgin = 0;
	color* temppal = new color[colornum];
	int tmppalnum = 0;
	for (int i = 0; i < colornum; i++) {
      temppal[i].red = 0;
      temppal[i].green = 0;
      temppal[i].blue = 0;
      temppal[i].alpha = 0;
    }
	for(unsigned i = 0; i < width*height*4; i+=4)
	{
		int r = image[i];
		int g = image[i+1];
		int b = image[i+2];
		int a = image[i+3];

		//cout << hex << setw(2) << setfill('0') << r << g << b << a << " ";
		if(bff2type == 0x55) //RGBA8888
	    {
			frmtImage[imgin+0] = r; //r
			frmtImage[imgin+1] = g; //g
			frmtImage[imgin+2] = b; //b
			frmtImage[imgin+3] = a; //a
			imgin+=4;
	    }
	    if(bff2type == 0x54) //RGBA5551
	    {
			int val1 = 0;
			int val2 = 0;

			r = ((float) r/255)*31;
			g = ((float) g/255)*31;
			b = ((float) b/255)*31;
			a = a>=1?1:0;

			val1 = (r<<3)+(g>>2);
			val2 = ((g<<6)&0xC0)+(b<<1)+a;

			frmtImage[imgin] = val1;
			frmtImage[imgin+1] = val2;
			imgin+=2;
	    }
	    if(bff2type == 0x33) //8bpp
	    {
	    	bool found=false;
	    	int closestpal = 0;
	    	int distance = 255*4;
	    	for(int c = 0; c < tmppalnum; c++)
	    	{
	    		if(temppal[c].alpha == a && temppal[c].red == r && temppal[c].green == g && temppal[c].blue == b)
	    		{
	    			frmtImage[imgin] = c;
	    			found=true;
	    			break;
	    		}else
	    		{
	    			int da = abs(temppal[c].alpha-a);
	    			int dr = abs(temppal[c].red-r);
	    			int dg = abs(temppal[c].green-g);
	    			int db = abs(temppal[c].blue-b);
	    			int compdist = da+dr+dg+db;
	    			if(compdist < distance)
	    			{
	    				closestpal = c;
	    				distance = compdist;
	    			}
	    		}
	    	}
	    	if(!found)
	    	{
	    		if(tmppalnum < colornum)
	    		{
	    			temppal[tmppalnum].red = r;
	    			temppal[tmppalnum].green = g;
	    			temppal[tmppalnum].blue = b;
	    			temppal[tmppalnum].alpha = a;
	    			frmtImage[imgin] = tmppalnum;
	    			tmppalnum++;
	    		}else
	    		{
	    			frmtImage[imgin] = closestpal;
	    		}
	    	}
			imgin++;
	    }
	    if(bff2type == 0x32) //L8/Shadow
	    {
	    	int val = (r+g+b)/3;
	    	frmtImage[imgin] = val;
	    	imgin++;
	    }
	    if(bff2type == 0x24) //LA8
	    {
	    	frmtImage[imgin] = (r+g+b)/3;
	    	frmtImage[imgin+1] = a;
	    	imgin+=2;
	    }
	    if(bff2type == 0x23) //Font/1bpp/LA4
	    {
	      int whiteblack = ((r+g+b)/3)/17;
	      int alpha = a/17;
	      frmtImage[imgin] = ((whiteblack<<4)&0xF0)+(alpha&0x0F);;
	      imgin++;
	    }
	}

	cout << tmppalnum << endl;

	int isize = width*height*colorSize;
	if(compressed)
	{	
		frmtImage = compress(frmtImage, width*height*colorSize, isize);
		/*for(int i = 0; i < isize; i++)
		{
			cout << hex << setw(2) << setfill('0') << (int)frmtImage[i] << " ";
		}*/
	}
	colors = temppal;
	colornum = tmppalnum;
	imgbuf = frmtImage;
	imgsize = isize;
	this->width = width;
	this->height = height;
}

void BFF2::printType()
{
	switch (bff2type)
	{
		case 0x55: cout << "RGBA8888" << endl; break;
		case 0x54: cout << "RGBA5551" << endl; break;
		case 0x33: cout << "8bpp" << endl; break;
		case 0x32: cout << "L8/Shadow" << endl; break;
		case 0x24: cout << "LA8" << endl; break;
		case 0x23: cout << "LA4" << endl; break;
	}
}
