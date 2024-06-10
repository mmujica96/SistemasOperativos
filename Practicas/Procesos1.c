#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
main()
{
  int pid, status;
  switch(pid=fork()){
    case -1: /*Gestion del error*/
            exit(1);
            break;
    case 0: /*Proceso Hijo*/
            printf("\n Proceso hijo, PID= %d\n",pid);
            break;
    default: /*Proceso Padre*/
            printf("\n Proceso padre, PID= %d\n",pid);
            wait(&status); /*espera al final del proceso hijo*/
            break;       
  }/*cierre del switch*/
  printf("\n Fin de la ejecucion \n");
  return 0;
}
