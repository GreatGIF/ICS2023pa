#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[64];
  if(NDL_PollEvent(buf, sizeof(buf)) == 0) {return 0;}
  ev->type = buf[1] == 'd' ? SDL_KEYDOWN : SDL_KEYUP;
  ev->key.type = ev->type;
  // printf("event:%s\n", buf);
  int i = 3;
  for(; buf[i] != '\n'; i++);
  buf[i] = '\0';
  for (int i = 0; i < 83; i++) {
    if(strcmp(keyname[i], buf + 3) == 0) {
      ev->key.keysym.sym = i;
      // printf("i:%d\n", i);
      return 1;
    }
  }
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[64];
  while(1) {
    if(NDL_PollEvent(buf, sizeof(buf)) == 0) {continue;}
    event->type = buf[1] == 'd' ? SDL_KEYDOWN : SDL_KEYUP;
    event->key.type = event->type;
    // printf("event:%s\n", buf);
    int i = 3;
    for(; buf[i] != '\n'; i++);
    buf[i] = '\0';
    for (int i = 0; i < 83; i++) {
      if(strcmp(keyname[i], buf + 3) == 0) {
        event->key.keysym.sym = i;
        // printf("i:%d\n", i);
        return 1;
      }
    }
  }
  return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  assert(0);
  return NULL;
}
