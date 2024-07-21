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
  char *temp = s;
  while(n--) {
    *temp++ = c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  char* move_dst = (char*)dst;
	const char* move_src = (const char*)src;
  // assert(n != 0xffffffff);
  // printf("%d\n", n);
  if(dst < src) {
		for(int i = 0; i < n; i++)
			move_dst[i] = move_src[i];
	}
	else {
		for(int i = n-1; i > 0; i--)
			move_dst[i] = move_src[i];
	}
	return move_dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  assert(out != NULL);
  assert(in != NULL);
  char *temp_out = out;
  const char *temp_in = in;
  while(n--) {
    *temp_out++ = *temp_in++;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  assert(s1 != NULL);
  assert(s2 != NULL);
  const char *temp1 = s1, *temp2 = s2;
  int idx = 0;
  while(idx < n - 1 && temp1[idx] == temp2[idx]) {
    idx++;
  }
  return temp1[idx] - temp2[idx];
}
// int memcmp(const void *s1, const void *s2, size_t n) {
// 	unsigned char *p1 = (unsigned char *)s1;
// 	unsigned char *p2 = (unsigned char *)s2;
// 	int i;
// 	for(i = 0; i < n ; i++)
// 		if(p1[i] != p2[i])
// 			return (p1[i] < p2[i]) ? -1 : 1;
// 	return 0;
// }

#endif
