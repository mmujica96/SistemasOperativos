#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
  int p[2];
  pid_t pid;
  char msg_padre[]="Hola, hijo!";
  char msg_hijo[20];
  
  //crear el pipe
  if(pipe(p)<0){
    perror("Error al crear el pipe");
    return 1;
  }
  
  //crear un nuevo proceso hijo
  pid= fork();
  
  if(pid<0){
    perror("Error al crear el proceso hijo");
    return 1;
  }else if(pid ==0){
    //proceso hijo
    close(p[1]);//cerrar el descriptor de escritura en el proce hijo
    //leer desde el pipe
    read(p[0],msg_hijo,size_of(msg_hijo));
    printf("Mensaje recibido en el proceso hijo: %s\n",msg_hijo);
    close(p[0]);
  }else{
    close(p[0]);
    write(p[1],msg_padre,size_of(msg_padre));
    close(p[1]);
    wait(NULL);
  }
  
}
