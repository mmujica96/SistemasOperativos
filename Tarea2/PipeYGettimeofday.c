#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

int main (){
  int pipefd[2];
  pid_t pid;
  struct timeval, start, end;
  
  //crear el pipe
  if(pipe(pipefd) == -1){
    perror("pipe");
    exit(EXIT?FAILURE);
  }
  
  //obtener el proceso actual antes de crear el proceso hijo
  gettimeofday(&start,NULL);
  
  pid = fork();
  if(pid == -1){
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if(pid == 0){//proceso hijo
    close(pipefd[0]); //cerrar el extremo de lectura del pipe
    
    //SImular trabajo del proceso hijo
    usleep(1000000); //dormir por 1 segundo
    
    //obtener la hora actual despues de la tarea
    gettimeofday(&end,NULL);
    
    //calcular tiempo trascurrido en microsegundos
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_sec;
    long elapsed = seconds * 1000000 + microseconds;
    
    //escribir el tiempo transcurrido en el pipe
    write(pipefd[1],&elapsed, sizeof(elapsed));
    close(pipefd[1]);//cerrar el extremo de escritura del pipe
    
    exit(EXIT_SUCCESS);
  }else{//Proceso padre
    close(pipefd[1]);//cerrar el extremo de escritura del pipe
    
    //esperar a que el proceso hijo termine
    wait(NULL);
    
    //Leer el tiempo transcurrido desde el pipe
    long elapsed;
    read(pipefd[0],&elapsed,sizeof(elapsed));
    close(pipefd[0]);//cerrar el extremo de lectura del pipe
    
    printf("TIempo transcurrido: %ld microsegundos\n",elapsed);
  }
  return 0;
}
