#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/* should close @in */
int sieve(int in)
{
  int res, fds[2], buf[40], n;

  n = read(in, buf, sizeof(buf));
  close(in);
  if (n == 0)
    return 0;
  n /= sizeof(int);

  printf("prime %d\n", buf[0]);

  if (n < 2)
    return 0;

  res = pipe(fds);
  if (res) {
    fprintf(2, "create pipe failed: %d\n", res);
    return res;
  }

  for (int i = 1; i < n; i++) {
    if (buf[i] % buf[0] != 0)
      write(fds[1], &buf[i], sizeof(int));
  }
  close(fds[1]);

  if ((res = fork()) == 0) {
    /* child */
    res = sieve(fds[0]);
    if (res)
      fprintf(2, "call sieve failed: %d\n", res);
    exit(0);
  } else if (res < 0) {
    fprintf(2, "fork failed: %d\n", res);
    return -1;
  }

  wait(0);
  return 0;
}

int
main(int argc, char *argv[])
{
  int res, fds[2];

  res = pipe(fds);
  if (res)
    return res;

  for (int i = 2; i <= 35; i++)
    write(fds[1], &i, sizeof(i));
  close(fds[1]);

  res = sieve(fds[0]);
  if (res)
    return res;

  wait(0);
  printf("OK\n");
  exit(0);
}
