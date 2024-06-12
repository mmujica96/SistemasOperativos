#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

int main (int argc,char *argv[]){
  int p[2];
  pid_t pid;
  
  
  //crear el pipe
  if(pipe(p) == -1){
    exit(1);
  }
  
  
  pid = fork();
  if(pid == -1){
    perror("fork");
    exit(EXIT_FAILURE);
  }else if (pid == 0){//proceso hijo
  struct timeval start;
    close(p[0]); //cerrar el extremo de lectura del pipe
    //obtener el proceso actual antes de crear el proceso hijo
    gettimeofday(&start,NULL);
    write(p[1],&start, sizeof(struct timeval));
    close(p[1]);//cerrar el extremo de escritura del pipe
    
    //dormir por 1 segundo
    usleep(1000000);
    //trabajo del hijo
    if(execvp(argv[1],&argv[1])<0){
      perror("execvp");
      exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
  }else{//Proceso padre
  struct timeval start, end;
    close(p[1]);//cerrar el extremo de escritura del pipe
    read(p[0],&start,sizeof(struct timeval));
    close(p[0]);//cerrar el extremo de lectura del pipe
    //esperar a que el proceso hijo termine
    int status;
    waitpid(pid,&status,0);
    //obtener la hora actual despues de la tarea
    gettimeofday(&end,NULL);
    
    //calcular tiempo trascurrido en microsegundos
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_sec;
    if(microseconds <0){
      seconds -= 1;
      microseconds +=1000000;
    }
    printf("TIempo transcurrido: %ld.%06ld segundos\n",seconds,microseconds);
  }
  return 0;
}
