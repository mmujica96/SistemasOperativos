#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <string.h>

int main (int argc,char *argv[]){
  int p[2];
  
  //para cubri el error de uso y los argumentos
  if(argc<2){
    printf ("Uso: \n ./time <arg1> \n ./time <arg1> <arg2> ... <argn>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  //crear el pipe
  if(pipe(p) == -1){
    perror("Error al crear el pipe");
    exit(EXIT_FAILURE);
  }

  //crear el proceso
  pid_t pid = fork();
  //error al crear el proceso
  if(pid == -1){
    perror("Error al crear el proceso");
    exit(EXIT_FAILURE);
  }
  if (pid == 0){//proceso hijo
    close(p[0]); //cerrar el extremo de lectura del pipe
    
    struct timeval start_time;
    //obtener el hora actual antes del proceso hijo
    gettimeofday(&start_time,NULL);
    
    write(p[1],&start_time, sizeof(struct timeval)); //escribir el tiempo de inicio 
    close(p[1]);//cerrar el extremo de escritura del pipe
    
    //trabajo del hijo
    if(execvp(argv[1],&argv[1])<0){
      perror("execvp");
      exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
  }else{//Proceso padre
    close(p[1]);//cerrar el extremo de escritura del pipe
    
    struct timeval start_time;
    read(p[0],&start_time,sizeof(struct timeval));
    close(p[0]);//cerrar el extremo de lectura del pipe
    
    //esperar a que el proceso hijo termine
    wait(NULL);
    
    //obtener la hora actual despues de la tarea
    struct timeval end_time;
    gettimeofday(&end_time,NULL);
    
    //calcular tiempo trascurrido en microseg
    long segundos = end_time.tv_sec - start_time.tv_sec;
    long microseg = end_time.tv_usec - start_time.tv_usec;
    double elapsed = segundos + microseg*1e-6;
    
    printf("TIempo transcurrido: %.6f seconds\n", elapsed);
  }
  return 0;
}
