#include <fixedptc.h>
#include <assert.h>
#include <stdio.h>

int main() {
  fixedpt a = fixedpt_rconst(1.0);
  fixedpt b = fixedpt_rconst(2.1);
  fixedpt c = fixedpt_rconst(0.1);
  fixedpt d = fixedpt_fromint(0);
  fixedpt e = fixedpt_rconst(-1.0);
  fixedpt f = fixedpt_rconst(-2.1);
  fixedpt g = fixedpt_rconst(-3.1);
  
  assert(1 == fixedpt_toint(fixedpt_floor(a)));
  assert(2 == fixedpt_toint(fixedpt_floor(b)));
  assert(0 == fixedpt_toint(fixedpt_floor(c)));
  assert(0 == fixedpt_toint(fixedpt_floor(d)));
  assert(-1 == fixedpt_toint(fixedpt_floor(e)));
  assert(-3 == fixedpt_toint(fixedpt_floor(f)));
  assert(-4 == fixedpt_toint(fixedpt_floor(g)));

  assert(1 == fixedpt_toint(fixedpt_ceil(a)));
  assert(3 == fixedpt_toint(fixedpt_ceil(b)));
  assert(1 == fixedpt_toint(fixedpt_ceil(c)));
  assert(0 == fixedpt_toint(fixedpt_ceil(d)));
  assert(-1 == fixedpt_toint(fixedpt_ceil(e)));
  assert(-2 == fixedpt_toint(fixedpt_ceil(f)));
  assert(-3 == fixedpt_toint(fixedpt_ceil(g)));

  float f1 = 5.5;
  float f2 = -5.5;

  printf("%d, %d\n", fixedpt_rconst(5.5), fixedpt_fromfloat(&f1));
  printf("%d, %d\n", fixedpt_rconst(-5.5), fixedpt_fromfloat(&f2));

  printf("pass!!!\n");

  return 0;
}