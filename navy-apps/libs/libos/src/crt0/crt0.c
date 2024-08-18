#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  uint8_t *sp = (uint8_t *)args;
  int argc = *(int *)sp;
  sp += sizeof(int);
  char **argv = (char **)sp;
  char **envp = (char **)(sp + sizeof(uint8_t *) * (argc + 1));
  // char *empty[] =  {NULL};
  // char *envp[] = {"PATH=/bin", NULL};
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
