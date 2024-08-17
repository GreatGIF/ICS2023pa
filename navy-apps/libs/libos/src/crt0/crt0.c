#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  uint8_t *sp = (uint8_t *)args;
  // printf("call_main\n");
  int argc = *(int *)sp;
  sp += sizeof(int);
  char **argv = (char **)sp;
  char **envp = (char **)(sp + 1 + sizeof(uint8_t *) * argc);
  char *empty[] =  {NULL };
  environ = empty;
  exit(main(argc, argv, empty));
  assert(0);
}
