#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

struct string {
  char *base, *p, *end;
};

int
init_string(struct string *s, unsigned int size)
{
  s->base = malloc(size);
  if (!s->base)
    return -1;
  s->p = s->base;
  s->end = s->base + size;
  return 0;
}

inline unsigned int
string_size(struct string *s)
{
  return s->p - s->base;
}

int
string_refill(struct string *s)
{
  if (s->p == s->end) {
    char *oldbase = s->base;
    unsigned int oldsize = s->end - s->base;
    unsigned int size = oldsize * 3 / 2;

    s->base = malloc(size);
    memcpy(s->base, oldbase, oldsize);
    free(oldbase);
    s->p = s->base + oldsize;
    s->end = s->base + size;
  }
  return 0;
}

inline char *
string_get(struct string *s)
{
  string_refill(s);
  *s->p = 0;
  return s->base;
}

inline int
string_push(struct string *s, const char ch)
{
  string_refill(s);
  *s->p++ = ch;
  return 0;
}

inline void
string_clear(struct string *s)
{
  s->p = s->base;
}

inline void
free_string(struct string *s)
{
  free(s->base);
}

int
getchar()
{
  int err;
  char ret;

  err = read(0, &ret, sizeof(char));
  if (err == 0)
    return 0;

  return ret;
}

int
readline(struct string *s)
{
  char ch;

  string_clear(s);

  ch = getchar();
  if (ch == 0)
    return 0;

  do {
    string_push(s, ch);
  } while ((ch = getchar()) != '\n' && ch != 0);
  return 1;
}

int
main(int argc, char *argv[])
{
  char *nargv[MAXARG];

  if (argc < 2) {
    fprintf(2, "usage: xargs PROG ARGS\n");
    exit(-1);
  }

  memset(nargv, 0, MAXARG * sizeof(char *));
  memmove(nargv, &argv[1], (argc - 1) * sizeof(char *));

  struct string line;
  init_string(&line, 16);
  while (readline(&line)) {
    nargv[argc - 1] = string_get(&line);
    nargv[argc] = 0;

    if (fork() == 0) {
      exec(nargv[0], nargv);
      fprintf(2, "exec failed.\n");
      exit(-1);
    }

    wait(0);
  }

  free_string(&line);
  exit(0);
}
