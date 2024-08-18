#include <common.h>
#include "syscall.h"
#include <fs.h>
#include <proc.h>

#ifdef CONFIG_STRACE
static void strace(int no, int a1, int a2, int a3, int ret) {
  printf("strace: ");
  switch(no) {
    case SYS_yield: printf("sys_yield() = 0\n"); break;
    case SYS_write: printf("sys_write(fd = %d(%s), buf = %p, count = %d) = %d\n", a1, fd2filepath(a1), a2, a3, ret); break;
    case SYS_read: printf("sys_read((fd = %d(%s), buf = %p, count = %d) = %d\n", a1, fd2filepath(a1), a2, a3, ret); break;
    case SYS_open: printf("sys_open(path = %s, flags = %d, mode = %d) = %d\n", a1, a2, a3, ret); break;
    case SYS_close: printf("sys_close(fd = %d(%s)) = %d\n", a1, fd2filepath(a1)); break;
    case SYS_lseek: printf("sys_lseek(fd = %d(%s), offset = %d, whence = %d) = %d\n", a1, fd2filepath(a1), a2, a3, ret); break;
    case SYS_brk: printf("sys_brk() = 0\n"); break;
    case SYS_execve: printf("sys_execve(filename = %s) = %d\n", a1, ret); break;
    case SYS_gettimeofday: printf("sys_gettimeofday(tv = %p, tz = %p) = %d\n", a1, a2, ret); break;
  }
}
#endif

static int sys_yield() {
  yield();
  return 0;
}

// static void sys_exit(int type) {
// #ifdef CONFIG_STRACE
//   printf("STRACE: sys_exit(type = %d)\n", type);
// #endif
//   halt(type);
// }

static size_t sys_write(int fd, const void *buf, size_t count) {
  return fs_write(fd, buf, count);
}

static size_t sys_read(int fd, void *buf, size_t count) {
  return fs_read(fd, buf, count);
}


static int sys_open(const char *path, int flags, int mode) {
  return fs_open(path, flags, mode);
}

static int sys_close(int fd) {
  return fs_close(fd);
}

static size_t sys_lseek(int fd, size_t offset, int whence) {
  return fs_lseek(fd, offset, whence);
}

struct timeval {
	long		tv_sec;		/* seconds */
	long	tv_usec;	/* and microseconds */
};
struct timezone {
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;	/* type of dst correction */
};
static int sys_gettimeofday(struct timeval *tv, struct timezone *tz) {
  uint64_t time = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec = time / 1000000;
  tv->tv_usec = time % 1000000;
  return 0;
}

int sys_execve(const char *pathname, char *argv[], char * envp[]) {
  // Log("filename=%s.", pathname);
  extern void context_uload(PCB * pcb, const char *filename, char *const argv[], char *const envp[]);
  // char *empty[] = {NULL};
  context_uload(current, pathname, argv, envp);
  if(current->cp->mepc == 0) {return -2;}
  extern void switch_boot_pcb();
  switch_boot_pcb();
  // Log("here.");
  yield();
  return 0;
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
    case SYS_read: c->GPRx = sys_read((int)a[1], (void *)a[2], (size_t)a[3]); break;
    case SYS_open: c->GPRx = sys_open((const char *)a[1], (int)a[2], (int)a[3]); break;
    case SYS_close: c->GPRx = sys_close((int)a[1]); break;
    case SYS_lseek: c->GPRx = sys_lseek((int)a[1], (size_t)a[2], (int)a[3]); break;
    case SYS_brk: c->GPRx = 0; break;
    case SYS_gettimeofday: c->GPRx = sys_gettimeofday((struct timeval *)a[1], (struct timezone *)a[2]); break;
    // case SYS_execve: 
    // if(sys_execve((char *)a[1], (char **const)a[2], (char **const)a[3]) == -2) {
    //   extern int errno;
    //   c->GPRx = -1;
    //   errno = 2;
    // }else {
    //   c->GPRx = 1;
    // }
    // break;
    case SYS_execve: c->GPRx = sys_execve((char *)a[1], (char **const)a[2], (char **const)a[3]); break;
    // case SYS_exit: sys_exit((int)a[1]); break;
    case SYS_exit: char *empty[] = {NULL}; c->GPRx = sys_execve("/bin/nterm", empty, empty); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

#ifdef CONFIG_STRACE
  uintptr_t ret = c->GPRx;
  strace(a[0], a[1], a[2], a[3], ret);
#endif
}

