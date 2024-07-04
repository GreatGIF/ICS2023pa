typedef struct token {
  int type;
  char str[32];
} Token;

void token_cpy(Token *wp_tks, int *wp_tk_num, bool wp2tokens);
word_t eval(int left, int right);