#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "filter.h"


void apply(BMP_Image * imageIn, BMP_Image * imageOut)
{
  int boxFilter[3][3]={
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1},
  };
  int filtroCaja = 9;//el factor de normalizacion para el filtro de la caja 
  for(int y = 0; y < imageIn->norm_height; y++) {
        for (int x = 0; x < imageIn->header.width_px; x++) {
            int red = 0, green = 0, blue = 0;

            for (int fy = -1; fy <= 1; fy++) {
                for (int fx = -1; fx <= 1; fx++) {
                    int iy = y + fy;
                    int ix = x + fx;

                    if (iy >= 0 && iy < imageIn->norm_height && ix >= 0 && ix < imageIn->header.width_px) {
                        red += imageIn->pixels[iy][ix].red * boxFilter[fy + 1][fx + 1];
                        green += imageIn->pixels[iy][ix].green * boxFilter[fy + 1][fx + 1];
                        blue += imageIn->pixels[iy][ix].blue * boxFilter[fy + 1][fx + 1];
                    }
                }
            }

            imageOut->pixels[y][x].red = red / filtroCaja;
            imageOut->pixels[y][x].green = green / filtroCaja;
            imageOut->pixels[y][x].blue = blue / filtroCaja;
          }
      }
}

void applyParallel(BMP_Image * imageIn, BMP_Image * imageOut, int boxFilter[3][3], int numThreads)
{
  //pthread_t* threads = malloc(numThreads * sizeof(pthread_t));
  //ThreadData* threadData = malloc(numThreads * sizeof(ThreadData));
  
  pthread_t threads[numThreads];
  ThreadData threadData[numThreads];
  
  int height = imageIn->norm_height;
  int rowsPerThread = height / numThreads;
  int remainingRows = imageIn->norm_height % numThreads;
  
  for(int i =0; i<numThreads; i++){
    threadData[i].imageIn = imageIn;
    threadData[i].imageOut = imageOut;
    //threadData[i].boxFilter = boxFilter;
    memcpy(threadData[i].boxFilter, boxFilter, sizeof(threadData[i]).boxFilter);
    threadData[i].startRow = i*rowsPerThread;
    threadData[i].endRow = (i== numThreads -1) ? threadData[i].endRow += remainingRows : (i+1) * rowsPerThread;
    
    //for(int j=0; j <3; ++j){
      //for(int k=0; k<3; ++k){
        //threadData[i].boxFilter[j][k] = boxFilter[j][k];
      //}
    //}
    
    pthread_create(&threads[i], NULL, filterThreadWorker, (void*)&threadData[i]);
  }
  for(int i=0; i< numThreads; i++){
    pthread_join(threads[i], NULL);
  }
  
  //free(threads);
  //free(threadData);
}

void *filterThreadWorker(void * args)
{
  ThreadData *data = (ThreadData *)args;
  BMP_Image* imageIn = data->imageIn;
  BMP_Image* imageOut = data->imageOut;
  int startRow = data->startRow;
  int endRow = data->endRow;
  int (*boxFilter)[3] = data->boxFilter;
  
  int filtroCaja = 9;//el factor de normalizacion para el filtro de la caja 
  int width = imageIn->header.width_px;
  int height = imageIn->norm_height;
  
  for(int y= startRow; y < endRow; y++){
    for(int x=0;x < width; x++){
      int red =0, green=0, blue=0;
      
      for(int fy =-1; fy<=1;fy++){//aplicar el filtro de caja 3*3
        for(int fx =-1; fx<=1;fx++){
          int ny = y + fy;
          int nx = x + fx;
          
          //comprobar limites
          if(ny >=0 && ny < height && nx >=0 && nx < width){
            red +=imageIn->pixels[ny][nx].red * boxFilter[fy +1][fx +1];
            green +=imageIn->pixels[ny][nx].green * boxFilter[fy+1][fx+1];
            blue +=imageIn->pixels[ny][nx].blue * boxFilter[fy+1][fx+1];
          }
        }
      }
      
      //calcular el valor promedio y asignarlo al pixel de destino
      //Pixel* destPixel= &imageOut->pixels[y][x];
      imageOut->pixels[y][x].red =red/filtroCaja;
      imageOut->pixels[y][x].green =green/filtroCaja;
      imageOut->pixels[y][x].blue =blue/filtroCaja;
      imageOut->pixels[y][x].alpha = 255;//alpha maximo, es opaco
    }
  }
  pthread_exit(NULL);
  
}
