#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;
static int canvas_x = 0, canvas_y = 0;

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", 0);
  // printf("fd = %d\n", fd);
  int ret = read(fd, buf, len);
  close(fd);
  return ret == 0 ? 0 : 1;
}

void get_vga_size() {
  if(screen_w != 0 && screen_h != 0) {return;}
  char buf[48];
  int fd = open("/proc/dispinfo", 0, 0);
  read(fd, buf, 48);
  int i = 0;
  for(; buf[i] > '9' || buf[i] < '0'; i++);
  screen_w = atoi(buf + i);
  for(; buf[i] != '\n'; i++);
  for(; buf[i] > '9' || buf[i] < '0'; i++);
  screen_h = atoi(buf + i);
  // printf("screen_W=%d, screen_h=%d\n", screen_w, screen_h);
}

void NDL_OpenCanvas(int *w, int *h) {
  if(*w == 0 && *h == 0) {
    get_vga_size();
    *w = screen_w, *h = screen_h;
  }
  canvas_w = *w, canvas_h = *h;
  canvas_x = (screen_w - canvas_w) / 2;
  canvas_y = (screen_h - canvas_h) / 2;
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  // printf("*w=%d, *h=%d, scr_w=%d, scr_h=%d\n", *w, *h, screen_w, screen_h);
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  x += canvas_x, y += canvas_y;
  // printf("srcw=%d, srch=%d, x=%d, y=%d\n", screen_w, screen_h, x, y);
  int fd = open("/dev/fb", 0, 0);
  // printf("fd = %d\n", fd);
  for (int i = 0; i < h; i++) {
    off_t ret = lseek(fd, ((y + i) * screen_w + x) * 4, SEEK_SET);
    write(fd, pixels + i * w, w * 4);
    // printf("off = %ld\n", lseek(fd, 0, SEEK_CUR));
  }
  close(fd);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
  int sbctl[3] = {freq, channels, samples};
  int fd = open("/dev/sbctl", 0, 0);
  write(fd, sbctl, 3 * sizeof(int));
  close(fd);
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  int fd = open("/dev/sb", 0, 0);
  write(fd, buf, len);
  close(fd);
  return len;
}

int NDL_QueryAudio() {
  int fd = open("/dev/sbctl", 0, 0);
  int space;
  read(fd, &space, sizeof(int));
  close(fd);
  return space;
}

int NDL_Init(uint32_t flags) {
  get_vga_size();
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}

int NDL_Open(const char *pathname) {
  int fd = open(pathname, 0, 0);
  fd = open(pathname, 0, 0);
  return fd;
}

int NDL_Lseek(int fd, int offset, int whence) {
  return lseek(fd, offset, whence);
}

int NDL_Read(int fd, void *buf, int nbyte) {
  return read(fd, buf, nbyte);
}

int NDL_Write(int fd, const void *buf, int nbyte) {
  return write(fd, buf, nbyte);
}

int NDL_Close(int fd) {
  return close(fd);
}