#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static char *_out = NULL;
typedef void(*putch_callback)(char);

void sputch(char ch) { *_out++ = ch; }

void itoa(int src, char dst[]) {
  int idx = 0;
  while(src / 10 != 0) {
    dst[idx++] = abs(src % 10) + '0';
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

void xtoa(unsigned long src, char dst[]) {
  int idx = 0;
  while(src / 16 != 0) {
    unsigned long temp = src % 16;
    if(temp < 10)
      dst[idx++] = temp + '0';
    else
      dst[idx++] = temp - 10 + 'a';
    src /= 16;
  }
  if(src < 10)
    dst[idx++] = src + '0';
  else
    dst[idx++] = src - 10 + 'a';
  dst[idx] = '\0';
  int left = 0, right = idx - 1;
  while(left < right) {
    char temp = dst[left];
    dst[left] = dst[right];
    dst[right] = temp;
    left++, right--;
  }
}

int vprint(putch_callback callback, const char *fmt, va_list ap) {
  char c;
  char *s;
  int d;
  unsigned long l;
  char num[12];
  int flag = 0, idx = 0;
  for(; *fmt != '\0'; fmt++) {
    if(*fmt == '%' && flag == 0) {
      flag = 1;
      continue;
    }
    if(flag == 1) {
      switch(*fmt) {
      case '%':
        callback('%');
        idx++;
        break;
      case 's':
        s = va_arg(ap, char *);
        for (int i = 0; i < strlen(s); i++) {
          callback(s[i]);
        }
        idx += strlen(s);
        break;
      case 'd':
        d = va_arg(ap, int);
        itoa(d, num);
        for (int i = 0; i < strlen(num); i++) {
          callback(num[i]);
        }
        idx += strlen(num);
        break;
      case 'c':
        c = va_arg(ap, int);
        callback(c);
        idx++;
        break;
      case 'p':
        l = va_arg(ap, unsigned long);
        xtoa(l, num);
        callback('0');
        callback('x');
        for (int i = 0; i < strlen(num); i++) {
          callback(num[i]);
        }
        idx += strlen(num);
        break;
      default:
        return -1;
      }
      flag = 0;
      continue;
    }
    callback(*fmt);
    idx++;
  }
  return idx;
}

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vprint(putch, fmt, ap);
  va_end(ap);
  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  _out = out;
  int ret = vprint(sputch, fmt, ap);
  sputch('\0');
  ret++;
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
