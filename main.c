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
#define HEADER_XSIZE 18
#define HEADER_YSIZE 22
#define HEADER_BPP 28

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : MilliTime()                                                *
 *               returns the relative time in milliseconds                  *
 *                                                                          *
 ****************************************************************************/
int MilliTime()
{
int iTime;
struct timespec res;

    clock_gettime(CLOCK_MONOTONIC, &res);
    iTime = 1000*res.tv_sec + res.tv_nsec/1000000;

    return iTime;
} /* MilliTime() */

// Draw a circle
// Simple (and slow) method for drawing a circle with 1-bit per pixel
// An exercise for the reader is to enhance this code for other bit depths
void DrawCircle(unsigned char *pBits, int iPitch, int iRadius, int iXCenter, int iYCenter, unsigned char ucColor)
{
float angle;
int x, y;
unsigned char *pDest;

   for (angle=0.0; angle<6.28; angle += 0.01)
   {
	x = (int)(cos(angle) * iRadius) + iXCenter;
	y = (int)(sin(angle) * iRadius) + iYCenter;
        pDest = &pBits[(y * iPitch) + (x >> 3)];
	*pDest &= ~(0x80 >> (x & 7)); // set pixel to 0
        if (ucColor) // if non-zero color, set pixel to 1
        {
           *pDest |= (0x80 >> (x & 7));           
	}
   }
} /* DrawCircle() */

// Invert a pixel in a 1-bpp bitmap
void invert_pixel(unsigned char *pBitmap, int iPitch, int x, int y)
{
unsigned char *pDest;

	pDest = &pBitmap[(y * iPitch) + (x >> 3)];
	*pDest ^= (0x80 >> (x & 7)); // XOR the selected pixel with 1
} /* invert_pixel() */

int main(int argc, char* argv[])
{
FILE *fhandle;
int i, x, y;
int iTime;
unsigned char *pBitmap;

// Create a 256 x 256 bitmap (1 bit per pixel)
   pBitmap = malloc(256 * 32); // 256 lines x 32 bytes per line
   memset(pBitmap, 0xf0, 256*32); // fill it with white and black stripes
// Draw a black circle in the center
   DrawCircle(pBitmap, 256>>3, 50, 128, 128, 0);

//
// Measure the time it takes to invert the bitmap 999 times
// Do it an odd number of times so that it comes out inverted
//
   iTime = MilliTime();
   for (i=0; i<999; i++)
   {
      for(x=0; x<256; x++) // go through every pixel and invert it
      {
         for (y=0; y<256; y++)
         {
            invert_pixel(pBitmap, 32, x, y);
         }
      }
   }
   iTime = MilliTime() - iTime;
   printf("Time to invert the bitmap 999 times = %d milliseconds\n", iTime);

// Prepare the Windows BMP file header
   *(int *)&bmp_header[HEADER_FILESIZE] = sizeof(bmp_header) + 256*32;
   *(int *)&bmp_header[HEADER_XSIZE] = 256;
   *(int *)&bmp_header[HEADER_YSIZE] = -256; // Windows BMP have Y flipped
   *(short *)&bmp_header[HEADER_BPP] = 1; // 1 bit per pixel

// Create the output file
   fhandle = fopen("test.bmp","w+");
   if (fhandle != NULL)
   {
      fwrite(bmp_header, 1, sizeof(bmp_header), fhandle); // Write the BMP file header
      fwrite(pBitmap, 1, 256*32, fhandle); // write the pixels
      fclose(fhandle);
   }
   free(pBitmap); // free the memory for the bitmap
   return 0;
} /* main() */
