#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv[]){
  if(argc<2){
    printf ("Uso: \n ./time <arg1> \n ./time <arg1> <arg2> ... <argn>\n", argv[0]);
    return 1;
  }
  //tamanio de la cadena
  int size_cadena=0;
  //guardar los argumentos
  for(int i=1; i<argc; i++){
    size_cadena+= strlen(argv[i])+1;
  }
  //guardar los argumentos en un arreglo
  char *ruta= (char *)malloc(size_cadena *sizeof(char));
  if(ruta == NULL){
    fprintf(stderr, "Error al asignar memoria\n");
    return 1;
  }
  //inicializar la cadena como vacia
  ruta[0]= '\0';

  //concatenar en una sola cedena
  for(int i=1; i<argc;i++){
    strcat(ruta, argv[i]);
    if(i<argc -1){
      strcat(ruta," ");//agregar el espacio entre los argumentos
    }
  }
  printf("Resultado: %s\n", ruta);
   //crear un proceso hijo
  pid_t pid=fork();
  if(pid<0){
    //error
    perror("Error al crear el proceso hijo");
    exit(EXIT_FAILURE);
  }else if (pid == 0){//proceso hijo
    execl(ruta,ruta,NULL);
    //en caso que execl retorna, hubo un error
    perror("Error al ejecutar el programa");
    exit(EXIT_FAILURE);
  }else{//proceso padre
    //espera a que el hijo termine
    int status;
    waitpid(pid,&status,0);
    //verificacion cuando el hijo termina
    if(WIFEXITED(status)){
      printf("El programa se ejecuto correctamente.\n");
    }else{
      printf("El programa termino con errores.\n");
    }
  }

  //liberar memoria asignada
  free(ruta);
  return 0;
}
