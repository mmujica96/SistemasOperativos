#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "bmp.h"
#include "filter.h"

#define NUM_THREADS 4// numero definido para procesamiento en paralelo

int numThreads = 4;

int main(int argc, char **argv) {
  FILE* source;
  FILE* dest;
  BMP_Image* image = NULL;
  BMP_Image* blurredImage = NULL;
  int boxFilter[3][3]={
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1},
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
    fclose(source);
    exit(EXIT_FAILURE);
  } 

  readImage(source, image);

  if(!checkBMPValid(&image->header)) {
    printError(VALID_ERROR);
    freeImage(image);
    fclose(source);
    fclose(dest);
    exit(EXIT_FAILURE);
  }

  readImage(source, image);
  printBMPHeader(&image->header);
  printBMPImage(image);
  
  blurredImage = (BMP_Image*)malloc(sizeof(BMP_Image));//imagen desenfocada
  if(!blurredImage){
    printError(MEMORY_ERROR);
    freeImage(image);
    fclose(source);
    fclose(dest);
    exit(EXIT_FAILURE);
  }
  
  blurredImage->header = image->header;
  blurredImage->norm_height = image->norm_height;
  
  blurredImage->pixels = (Pixel**)malloc(image->norm_height * sizeof(Pixel*));
    if (!blurredImage->pixels) {
        printError(MEMORY_ERROR);
        freeImage(image);
        free(blurredImage);
        fclose(source);
        fclose(dest);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < image->norm_height; i++) {
        blurredImage->pixels[i] = (Pixel*)malloc(image->header.width_px * sizeof(Pixel));
        if (!blurredImage->pixels[i]) {
            printError(MEMORY_ERROR);
            for (int j = 0; j < i; j++) {
                free(blurredImage->pixels[j]);
            }
            free(blurredImage->pixels);
            freeImage(image);
            free(blurredImage);
            fclose(source);
            fclose(dest);
            exit(EXIT_FAILURE);
        }
    }

    // Aplicar el filtro de caja en paralelo
    applyParallel(image, blurredImage, boxFilter, numThreads);

    // Escribir la imagen desenfocada en el archivo de destino
    writeImage(argv[2], blurredImage);

    // Liberar memoria
    freeImage(image);
    freeImage(blurredImage);
    fclose(source);
    fclose(dest);

    return EXIT_SUCCESS;
}
