// #include <NDL.h>
// #include <SDL.h>
// #include <stdlib.h>

// static int freq, gap, channels, samples;
// static uint8_t *stream;
// void (*callback)(void *userdata, uint8_t *stream, int len);

// void CallbackHelper(int flag) { //1: start; 0: stop; 2: contunue
//   static uint32_t timer = 0;
//   static int st = 0; // 1 running ; 0 no working
//   timer = (timer == 0) ? SDL_GetTicks() : timer;
//   if(flag == 1) st = 1;
//   if(flag == 0) st = 0;
//   if(st == 0) return;
//   if(SDL_GetTicks() - timer < gap) return;
//   timer = SDL_GetTicks();
//   int len = NDL_QueryAudio();
//   len = (len > samples) ? samples : len;
//   callback(NULL, stream, len);
//   NDL_PlayAudio(stream, samples);
// }

// int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
//   *obtained = *desired;
//   freq = desired->freq;
//   gap = 1000 / freq;
//   channels = desired->channels;
//   samples = desired->samples;
//   callback = desired->callback;
//   NDL_OpenAudio(freq, channels, samples);
//   stream = (uint8_t *)malloc(samples);
//   CallbackHelper(1);
//   return 0;  //
// }

// void SDL_CloseAudio() {
//   CallbackHelper(0);
//   NDL_CloseAudio();
// }

// void SDL_PauseAudio(int pause_on) {
//   if(pause_on != 0) return;
//   // uint32_t delay = 1000 / freq;
//   // uint8_t *stream = (uint8_t *)malloc(samples);
//   CallbackHelper(1);
// }

// void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
// }

// SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
//   return NULL;
// }

// void SDL_FreeWAV(uint8_t *audio_buf) {
// }

// void SDL_LockAudio() {
// }

// void SDL_UnlockAudio() {
// }
#include <NDL.h>
#include <SDL.h>

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
  return 0;
}

void SDL_CloseAudio() {
}

void SDL_PauseAudio(int pause_on) {
}

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
  return NULL;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
}

void SDL_LockAudio() {
}

void SDL_UnlockAudio() {
}
