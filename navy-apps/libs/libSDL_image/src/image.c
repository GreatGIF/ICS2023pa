#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

// SDL_Surface* IMG_Load(const char *filename) {
//   FILE *fp = fopen(filename, "rb");
//   fseek(fp, 0, SEEK_END);
//   int size = ftell(fp);
//   char *buf = (char *)malloc(size);
//   fread(buf, size, 1, fp);
//   SDL_Surface *surface = STBIMG_LoadFromMemory(buf, size);
//   free(buf);
//   fclose(fp);
//   return surface;
// }
SDL_Surface* IMG_Load(const char *filename) {
  int filesize;
  char *buf;
  SDL_Surface *s = malloc(sizeof(SDL_Surface));
  FILE *file = fopen(filename ,"rb");
  if(file == NULL){printf("no file name %s\n",filename);
    assert(0);
  }

  fseek(file, 0, SEEK_END);
  filesize = ftell(file);
  rewind(file);
  buf = (char *)malloc(filesize);
  fread(buf, filesize, 1, file);
  s = STBIMG_LoadFromMemory(buf, filesize);

  free(buf);
  fclose(file);
  return s;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
