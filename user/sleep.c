#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc <= 1) {
    fprintf(2, "usage: sleep SECONDS");
    exit(-1);
  }
  int secs = atoi(argv[1]);
  sleep(secs);
  exit(0);
}
