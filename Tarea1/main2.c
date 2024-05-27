//permite leer varios archivos e imprimirlos
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void cat(const char * nombreArchivo){
  char buffer[4096];
  ssize_t nbytes;
  FILE *archivo = fopen(nombreArchivo, "r"); //abrimos el archivo en modo lectura
  
  if(archivo == NULL){
    perror("Error al abrir el archivo");
    return;
  }
  while((nbytes = fread(buffer, sizeof(char), sizeof(buffer), archivo)) !=0){
    fwrite(buffer, sizeof(char), nbytes, stdout); //imprimir en pantalla
  }
  fclose(archivo);
}

int main (int argc, char *argv[]){
  if (argc < 2) {
      printf("Leyendo: %s <archivo>\[<archivo2> ...]n", argv[0]);
      return 1;
  }
  for (int i=1; i< argc; i++){
    cat(argv[i]);
  }
  
  return 0;
  
}
  
