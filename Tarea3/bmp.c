#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bmp.h"

/* USE THIS FUNCTION TO PRINT ERROR MESSAGES
   DO NOT MODIFY THIS FUNCTION
*/
void printError(int error){
  switch(error){
  case ARGUMENT_ERROR:
    printf("Usage:ex5 <source> <destination>\n");
    break;
  case FILE_ERROR:
    printf("Unable to open file!\n");
    break;
  case MEMORY_ERROR:
    printf("Unable to allocate memory!\n");
    break;
  case VALID_ERROR:
    printf("BMP file not valid!\n");
    break;
  default:
    break;
  }
}

/* The input argument is the source file pointer. The function will first construct a BMP_Image image by allocating memory to it.
 * Then the function read the header from source image to the image's header.
 * Compute data size, width, height, and bytes_per_pixel of the image and stores them as image's attributes.
 * Finally, allocate menory for image's data according to the image size.
 * Return image;
*/
BMP_Image* createBMPImage(FILE* fptr) {
  if(fptr == NULL){
    printError(FILE_ERROR);
    return NULL;
  }
  BMP_Image* bmpimage = (BMP_Image*)malloc(sizeof(BMP_Image));//asignamos memoria al puntero de la BPM_Image
  
  if(bmpimage == NULL){//si falla la asignacion de memoria
    printError(MEMORY_ERROR);
    return NULL; 
  }
  //leer los 54 bytes que corresponden al encabezado BMP para almacenarlo en la estructura
  if(fread(&(bmpimage->header), sizeof(BMP_Header), 1, fptr) !=1)
  {
    free(bmpimage);
    return NULL;
  }
 
  //comprobando que el encabezado cumpla con los parametros establecidos
  if(!checkBMPValid(&(bmpimage->header))){
    printError(VALID_ERROR);
    free(bmpimage);//IMPORTANTE: Liberamos la memoria asignada en caso de no ser valido
    return NULL;
  }

  bmpimage->bytes_per_pixel = bmpimage->header.bits_per_pixel / 8;
  int anchoImg = bmpimage->header.width_px;
  int alturaIm = bmpimage->header.height_px;//podemos normalizar la altura
  bmpimage->norm_height = (alturaIm <0) ? -alturaIm : alturaIm;
  
  //asignar memoria a la matriz de pixeles
  bmpimage->pixels= (Pixel**)malloc(bmpimage->norm_height * sizeof(Pixel*));
  if(bmpimage->pixels == NULL){//verificando si falla la asignacion de memoria
    free(bmpimage);
    printError(MEMORY_ERROR);
    return NULL; 
  }
  
  for (int i=0; i < bmpimage->norm_height; i++){ //recorrer a la matriz 
    bmpimage->pixels[i] = (Pixel*)malloc(anchoImg * sizeof(Pixel));
    if(bmpimage->pixels[i]==NULL){
      while(--i >=0){
        free(bmpimage->pixels[i]);
      }
      free(bmpimage->pixels);
      free(bmpimage);
      return NULL ;
    }
  }
  //printBMPImage(bmpimage);
  //printBMPHeader(&bmpimage->header);
  return bmpimage;
  
}

/* The input arguments are the source file pointer, the image data pointer, and the size of image data.
 * The functions reads data from the source into the image data matriz of pixels.
*/
void readImageData(FILE* srcFile, BMP_Image* image, int dataSize) {
  //validaciones de archivo fuente y puntero BMP
  if(srcFile == NULL || image ==NULL){
    printError(FILE_ERROR);
    return;
  }
  
  //obtenemos los datos de ancho y altura
  int ancho = image->header.width_px;
  int altura = image->norm_height;
  
  for(int i =0; i < altura; i++){//recorre los datos de los pixeles de la imagen
    for(int j=0; j<ancho; j++){
      fread(&image->pixels[i][j], dataSize, 1, srcFile);
    }
  }
}

/* The input arguments are the pointer of the binary file, and the image data pointer.
 * The functions open the source file and call to CreateBMPImage to load de data image.
*/
void readImage(FILE *srcFile, BMP_Image * dataImage) {
  //comprobar que lo recibido sea valido
  if(srcFile == NULL){
    printError(ARGUMENT_ERROR);
    return;
  }
  //usando el metodo createBMP para cargar la data de la imagen del archivo
  BMP_Image *dataImageTemp= createBMPImage(srcFile);
  
  if(dataImageTemp == NULL){
    printError(MEMORY_ERROR);
    return;
  }
  
  //calculamos el tamanio de llos datos de la imagen
  int dataSize = dataImage->bytes_per_pixel;
  if(dataSize == 0){
    dataSize= dataImageTemp->header.width_px*dataImageTemp->norm_height*(dataImageTemp->header.bits_per_pixel /8);
  }
  
  memcpy(dataImage, dataImageTemp, sizeof(BMP_Image));
  //liberar memoria
  free(dataImageTemp);
  
  //leemos los datosss de la imagen
  readImageData(srcFile, dataImage, dataSize);
  
  
}

