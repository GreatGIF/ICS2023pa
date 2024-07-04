/*************************************************************************
	> File Name: tokentype.h
	> Author: xh	
	> Mail: xh13242981739@163.com 
	> Created Time: 2024年07月04日 星期四 23时48分36秒
 ************************************************************************/

typedef struct token {
  int type;
  char str[32];
} Token;

void token_cpy(Token *wp_tks, int *wp_tk_num, bool wp2tokens);
word_t eval(int left, int right);