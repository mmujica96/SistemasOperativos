#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"
#include "filter.h"

int main(int argc, char **argv) {
  FILE* source;
  FILE* dest;
  BMP_Image* imageIn = NULL;
  BMP_Image* imageOut = NULL;
  int numThreads=4;
  int boxFilter[3][3] = {
    {1,1,1},
    {1,1,1},
    {1,1,1}
  };
  if (argc != 3) {
    printError(ARGUMENT_ERROR);
    exit(EXIT_FAILURE);
  }
  if((source = fopen(argv[1], "rb")) == NULL) {
    printError(FILE_ERROR);
    exit(EXIT_FAILURE);
  }
  if((dest = fopen(argv[2], "wb")) == NULL) {
    printError(FILE_ERROR);
    exit(EXIT_FAILURE);
  } 
  
  imageIn = malloc(sizeof(BMP_Image));
  imageOut = malloc(sizeof(BMP_Image));
  readImage(source, imageIn);

  if(!checkBMPValid(&imageIn->header)) {
    printError(VALID_ERROR);
    exit(EXIT_FAILURE);
  }
  transfImage(imageIn,imageOut);
  if(!checkBMPValid(&imageIn->header)) {
    printError(VALID_ERROR);
    exit(EXIT_FAILURE);
  }
  
  applyParallel(imageIn, imageOut, boxFilter, numThreads);
  
  writeImage(dest,imageOut);
  
  printBMPHeader(&imageIn->header);
  printBMPImage(imageIn);

  //freeImage(imageIn);
  //freeImage(imageOut);
  fclose(source);
  fclose(dest);

  exit(EXIT_SUCCESS);
}
