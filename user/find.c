#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void
find(char *path, char *pattern)
{
  char buf[512];
  int fd;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
  case T_FILE:
    if (strcmp(path + strlen(path) - strlen(pattern), pattern) == 0)
      printf("%s\n", path);
    break;

  case T_DIR: {
    char *p;
    struct dirent de;

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      fprintf(2, "find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)
        continue;
      int n = strlen(de.name);
      if (de.name[0] == '.' && (n == 1 || (n == 2 && de.name[1] == '.')))
        continue;
      memcpy(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, pattern);
    }
    break;
  }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc < 3) {
    printf("usage: find PATTERN PATH");
    exit(-1);
  }

  find(argv[1], argv[2]);
  exit(0);
}
