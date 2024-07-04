/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/


#include "watchpoint.h"

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp() {
  Assert(free_ != NULL, "Watchpoint pool is full.");
  WP *temp = free_;
  free_ = free_->next;
  temp->next = head;
  head = temp;
  return temp;
}

bool free_wp(int id) {
  Assert(head != NULL, "Watchpoint pool is empty.");
  WP temp, *pre = &temp;
  pre->next = head;
  while(pre->next != NULL) {
    if(pre->next->NO == id) {
      WP *wp = pre->next;
      pre->next = pre->next->next;
      wp->next = free_;
      free_ = wp;
      head = temp.next;
      return true;
    }
    pre = pre->next;
  }
  Log("wrong id: %d.\n", id);
  return false;
}

void print_wp() {
  if(head == NULL) {
    printf("There is no watchpoint.\n");
  }
  else {
    WP *temp = head;
    printf("NO          Value       hex         expr\n");
    while(temp != NULL) {
      printf("%-12u%-12d0x%-10x%s\n", temp->NO, temp->pre_val, temp->pre_val, temp->expr);
      temp = temp->next;
    }
  }
}

bool detect_wp() {
  int is_hit = false;
  WP *temp = head;
  while(temp != NULL) {
    token_cpy(temp->tokens, &temp->token_num, true);
    word_t ret = eval(0, temp->token_num - 1);
    if(ret != temp->pre_val) {
      printf("Hit NO.%d watchpoint: \"%s\"\nold value is %u and 0x%x\nnew value is %u and 0x%x\n", temp->NO, temp->expr, temp->pre_val, temp->pre_val, ret, ret);
      temp->pre_val = ret;
      is_hit = true;
      break;
    }
    temp = temp->next;
  }
  return is_hit;
}


