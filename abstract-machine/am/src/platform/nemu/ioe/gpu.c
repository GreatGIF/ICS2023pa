#include <am.h>
#include <nemu.h>

#define WIDTH 400
#define HIGH  300

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  // int i;
  // int w = io_read(AM_GPU_CONFIG).width;
  // int h = io_read(AM_GPU_CONFIG).height;
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // for (i = 0; i < w * h; i ++) fb[i] = i;
  // outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint16_t w = inw(VGACTL_ADDR + 2), h = inw(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = w * h * 4
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
  int vga_W = io_read(AM_GPU_CONFIG).width;
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  uint32_t *p = (uint32_t *)ctl->pixels;
  uint32_t *start = (uint32_t *)(uintptr_t)(FB_ADDR) + (y *  vga_W + x);
  if (w == 0 || h == 0) return;
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      start[j] = *p++;
    }
    start += vga_W;
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
