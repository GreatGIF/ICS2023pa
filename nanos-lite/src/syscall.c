#include <common.h>
#include "syscall.h"

static int sys_yield() {
  yield();
#ifdef CONFIG_STRACE
  printf("STRACE: sys_yield() = 0\n");
#endif
  return 0;
}

static void sys_exit(int type) {
#ifdef CONFIG_STRACE
  printf("STRACE: sys_exit(type = %d)\n", type);
#endif
  halt(type);
}

static size_t sys_write(int fd, const void *buf, size_t count) {
  size_t ret;
  if(fd == 1 || fd == 2) {
    const char *str = buf;
    for (int i = 0; i < count; i++) {
      putch(str[i]);
    }
    ret = count;
  }else {
    ret = -1;
  }
#ifdef CONFIG_STRACE
  printf("STRACE: sys_write(fd = %d, buf = %p, count = %d) = %d\n", fd, buf, count, ret);
#endif
  return ret;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield: c->GPRx = sys_yield(); break;
    case SYS_write: c->GPRx = sys_write((int)a[1], (void *)a[2], (size_t)a[3]); break;
    case SYS_brk: c->GPRx = 0; break;
    case SYS_exit: sys_exit(1); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
