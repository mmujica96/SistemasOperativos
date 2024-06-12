#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/time.h>

int main(int argc,char *argv[]){
  pid_t pid;
  struct timeval start, end;
  
  //para cubri el error de uso y los argumentos
  if(argc<2){
    printf ("Uso: \n ./time <arg1> \n ./time <arg1> <arg2> ... <argn>\n", argv[0]);
    return 1;
  }
  struct timeval *sh_memory = mmap(NULL, sizeof(struct timeval) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if(sh_memory == MAP_FAILED){
    perror("Error al crear mmap");
    exit(EXIT_FAILURE);
  }
  
  //crear un proceso hijo
  pid=fork();
  if(pid<0){
    //error
    perror("Error al crear el proceso hijo");
    exit(EXIT_FAILURE);
  }
  if (pid == 0){//proceso hijo
    //hora actual
    gettimeofday(&sh_memory[0],NULL);
  
    if(execvp(argv[1],&argv[1])<0){
      perror("execvp");
      exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
  }else{//proceso padre
    //espera a que el hijo termine
    wait(NULL);
    
    //hora actual del fin de la ejecucion
    gettimeofday(&sh_memory[1], NULL);
    
    //tiempo transcurrido en segundos
    double seg = (sh_memory[1].tv_sec - sh_memory[0].tv_sec) + (sh_memory[1].tv_usec - sh_memory[0].tv_usec)/1e6;
    

    printf("Tiempo transcurrido: %.6f segundos\n", seg);
    
    //no olvidar desmapear la memoria compartida
    munmap(sh_memory, sizeof(struct timeval) * 2);
  }
  return 0;
}
