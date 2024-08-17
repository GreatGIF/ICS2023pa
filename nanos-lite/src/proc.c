#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    // Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg) {
  pcb->cp = kcontext((Area){pcb->stack, pcb + 1}, entry, arg);
}


void init_proc() {
  // switch_boot_pcb();

  // Log("Initializing processes...");

  // // load program here
  // extern void naive_uload(PCB * pcb, const char *filename);
  // naive_uload(NULL, "/bin/nterm");
  context_kload(&pcb[0], hello_fun, (char *)"first");
  // context_kload(&pcb[1], hello_fun, (char *)"second");
  char *const argv[] = {NULL};
  char *const envp[] = {NULL};
  extern void context_uload(PCB * pcb, const char *filename, char *const argv[], char *const envp[]);
  context_uload(&pcb[1], "/bin/nterm", argv, envp);
  switch_boot_pcb();
}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;
  // switch between pcb[0] and pcb[1]
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  // then return the new context
  return current->cp;
}
