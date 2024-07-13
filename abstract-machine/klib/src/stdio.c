#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void itoa(int src, char dst[]) {
  int idx = 0;
  while(src / 10 != 0) {
    dst[idx++] = src % 10 + '0';
    src /= 10;
  }
  dst[idx++] = abs(src) + '0';
  if(src < 0) {
    dst[idx++] = '-';
  }
  dst[idx] = '\0';
  int left = 0, right = idx - 1;
  while(left < right) {
    char temp = dst[left];
    dst[left] = dst[right];
    dst[right] = temp;
    left++, right--;
  }
}

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  char *s;
  int d;
  va_start(ap, fmt);
  int flag = 0, idx = 0;
  for (int i = 0; fmt[i] != '\0'; i++) {
    if(fmt[i] == '%') {
      flag = 1;
      continue;
    }
    if(flag == 1) {
      switch(fmt[i]) {
      case '%':
        out[idx++] = '%';
        break;
      case 's':
        s = va_arg(ap, char *);
        strcpy(out + idx, s);
        idx += strlen(s);
        break;
      case 'd':
        d = va_arg(ap, int);
        char num[10];
        itoa(d, num);
        strcpy(out + idx, num);
        idx += strlen(num);
        break;
      default:
        va_end(ap);
        return -1;
      }
      flag = 0;
      continue;
    }
    out[idx++] = fmt[i];
  }
  out[idx] = '\0';
  va_end(ap);
  return idx;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
