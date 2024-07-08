#include "bmp.h"

typedef struct {
  BMP_Image* imageIn;
  BMP_Image* imageOut;
  int startRow;
  int endRow;
  int boxFilter[3][3];
}ThreadData;

void apply(BMP_Image * imageIn, BMP_Image * imageOut);

void applyParallel(BMP_Image * imageIn, BMP_Image * imageOut, int boxFilter[3][3], int numThreads);

void *filterThreadWorker(void * args);
