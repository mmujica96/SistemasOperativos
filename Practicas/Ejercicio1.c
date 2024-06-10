/*Crear un proceso que leera por el terminal el nombre e un prg y seguidamente lo ejecutara. El programa a ejecutar debe residir en el directorio /bin , NO se debe ejecutar un programa hasta que el anterior no haya acabado, el proceso terminara cuando se introduszca la cadena "salir"*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>

#define MAX_PROGRAM_NAME_LENGTH 100

int main() {
  char programa[MAX_PROGRAM_NAME_LENGTH];
  
  //el usuario debe ingresar el nombre del programa
  while(1){
    printf("Ingrese el nombre del programa a ejecutar: ");
    scanf("%s", programa);
    
    //comprobar si se quiere salir del programa
    if(strcmp(programa,"salir")==0){
      printf("Saliendo del programa..\n");
      break;
    }
    //construir la ruta completa del programa
    char ruta[] = "/bin/";
    strcat(ruta,programa);
    //crear un proceso hijo para ejecutar el programa
    pid_t pid=fork();
    if(pid<0){
      //error
      perror("Error al crear el proceso hijo");
      exit(EXIT_FAILURE);
    }else if (pid==0){
      //proceso hijo
      //ejecutar el programa en el directorio /bin
      execl(ruta,programa,NULL);
      
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
  }
  return 0;
}

