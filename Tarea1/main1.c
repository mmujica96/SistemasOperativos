#include <stdio.h>
#include <stdlib.h>


int main (int argc, char *argv[]){
  if (argc >2) {
      printf("Leyendo: %s <archivo>\n", argv[0]);
      return 1;
  }
  
  char *nombreArchivo = argv[1]; //obtener el archivo pasado por argumento
  char caracter;
  
  FILE *archivo = fopen(nombreArchivo, "r");
  
  if(archivo == NULL){
    printf("Error al abrir el archivo.\n");
    return 1;
  }
  
  while((caracter= fgetc(archivo)) != EOF){
    putchar(caracter); //muestra los caracteres por pantalla
  }
  fclose(archivo);
  
  return 0;
}
