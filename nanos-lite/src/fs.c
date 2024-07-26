#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t fp_idx;
} Finfo;

static int file_num = 0;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t serial_write(const void *buf, size_t offset, size_t len);
/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_num = sizeof(file_table) / sizeof(Finfo);
}

int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < file_num; i++) {
    if(strcmp(pathname, file_table[i].name) == 0) {
      file_table[i].fp_idx = 0;
      return i;
    }
  }
  assert(0);
}

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t fs_read(int fd, void *buf, size_t len) {
  if(file_table[fd].read) {
    return file_table[fd].read(buf, file_table[fd].fp_idx, len);
  }
  size_t max_len = file_table[fd].size - file_table[fd].fp_idx;
  len = len > max_len ? max_len : len;
  size_t ret = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].fp_idx, len);
  file_table[fd].fp_idx += ret;
  return ret;
}

size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t fs_write(int fd, const void *buf, size_t len) {
  if(file_table[fd].write) {
    return file_table[fd].write(buf, file_table[fd].fp_idx, len);
  }
  size_t max_len = file_table[fd].size - file_table[fd].fp_idx;
  len = len > max_len ? max_len : len;
  size_t ret = ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].fp_idx, len);
  file_table[fd].fp_idx += ret;
  return ret;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  switch(whence) {
  case SEEK_CUR: break;
  case SEEK_END: file_table[fd].fp_idx = file_table[fd].size; break;
  case SEEK_SET: file_table[fd].fp_idx = 0; break;
  default: return -1;
  }
  file_table[fd].fp_idx += offset;
  if(file_table[fd].fp_idx > file_table[fd].size) {
    file_table[fd].fp_idx = file_table[fd].size;
  }
  return 0;
}
int fs_close(int fd) {
  file_table[fd].fp_idx = 0;
  return 0;
}