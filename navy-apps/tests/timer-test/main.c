#include <time.h>
#include <stdio.h>
#include <NDL.h>

int main() {
  NDL_Init(0);
  int pre = NDL_GetTicks();
  while(1) {
    int curr = NDL_GetTicks();
    if(curr - pre > 500) {
      printf("111curr:%d\n", curr);
      pre = curr;
    }
  }
  NDL_Quit();
  return 0;
}