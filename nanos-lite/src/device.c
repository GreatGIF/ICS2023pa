#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  const char *str = buf/* + offset*/;
  for (int i = 0; i < len; i++) {
    putch(str[i]);
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if(ev.keycode == AM_KEY_NONE) {return 0;}
  size_t name_len = strlen(keyname[ev.keycode]);
  // size_t min_len = 3 + name_len;
  size_t min_len = 4 + name_len;
  len = len > min_len ? min_len : len;
  char *head;
  if(ev.keydown) {
    head = "kd ";
  } else {
    head = "ku ";
  }
  strcpy(buf, head);
  strcpy(buf + 3, keyname[ev.keycode]);
  strcpy(buf + 3 + name_len, "\n");
  return len;
}

static uint32_t screen_w = 0, screen_h = 0;

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T gpu_config = io_read(AM_GPU_CONFIG);
  screen_w = gpu_config.width, screen_h = gpu_config.height;
  char temp[48];
  int ret = sprintf(temp, "WIDTH:%d\nHEIGHT:%d\n", screen_w, screen_h);
  len = ret > len ? len : ret;
  buf = strncpy(buf, temp, len);
  return len;
}

// Input a line to the screen and output it byte by byte
size_t fb_write(const void *buf, size_t offset, size_t len) {
  int x = (offset % (screen_w * 4)) / 4 , y = offset / (screen_w * 4);
  io_write(AM_GPU_FBDRAW, x, y, (uint32_t *)buf, len / 4, 1, true);
  return 1;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
