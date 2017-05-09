
#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>

int main() {
  char path[MAXPATHLEN];
  ssize_t len = readlink("/proc/self/exe", path, sizeof(path));
  if (len < 0) {
    perror("readlink(\"/proc/self/exe\")");
    return 1;
  }
  path[len] = '\0';
  printf("path: (%s)\n", path);
}
