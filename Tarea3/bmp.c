#include <stdlib.h>
#include <stdio.h>
#include "filter.h"
#include "bmp.h"

//int ancho;
//int alto;
//int bytesPorPix;
//int dataSize;

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
  fread(&(bmpimage->header), sizeof(BMP_Header), 1, fptr);
  
  //printBMPHeader(&bmpimage->header);//comprobar la informacion del encabezado bmp

  //comprobando que el encabezado cumpla con los parametros establecidos
  if(!checkBMPValid(&(bmpimage->header))){
    printError(VALID_ERROR);
    free(bmpimage);//IMPORTANTE: Liberamos la memoria asignada en caso de no ser valido
    return NULL;
  }

  int ancho = bmpimage->header.width_px;
  int altura = bmpimage->header.height_px;//podemos normalizar la altura
  bmpimage->norm_height = (altura <0) ? -altura : altura;
  
  //asi mismo, asignarle memoria a la matriz de pixeles
  bmpimage->pixels= (Pixel**)malloc(bmpimage->norm_height * sizeof(Pixel*));

  if(bmpimage->pixels == NULL){//verificando si falla la asignacion de memoria
    printError(MEMORY_ERROR);
    free(bmpimage);
    return NULL; 
  }
  for (int i=0; i < bmpimage->norm_height; i++){ //asignando a la matriz 
    bmpimage->pixels[i] = (Pixel*)malloc(ancho * sizeof(Pixel));
    printf("iteracion ..%d\n",i);
    printBMPPixel(&bmpimage->pixels[i]);
    if(bmpimage->pixels[i]==NULL){
      printError(MEMORY_ERROR);
      for(int j =0; j<i; j++){ //comprobando y liberando a cada fila si falla la asignacion
        free(bmpimage->pixels[j]);
      }
      free(bmpimage->pixels);
      free(bmpimage);
      return NULL ;
    }
  }
  printf("fin for\n");
  //fseek para mover/saltar el puntero del arch al inicio de los datps de la imagen 
  fseek(fptr, bmpimage->header.offset, SEEK_SET);
  
  for(int i=0; i< bmpimage->norm_height; i++){//leer cada fila de pixeles
    //int fila = (altura <0) ? i : (altura_normalizada -1 -i);//controlar movimiento de arriba hacia abajo o abajo hacia arrba
    fread(bmpimage->pixels[i],sizeof(Pixel), ancho, fptr);
  }
  
  //normalzando
  //bmpimage->norm_height = altura_normalizada;
  
  printBMPImage(bmpimage);
  printBMPHeader(&bmpimage->header);
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
  
  //obtenemos el numero de bytes por pixeles
  int ancho = image->header.width_px;
  int altura = image->norm_height;
  
  fseek(srcFile, image->header.offset, SEEK_SET);//mover el puntero del archivo a la posicion de los datos de la imagen
  
  for(int i =0; i < altura; i++){//leer los datos de los pixeles de la imagen
    fread(image->pixels[i], sizeof(Pixel), ancho, srcFile);
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
  BMP_Image *dataImageTemp;
  dataImageTemp = createBMPImage(srcFile);
  if(dataImageTemp == NULL){
    return;
  }
  
  //calculamos el tamanio de llos datos de la imagen
  int dataSize = dataImageTemp->header.imagesize;
  if(dataSize == 0){
    dataSize= dataImageTemp->header.width_px*dataImageTemp->norm_height*(dataImageTemp->header.bits_per_pixel /8);
  }
  
  //leemos los datosss de la imagen
  readImageData(srcFile, dataImageTemp, dataSize);
  
  //asignar la imagen leida al puntero los datos a la Imagen
  *dataImage = *dataImageTemp;
  
  //liberar memoria
  free(dataImageTemp);
  
}

/* The input arguments are the destination file name, and BMP_Image pointer.
 * The function write the header and image data into the destination file.
*/
void writeImage(char* destFileName, BMP_Image* dataImage) {
  if(destFileName == NULL || dataImage == NULL){
    printError(ARGUMENT_ERROR);
    return;
  }
  
  FILE *destino = fopen(destFileName, "wb");//abrir el fichero en modo escritura binaria
  if(destino == NULL){
    printError(FILE_ERROR);
    return;
  }
  
  fwrite(&(dataImage->header), sizeof(BMP_Header),1, destino);//escribir en el archivo de destino
  
  //pasar datos dde la imagen al archivo
  int ancho = dataImage->header.width_px;
  int altura = dataImage->norm_height;
  for(int i =0; i< altura; i++){
    fwrite(dataImage->pixels[i], sizeof(Pixel), ancho, destino);
  }
  
  //cerrar el archivo
  fclose(destino);
  
  //comprobando
  printf("La BMPImage fue escrita correctamente en %s\n",destFileName);
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
    printf("hasta aqui2\n");
    return FALSE;
  }
  // Make sure there is only one image plane
  if (header->planes != 1) {
    printf("hasta aqui3\n");
    return FALSE;
  }
  // Make sure there is no compression
  if (header->compression != 0) {
    printf("hasta aqui4\n");
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
