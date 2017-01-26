//
// Computer science lesson 1
// working with files and bitmaps
// This program will create a bitmap image in memory
// then write it to a Windows BMP file
// The student will take this further
// 
// Written by Larry Bank 11/17/2016
//
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* Windows BMP header info */
unsigned char bmp_header[62] =
        {0x42,0x4d, /* magic values = 'BM' */
         0,0,0,0,         /* File size */
         0,0,0,0,
         0x3e,0,0,0,0x28,0,0,0, /* offset to pixels */
         0,0,0,0, /* Xsize */
         0,0,0,0, /* Ysize - needs to be negative or bitmap will be upside down */
         1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       /* number of planes, bits per pel */
         0,0,0,0,0,0,0,0,0,0,0xff,0xff,0xff};     /* color palette */

#define HEADER_FILESIZE 2
#define HEADER_OFFBITS 10
#define HEADER_XSIZE 18
#define HEADER_YSIZE 22
#define HEADER_PLANES 26
#define HEADER_BPP 28

// workaround to suppress pointer aliasing warnings
int *pHeaderOffBits = (int *)&bmp_header[HEADER_OFFBITS];
int *pHeaderFileSize = (int *)&bmp_header[HEADER_FILESIZE];
int *pHeaderXSize = (int *)&bmp_header[HEADER_XSIZE];
int *pHeaderYSize = (int *)&bmp_header[HEADER_YSIZE];
short *pHeaderBPP= (short *)&bmp_header[HEADER_BPP];

int ReadBitmap(char *name, unsigned char **pData, int *iWidth, int *iHeight, int *iBpp, int *iOff)
{
FILE *fp;
int iFileSize;
unsigned char *pBitmap;
int iPitch, y;

	fp = fopen(name, "rb"); // open the input file for reading
	if (fp == NULL)
	{
		printf("Error: unable to open the input file %s\n", name);
		return -1;
	}
	fseek(fp, 0L, SEEK_END); // get the file size
	iFileSize = (int)ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	pBitmap = malloc(iFileSize);
	if (pBitmap == NULL)
	{
		printf("Error allocating %d bytes; aborting...\n", iFileSize);
		fclose(fp);
		return -1;
	}
	fread(pBitmap, 1, iFileSize, fp);
	fclose(fp);
	if (pBitmap[0] != 'B' || pBitmap[1] != 'M')
	{
		printf("Error - not a BMP file; aborting...\n");
		free(pBitmap);
		return -1;
	}
	*iOff = pBitmap[HEADER_OFFBITS] + (pBitmap[HEADER_OFFBITS+1] << 8); // offset to start of bits
	*iWidth = *(int32_t *)&pBitmap[HEADER_XSIZE];
	*iHeight = *(int32_t *)&pBitmap[HEADER_YSIZE];
	*iBpp = (pBitmap[HEADER_PLANES] * pBitmap[HEADER_BPP]);
	iPitch = (*iWidth * *iBpp) / 8;
    iPitch = ((iPitch + 3) & 0xfffc); // DWORD aligned
    printf("width=%d, height=%d, off=%d, pitch=%d\n", *iWidth, *iHeight, *iOff, iPitch);
	if (*iHeight < 0) // normal memory direction
	{
		*iHeight = 0 - *iHeight;
	}
	else // need to flip the vertical direction
	{
		unsigned char ucTemp[iPitch];
		unsigned char *s, *d;
		for (y=0; y<*iHeight/2; y++) // swap scanlines
		{
			s = &pBitmap[*iOff + (y * iPitch)];
			d = &pBitmap[*iOff + ((*iHeight - 1 - y) * iPitch)];
			// swap the top/bottom lines to flip the image vertically
			memcpy(ucTemp, s, iPitch);
			memcpy(s, d, iPitch);
			memcpy(d, ucTemp, iPitch);
		}
	}
    *pData = pBitmap;
    return 0;
} /* ReadBitmap() */

void WriteBitmap(char *name, unsigned char *pData, int iWidth, int iHeight, int iBpp)
{
FILE *fhandle;

// Prepare the Windows BMP file header
   *pHeaderXSize = iWidth;
   *pHeaderYSize = -iHeight; // Windows BMP have Y flipped
   *pHeaderBPP = iBpp; // bits per pixel

// Create the output file
   fhandle = fopen(name,"w+");
   if (fhandle != NULL)
   {
          if (iBpp == 8) // create a grayscale palette
          {
                  int i;
                  unsigned char pal[1024];
                  for (i=0; i<256; i++)
                  {
                          pal[i*4+0] = i;
                          pal[(i*4)+1] = i;
                          pal[(i*4)+2] = i;
				  }
          		 *pHeaderFileSize = 54 + 1024 + (iWidth * iHeight);
                *pHeaderOffBits = 54 + 1024;
        fwrite(bmp_header, 1, 54, fhandle); // Write the BMP file header
        fwrite(pal, 1, 1024, fhandle); // Write the BMP file header
          }
          else
          {
        *pHeaderFileSize = sizeof(bmp_header) + (iWidth * iBpp * iHeight) / 8;
        fwrite(bmp_header, 1, sizeof(bmp_header), fhandle); // Write the BMP file header
          }
      fwrite(pData, 1, (iWidth*iHeight*iBpp)/8, fhandle); // write the pixels
      fclose(fhandle);
   }
} /* WriteBitmap() */

//
// This is the assignment - write a function which takes input
// of a color bitmap (24-bits per pixel - R/G/B)
// and converts each pixel to grayscale (8-bits per pixel)
// The formula for converting color pixels to black and white (gray)
// is gray = (R + 2*G + B)/4;
// The green color has more "weight" because our eyes are more
// sensitive to green.
// The 24-bit pixels are in the form BGR
// This means that for a single pixel, if *p is a character
// pointer pointing to the pixel, then p[0]=B, p[1]=G, p[2]=R

void MakeGray(unsigned char *pColor, int iWidth, int iHeight, unsigned char *pDest)
{
} /* MakeGray() */

int main(int argc, char* argv[])
{
unsigned char *pColor, *pGray, *pTemp;
int rc, iWidth, iHeight, iBpp, iOff24, iOff8;
int x, y;
unsigned char *s, *d;

   rc = ReadBitmap("PARROTS24.BMP", &pColor, &iWidth, &iHeight, &iBpp, &iOff24);
   if (rc != 0)
      return -1;
   rc = ReadBitmap("PARROTS8.BMP", &pGray, &iWidth, &iHeight, &iBpp, &iOff8);
   if (rc != 0)
      return -1;

   pTemp = malloc(iWidth * iHeight); // allocate a bitmap for our gray converter

   // Write this function to generate gray pixels from color pixels
   MakeGray(&pColor[iOff24], iWidth, iHeight, pTemp);

   // Save your temporary bitmap so that you can visually see what the problem is
   // if your grayscale output doesn't match the standard version
   WriteBitmap("my_bitmap.bmp", pTemp, iWidth, iHeight, 8);

   // Check if the output matches the expected output
   for (y=0; y<iHeight; y++)
   {
      s = &pGray[iOff8 + (y * iWidth)];
      d = &pTemp[y * iWidth];
      for (x=0; x<iWidth; x++)
      {
         if (*s != *d)
         {
             printf("Bitmaps are different - fail\n");
             return -1;
         }
      }
   }
   printf("Bitmaps match - pass!\n");
  
   return 0;
} /* main() */
