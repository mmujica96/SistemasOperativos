#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main(){
  struct timeval start, end;
  
  //obtener la hora actual antes de la seccion de codigo
  gettimeofday(&start,NULL);
  
  //seccion de codigo cuyo tiempo de ejecucion queremos medir
  usleep(1000000);//dormir por 1 segundo
  
  //obtener la hora actual despues de la seccion de codigo
  gettimeofday(&end,NULL);
  
  
  //calcular tiempo trascurrido en microsegundos
  long seconds = end.tv_sec - start.tv_sec;
  long microseconds = end.tv_usec - start.tv_sec;
  long elapsed = seconds * 1000000 + microseconds;
  
  printf("TIEMPO TRASCURRIDO: %ld microsegundos\n", elapsed);
  
  return 0;
}
