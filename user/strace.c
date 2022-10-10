#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int 
main(int argc, char ** argv) 
{
  int pid = fork();
  if(pid < 0) {
    printf("fork(): failed\n");
    exit(1);
  } else if(pid == 0) {
    trace(atoi(argv[1]));
    exec(argv[2], argv + 2);
    printf("exec(): failed\n");
    exit(1);
  }
  exit(0);
}
