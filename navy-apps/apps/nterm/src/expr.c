#include <regex.h>
#include <assert.h>
// #include <stdint.h>
#include "expr.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))
#define NR_REGEX ARRLEN(rules)

typedef struct token {
  int type;
  char str[32];
} Token;

enum {
  TK_NOTYPE = 256,

  TK_PLUS, 
  TK_DEL, 
  TK_MUL, 
  TK_DIV, 

  TK_EQ, 
  TK_UNEQ, 
  TK_AND, 
  TK_OR, 
  TK_DEREF,
  TK_REF,

  TK_LPAREN,
  TK_RPAREN, 

  TK_NUM, 
  TK_HEX, 

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},                // spaces

  {"\\+", TK_PLUS},                 // plus
  {"-", TK_DEL},                    // delete
  {"\\*", TK_MUL},                  // multiple
  {"/", TK_DIV},                    // divide
  
  {"==", TK_EQ},                    // equal
  {"!=", TK_UNEQ},                  // unequal
  {"&&", TK_AND},                   // and
  {"\\|\\|", TK_OR},                // or
  
  {"&", TK_REF},

  {"\\(", TK_LPAREN},               // left parenthesis
  {"\\)", TK_RPAREN},               // right parenthesis

  {"0[x|X][0-9A-Fa-f]*", TK_HEX},   // hex num
  {"0|[1-9][0-9]*", TK_NUM},        // num
};

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      printf("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool is_nonoperator(Token token) {
  switch(token.type) {
    case TK_NUM:
    case TK_HEX:
    case TK_RPAREN: return true;
  }
  return false;
}

static bool is_unaryoperator(Token token) {
  switch(token.type) {
    case TK_DEREF:
    case TK_REF: return true;
  }
  return false;
}

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        printf("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        tokens[nr_token].type = rules[i].token_type;

        switch (rules[i].token_type) {
          case TK_NOTYPE: continue;
          case TK_NUM: {
            int idx = 0;
            if(nr_token > 0 && tokens[nr_token-1].type == TK_DEL && 
            (nr_token == 1 || !is_nonoperator(tokens[nr_token-2]))) {
              idx = 1;
              nr_token--;
              tokens[nr_token].type = TK_NUM;
              tokens[nr_token].str[0] = '-';
            }

            strncpy(tokens[nr_token].str + idx, substr_start, substr_len);
            tokens[nr_token].str[idx + substr_len] = '\0';
            break;
          }
          case TK_DEL: {
            if(nr_token > 0 && tokens[nr_token-1].type == TK_DEL && 
            !(nr_token > 1 && is_nonoperator(tokens[nr_token-2]))) {
              nr_token -= 2;
            }//合并连续的负号
            break;
          }
          case TK_HEX:{
            strncpy(tokens[nr_token].str, substr_start + 2, substr_len - 2);  //去除0x
            tokens[nr_token].str[substr_len - 2] = '\0';
            break;
          }
          case TK_MUL: {
            if(nr_token == 0 || !is_nonoperator(tokens[nr_token-1])) {
              tokens[nr_token].type = TK_DEREF;
            }
            break;
          }
        }

        nr_token++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t eval(int left, int right);
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  *success = true;
  return eval(0, nr_token - 1);
}


typedef struct {
	int *array;
	int capcity;
	int top;
}Stack;

Stack s;

bool stack_init(int size) {
	s.array = (int*)malloc(sizeof(int) * size);
	s.capcity = size;
	s.top = 0;
	if(s.array == NULL) {
		return false;
	}
	return true;
}

bool stack_push(int num) {
	if(s.top == s.capcity) {
		return false;
	}
	s.array[s.top++] = num;
	return true;
}

bool stack_pop() {
	if(s.top > 0) {
		s.top--;
		return true;
	}
	return false;
}

int stack_top(bool *success) {
	if(s.top > 0) {
		*success = true;
		return s.array[s.top];
	}
	else {
		*success = false;
		return 0;
	}
}

bool stack_isEmpty() {
	return s.top == 0;
}

void stack_exit() {
	free(s.array);
}

bool check_parentheses(int left, int right) {
  if(tokens[left].type != TK_LPAREN || tokens[right].type != TK_RPAREN) {
    return false;
  }
  stack_init((nr_token / 2) + 1);
  bool flag = true;
  for(int i = left; i <= right; i++) {
    if(tokens[i].type == TK_LPAREN) {
      stack_push(1);
    }
    else if(tokens[i].type == TK_RPAREN) {
      assert(!stack_isEmpty());
      stack_pop();
      if(i != right && stack_isEmpty()) {
        flag = false;
      }
    }
  }
  assert(stack_isEmpty());
  stack_exit();
  return flag;
}

int main_operator(int left, int right) {
  int main_op = -1;
  for (int i = left; i <= right; i++) {
    if(tokens[i].type == TK_LPAREN) {
      do {
        i++;
      } while (tokens[i].type != TK_RPAREN);
    }
    if(!is_nonoperator(tokens[i])) {
      if(main_op == -1) {
        main_op = i;
      }
      else {
        main_op = tokens[i].type < tokens[main_op].type ? i : main_op;
      }
    }
  }
  //printf("main_op:%d\n", tokens[main_op].type);
  return main_op;
}

uint32_t eval(int left, int right) {
  if(right < 30 && is_unaryoperator(tokens[right + 1])) {
    return 0;
  }
  assert(left <= right);
  if(left == right) {
    if(tokens[left].type == TK_NUM) {
      return atoi(tokens[left].str);
    }
    else if(tokens[left].type == TK_HEX) {
      char *ptr;
      return strtoul(tokens[left].str,&ptr,16);
    }
    else {
      assert(0);
    }
  }
  else if(check_parentheses(left, right)) {
    return eval(left + 1, right - 1);
  }
  else {
    int main_op = main_operator(left, right);
    uint32_t val1 = eval(left, main_op - 1), val2 = eval(main_op + 1, right), val3 = 0;
    switch(tokens[main_op].type) {
      case TK_PLUS: val3 = val1 + val2; break;
      case TK_DEL: val3 = val1 - val2; break;
      case TK_MUL: val3 =  val1 * val2; break;
      case TK_DIV: {
        assert(val2 != 0);
        val3 = val1 / val2;
        break;
      }

      // case TK_DEREF: val3 = paddr_read(val2, 4); break;
      case TK_REF: break;
      
      case TK_AND: val3 = (val1 && val2); break;
      case TK_OR: val3 = (val1 || val2); break;
      case TK_EQ: val3 = (val1 == val2); break;
      case TK_UNEQ: val3 = (val1 != val2); break;
      default: assert(0); break;
    }
    return val3;
  }
}

// void token_cpy(Token *wp_tks, int *wp_tk_num, bool wp2tokens) {
//   Token *src = wp_tks, *dst = tokens;
//   int num = *wp_tk_num;
//   if(!wp2tokens) {
//     src = tokens, dst = wp_tks;
//     num = nr_token;
//     *wp_tk_num = nr_token;
//   }
//   for (int i = 0; i < num; i++) {
//     dst[i] = src[i];
//   }
// }