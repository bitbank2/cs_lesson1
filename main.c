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

#define ARRAY_SIZE 128

int main(int argc, char* argv[])
{
int i, j;
uint32_t *pInt32s;
uint8_t *pInt8s

// Allocate memory for 2 arrays of integers
   pInt32s = malloc(ARRAY_SIZE * sizeof(uint32_t);
   pInt8s = malloc(ARRAY_SIZE * sizeof(uint8_t);

   for (i=0; i<ARRAY_SIZE; i++)
   {
      pInt32s[i] = i; // fill the array such that each element has the value of its index
   }

   // convert the values from uint32_t's into uint8_t's with direct pointer access
   j = 0;
   for (i=0; i<ARRAY_SIZE; i++)
   {
      *pInt8s = *pIn32s;
      pInt32s += 4;
      pIn8s += 1;
   }
   free(pInt32s);
   free(pInt8s);

   for (i=0; i<ARRAY_SIZE; i++)
   {
      printf("pInt8s[%d] = %d\n", pInts8[i]);
   }

   return 0;
} /* main() */
