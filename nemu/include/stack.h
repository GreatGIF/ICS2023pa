/*************************************************************************
	> File Name: stack.h
	> Author: xh	
	> Mail: xh13242981739@163.com 
	> Created Time: 2024年07月03日 星期三 23时12分40秒
 ************************************************************************/

#include <stdbool.h>

typedef struct {
	int *array;
	int capcity;
	int top;
}Stack;

bool stack_init(int size);
bool stack_push(int num);
bool stack_isEmpty();
bool stack_pop();
int stack_top(bool *success);
void stack_exit();
