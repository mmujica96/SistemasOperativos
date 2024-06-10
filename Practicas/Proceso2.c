#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

main()
{
  int pid,status;
  if(fork()==0)
    execl("/bin/date", "date", 0);
  wait(&status);
}
