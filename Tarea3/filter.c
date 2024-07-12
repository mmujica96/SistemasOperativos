#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "filter.h"
#include "bmp.h"

void apply (BMP_Image * imageIn, BMP_Image * imageOut)
{
printf("dentro de aplly\n");
  int boxFilter[3][3]={//filtro de desenfoque
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1},
  };
  int filtroCaja = 9;//el factor de normalizacion para el filtro de la caja 
  for(int y = 0; y < imageIn->norm_height; y++) {
        for (int x = 0; x < imageIn->header.width_px; x++) {
            int red = 0, green = 0, blue = 0;

            for (int offsetX = -1; offsetX <=1; offsetX++) {
                for (int offsetY = -1; offsetY <=1; offsetY++) {
                    int adjusteX = y + offsetX;
                    int adjusteY = x + offsetY;

                    if (adjusteX >= 0 && adjusteX < imageIn->norm_height && adjusteY >= 0 && adjusteY < imageIn->header.width_px) {
                        red += imageIn->pixels[adjusteX][adjusteY].red * boxFilter[offsetX+1][offsetY+1];
                        green += imageIn->pixels[adjusteX][adjusteY].green * boxFilter[offsetX+1][offsetY+1];
                        blue += imageIn->pixels[adjusteX][adjusteY].blue * boxFilter[offsetX+1][offsetY+1];
                    }
                }
            }

            imageOut->pixels[y][x].red = red / filtroCaja;
            imageOut->pixels[y][x].green = green / filtroCaja;
            imageOut->pixels[y][x].blue = blue / filtroCaja;
            imageOut->pixels[y][x].alpha = 255;
          }
      }
}

void applyParallel(BMP_Image * imageIn, BMP_Image * imageOut, int boxFilter[3][3], int numThreads)
{
printf("inicio parallel\n");
  pthread_t* threads = (pthread_t *)malloc(numThreads * sizeof(pthread_t));
  ThreadData* threadData = (ThreadData *) malloc(numThreads * sizeof(ThreadData));
  
  int height = imageIn->norm_height;
  int rowsPerThread = height / numThreads;
  int remainingRows = height % numThreads;
  int inicioFila=0;
  
  for(int i =0; i<numThreads; i++){
    threadData[i].imageIn = imageIn;
    threadData[i].imageOut = imageOut;
    threadData[i].startRow = i*rowsPerThread;
    int finalFila = inicioFila + rowsPerThread + (remainingRows--> 0 ? 1 : 0);
    threadData[i].endRow = finalFila;
    
    for(int j=0; j<3; j++){
      for(int k=0; k<3; k++){
         threadData[i].boxFilter[j][k] = boxFilter[j][k];
      }
    }
    printf("antes del created con filter\n");
    pthread_create(&threads[i], NULL, filterThreadWorker, &threadData[i]);
    
    inicioFila = finalFila;
  }
  for(int i=0; i< numThreads; i++){
    pthread_join(threads[i], NULL);
  }
  free(threadData);
  free(threads);
  printf("fin parallel\n");
}

void *filterThreadWorker(void * args)
{
printf("dentro de filterthread\n");
  ThreadData *data = (ThreadData *)args;
  BMP_Image* imageIn = data->imageIn;
  BMP_Image* imageOut = data->imageOut;
  //int startRow = data->startRow;
  //int endRow = data->endRow;
  //int (*boxFilter)[3] = data->boxFilter;
  
  apply(imageIn,imageOut);
  pthread_exit(NULL);
}
