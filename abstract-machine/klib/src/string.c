#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  assert(s != NULL);
  size_t len = 0;
  while(s[len] != '\0') {
    len++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  assert(dst != NULL);
  // assert(sizeof(dst) > strlen(src));
  for (int i = 0; i <= strlen(src); i++) {
    dst[i] = src[i];
  }
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  int len1 = strlen(dst), len2 = strlen(src);
  // assert(len1 + len2 < sizeof(dst));
  for (int i = 0; src[i] != '\0'; i++) {
    dst[len1 + i] = src[i];
  }
  dst[len1 + len2] = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  int idx = 0;
  while(idx < strlen(s1) && idx < strlen(s2) && s1[idx] == s2[idx]) {
    idx++;
  }
  return s1[idx] - s2[idx];
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  assert(s != NULL);
  char *ch = (char *)s;
  for (int i = 0; i < n ; i++) {
    ch[i] = c;
  }
  return ch;
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  assert(s1 != NULL);
  assert(s2 != NULL);
  char *c1 = (char *)s1, *c2 = (char *)s2;
  int idx = 0;
  while(idx < n && idx < sizeof(c1) && idx < sizeof(c2) && c1[idx] == c2[idx]) {
    idx++;
  }
  return c1[idx] - c2[idx];
}

#endif
