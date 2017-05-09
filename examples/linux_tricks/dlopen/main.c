
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>

typedef int param1str(char[]);
typedef void* (*param1wildcard)(void*);

int main()
{
  void *lib = dlopen(NULL, RTLD_LOCAL | RTLD_LAZY);
  void *fn = dlsym(lib, "printf");
  param1str *deploy = fn;
  int res = deploy("Hello\n");
  printf("result: %d\n", res);
  param1wildcard juggle = fn;
  char arg[] = "World!\n";
  int kappa = (intptr_t)juggle(arg);
  printf("kappa: %d\n", kappa);
  return 0;
}
