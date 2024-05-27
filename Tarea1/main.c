#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //funciones de la libreria posix
#include <errno.h>  //biblioteca para manejo de errores
#include <string.h>
#include <fcntl.h> //biblioteca que contiene el ORDONLY

int main (int argc, char *argv[]){
  //declarar la variables a usar
  char ingreso; //ingreso de caracteres por consola
  const char *nombreArchivo = argv[1]; // puntero del archivo pasado como parametro
  FILE *fichero = fopen(nombreArchivo, "r"); //abrir el archivo en modo lectura
  
  if(argc == 1){
    while((ingreso = getchar()) != EOF){
      putchar(ingreso); //imprime los caracteres
    }
  }
  if(argc == 2){
  //verificar que el fichero/archivo exista
    if (fichero == NULL) {
      //manejar errores
      perror("Error al abrir.\n");
      exit(EXIT_FAILURE);
    }
    char buffer[1024]; //definiendo longitud de la cadena
    size_t nbytes; //numero de bytes leidos
    while((nbytes = fread(buffer, 1, sizeof(buffer), fichero))>0){
        //escribir los datos en pantalla
        write(STDOUT_FILENO, buffer, nbytes);
    }
 }
  if (argc > 2) {
        fprintf(stderr, "Error de uso:\n uso1: ./programa\n uso2: ./programa nombreArchivo\n"); //error standart
        exit(EXIT_FAILURE);
  }
  fclose(fichero); //cerrar archivo
  return 0;
}