/* The input arguments are the destination file name, and BMP_Image pointer.
 * The function write the header and image data into the destination file.
*/
void writeImage(FILE *destFileName, BMP_Image* dataImage) {
  if(destFileName == NULL || dataImage == NULL){
    printError(ARGUMENT_ERROR);
    return;
  }

  fwrite(&(dataImage->header), sizeof(BMP_Header),1, destFileName);//escribir en el archivo de destino
  
  //pasar datos dde la imagen al archivo
  int ancho = dataImage->header.width_px;
  int altura = dataImage->norm_height;
  for(int i =0; i< altura; i++){
    if(fwrite(dataImage->pixels[i], sizeof(Pixel), ancho, destFileName) != ancho){
      printError(MEMORY_ERROR);
      return;
    }
  }
  
  //comprobando
  printf("La BMPImage fue escrita correctamente\n");
}

void transfImage(BMP_Image *imageIn, BMP_Image *imageOut)
{
  memcpy(&imageOut->header, &imageIn->header, sizeof(BMP_Header));

  if (imageIn->header.bits_per_pixel == 24)
  {
    imageOut->header.bits_per_pixel = 32;
    imageOut->header.imagesize = imageIn->norm_height * imageIn->header.width_px * 4;
    imageOut->header.size = imageOut->header.imagesize + sizeof(BMP_Header);

    imageOut->bytes_per_pixel = 4;
    imageOut->norm_height = imageIn->norm_height;
  }
  else if (imageIn->header.bits_per_pixel == 32)
  {
    imageOut->bytes_per_pixel = imageIn->bytes_per_pixel;
    imageOut->norm_height = imageIn->norm_height;
  }

  imageOut->pixels = (Pixel **)malloc(imageOut->norm_height * sizeof(Pixel *));
  if (!imageOut->pixels)
  {
    printError(MEMORY_ERROR);
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < imageOut->norm_height; ++i)
  {
    imageOut->pixels[i] = (Pixel *)malloc(imageIn->header.width_px * sizeof(Pixel));
    if (!imageOut->pixels[i])
    {
      for (int prevRow = 0; prevRow < i; ++prevRow)
      {
        free(imageOut->pixels[prevRow]);
      }
      free(imageOut->pixels);
      printError(MEMORY_ERROR);
      exit(EXIT_FAILURE);
    }
  }
}


/* The input argument is the BMP_Image pointer. The function frees memory of the BMP_Image.
*/
void freeImage(BMP_Image* image) {
  if(image == NULL){
   return;
  }
  
  //liberando la matriz de pixeles
  if(image->pixels != NULL){
    for(int i = 0; i<image->norm_height; i++){
      free(image->pixels[i]);
    }
    free(image->pixels);
  }
  
  image->norm_height =0;
  image->header.size =0;
  image->header.width_px =0;
  image->header.height_px =0;
  image->header.bits_per_pixel =0;
  
  free(image);//liberar la estructura bmpimage
  
}

/* The functions checks if the source image has a valid format.
 * It returns TRUE if the image is valid, and returns FASLE if the image is not valid.
 * DO NOT MODIFY THIS FUNCTION
*/
int checkBMPValid(BMP_Header* header) {
  // Make sure this is a BMP file
  if (header->type != 0x4d42) {
    return FALSE;
  }
  // Make sure we are getting 24 bits per pixel
  if (header->bits_per_pixel != 32) {
    return FALSE;
  }
  // Make sure there is only one image plane
  if (header->planes != 1) {
    return FALSE;
  }
  // Make sure there is no compression
  if (header->compression != 0) {
    return FALSE;
  }
  //printf("hasta aqui fin validacion \n");
  return TRUE;
}

/* The function prints all information of the BMP_Header.
   DO NOT MODIFY THIS FUNCTION
*/
void printBMPHeader(BMP_Header* header) {
  printf("file type (should be 0x4d42): %x\n", header->type);
  printf("file size: %d\n", header->size);
  printf("offset to image data: %d\n", header->offset);
  printf("header size: %d\n", header->header_size);
  printf("width_px: %d\n", header->width_px);
  printf("height_px: %d\n", header->height_px);
  printf("planes: %d\n", header->planes);
  printf("bits: %d\n", header->bits_per_pixel);
}

/* The function prints information of the BMP_Image.
   DO NOT MODIFY THIS FUNCTION
*/
void printBMPImage(BMP_Image* image) {
  printf("data size is %ld\n", sizeof(image->pixels));
  printf("norm_height size is %d\n", image->norm_height);
  printf("bytes per pixel is %d\n", image->bytes_per_pixel);
}
void printBMPPixel(Pixel* pixel) {
  printf("blue is %d\n", pixel->blue);
  printf("green is %d\n", pixel->green);
  printf("red %d\n", pixel->red);
}
