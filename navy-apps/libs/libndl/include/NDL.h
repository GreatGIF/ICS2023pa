#ifndef __NDL_H__
#define __NDL_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int NDL_Init(uint32_t flags);
void NDL_Quit();
uint32_t NDL_GetTicks();
void NDL_OpenCanvas(int *w, int *h);
int NDL_PollEvent(char *buf, int len);
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h);
void NDL_OpenAudio(int freq, int channels, int samples);
void NDL_CloseAudio();
int NDL_PlayAudio(void *buf, int len);
int NDL_QueryAudio();

int NDL_Open(const char *pathname);
int NDL_Lseek(int fd, int offset, int whence);
int NDL_Read(int fd, void *buf, int nbyte);
int NDL_Write(int fd, const void *buf, int nbyte);
int NDL_Close(int fd);

#ifdef __cplusplus
}
#endif

#endif
