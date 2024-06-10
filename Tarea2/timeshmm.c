#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv[]){
  if(argc<2){
    printf ("Uso: \n ./time <arg1> \n ./time <arg1> <arg2> ... <argn>\n", argv[0]);
    return 1;
  }
   //crear un proceso hijo
  pid_t pid=fork();
  if(pid<0){
    //error
    perror("Error al crear el proceso hijo");
    exit(EXIT_FAILURE);
  }else if (pid == 0){//proceso hijo
    if(execvp(argv[1],&argv[1])<0){
      perror("execvp");
      exit(EXIT_FAILURE);
    }
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
