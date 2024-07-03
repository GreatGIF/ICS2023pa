/*************************************************************************
	> File Name: stack.c
	> Author: xh	
	> Mail: xh13242981739@163.com 
	> Created Time: 2024年07月03日 星期三 23时19分25秒
 ************************************************************************/

#include <stack.h>
#include <stdlib.h>

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