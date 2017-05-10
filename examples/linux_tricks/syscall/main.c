
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
//#include <sys/syscall.h>
//#include <sys/types.h>

typedef void* (*param0wildcard)(void);
typedef void* (*param1wildcard)(void*);
typedef void* (*param2wildcard)(void*, void*);
typedef void* (*param3wildcard)(void*, void*, void*);

int main()
{
  void *lib = dlopen(NULL, RTLD_LOCAL | RTLD_LAZY);
  void *fn = dlsym(lib, "sleep");
  param1wildcard juggle = fn;
  printf("hey\n");
  juggle((void*)1);
  printf("ho\n");

  void *scfn = dlsym(lib, "syscall");
  param1wildcard mc1 = scfn;
  void* res1 = mc1((void*)39);
  printf("pid: %lu\n", (intptr_t)res1);

  // printf("getpid %d", SYS_getpid); // SYS_getpid 39
  return 0;
}
