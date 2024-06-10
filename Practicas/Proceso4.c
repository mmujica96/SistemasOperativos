#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>

#define MAX_PROGRAM_NAME_LENGTH 100

int main(int argc, char *argv[]) {
  if(argc !=2){
    fprintf(stderr, "USO: ./programa ruta_programa\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  //asignacion del argumento
  char *ruta= argv[1];
  
  //crear un proceso hijo para ejecutar el programa
  pid_t pid=fork();
  if(pid<0){
    //error
    perror("Error al crear el proceso hijo");
    exit(EXIT_FAILURE);
  }else if (pid==0){
    //proceso hijo
    //ejecutar el programa en el directorio /bin
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
  return 0;
}

