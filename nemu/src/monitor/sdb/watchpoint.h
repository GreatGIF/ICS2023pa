#include "sdb.h"
#include "tokentype.h"
#include <detect_wp.h>

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  char expr[64];
  word_t pre_val;
  Token tokens[32];
  int token_num;
} WP;

WP *new_wp();
bool free_wp(int id);
void print_wp();
