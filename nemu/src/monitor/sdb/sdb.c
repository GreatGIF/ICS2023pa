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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <memory/paddr.h>
#include "watchpoint.h"
#include <difftest-def.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_si(char *args) {
  int steps = 1;
  args = strtok(NULL, " ");
  if (args != NULL) {
    sscanf(args, "%d", &steps);
  }
  if (steps < 0) {
    Log("There should be a positive number.");
    return 0;
  }
  cpu_exec(steps);
  return 0;
}

static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_info(char *args) {
  char *arg = strtok(NULL, "");
  if (arg == NULL) {
    printf("List of info subcommands:\nr : registers\nw : watchpoints\n");
  }
  else if(arg[0] == 'r') {
    isa_reg_display();
  }
  else if(arg[0] == 'w') {
    print_wp();
  }
  else {
    printf("Invalid command.\nList of info subcommands:\nr : registers\nw : watchpoints\n");
  }
  return 0;
}

static int cmd_x(char *args) {
  args = strtok(NULL, " ");
  if (args == NULL) {
    printf("Argument required (starting display address).\n");
    return 0;
  }
  char *expr = strtok(NULL, " ");
  int64_t len = 1;
  if (expr == NULL) {
    expr = args;
  }
  else {
    len = atol(args);
  }
  if (expr[0] == '0' && (expr[1] == 'x' || expr[1] == 'X')) {
    expr += 2;
  }
  else {
    printf ("Invalid number \"%s\".\n", expr);
    return 0;
  }

  paddr_t address = 0;
  for (int i = 0; i < strlen(expr); ++i) {
    address <<= 4;
    if (isdigit(expr[i])) {
      address += (expr[i] - '0');
    }
    else if (expr[i] >= 'a' && expr[i] <= 'f') {
      address += (expr[i] - 'a' + 10);
    }
    else if (expr[i] >= 'A' && expr[i] <= 'F') {
      address += (expr[i] - 'A' + 10);
    }
    else {
      printf ("Invalid number \"%s\".\n", expr-2);
      return 0;
    }
  }

  // 读取数据并打印
  int direct = len > 0 ? 4 : -4;    // 地址增加的方向
  len = len > 0 ? len : -len;
  for ( ; len > 0; --len) {
    word_t ret = paddr_read(address, 4);  // paddr_read已经做了地址合法性的检查
    printf(ANSI_FMT(FMT_WORD, ANSI_FG_BLUE), address);
    printf(": 0x%08x\n",ret);   // 4字节，16进制就有8个字符，右对其，高位补0
    address += direct;
  }
  return 0;
}

static int cmd_p(char *args) {
  char *e = strtok(NULL, "");
  bool success = true;
  word_t ret = expr(e, &success);
  if(!success) {
    printf("Error: In evaluation.\n");
    return 0;
  }
  else {
    printf("The value of \"%s\" is 0x%08x and %d.\n", e, ret, ret);
    return 0;
  }
}

static int cmd_w(char *args) {
  char *e = strtok(NULL, "");
  bool success = true;
  word_t ret = expr(e, &success);
  if(!success) {
    printf("Error: In evaluation.\n");
    return 0;
  }
  WP *temp = new_wp();
  temp->pre_val = ret;
  strcpy(temp->expr, e);
  token_cpy(temp->tokens, &temp->token_num, false);
  printf("The current value of No.%d watchpoint is %u and 0x%x.\n", temp->NO, temp->pre_val, temp->pre_val);
  return 0;
}

static int cmd_d(char *args) {
  char *e = strtok(NULL, "");
  bool success = true;
  word_t ret = expr(e, &success);
  if(!success) {
    printf("Error: In evaluation.\n");
    return 0;
  }
  if(free_wp(ret)) {
    printf("Succeed in delete NO.%d watchpoint.\n", ret);
  }
   else {
    printf("Failed in delete NO.%d watchpoint.\n", ret);
  }
  return 0;
}

static int cmd_detach(char *args) {
#undef _IS_DIFFTEST_MODE_
  printf("Exit DiffTest mode.\n");
  return 0;
}

static int cmd_attach(char *args) {
#ifndef _IS_DIFFTEST_MODE_
#define _IS_DIFFTEST_MODE_
#endif
  __EXPORT void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction);
  __EXPORT void difftest_regcpy(void *dut, bool direction);
  difftest_memcpy(0x100000 + CONFIG_MBASE, guest_to_host(0x100000 + CONFIG_MBASE), CONFIG_MSIZE - 0x100000, DIFFTEST_TO_REF);
  difftest_regcpy(&cpu, DIFFTEST_TO_REF);
  printf("Start DiffTest mode.\n");
  return 0;
}

static int cmd_save(char *args) {
  char *filename = strtok(NULL, "");
  FILE *fp = fopen(filename, "r+");
  if(fp == NULL) {
    printf("Wrong filename.\n");
    return 0;
  }
  fwrite(&cpu, sizeof(CPU_state), 1, fp);
  fwrite(guest_to_host(CONFIG_MBASE), CONFIG_MSIZE, 1, fp);
  fclose(fp);
  return 0;
}

static int cmd_load(char *args) {
  char *filename = strtok(NULL, "");
  FILE *fp = fopen(filename, "r+");
  if(fp == NULL) {
    printf("Wrong filename.\n");
    return 0;
  }
  int ret = fread(&cpu, sizeof(CPU_state), 1, fp);
  assert(ret == 1);
  ret = fread(guest_to_host(CONFIG_MBASE), CONFIG_MSIZE, 1, fp);
  assert(ret == 1);
  fclose(fp);
  return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Paused execution after executing some commands", cmd_si },
  { "info", "Print the program infomation", cmd_info },
  { "x", "Scan the memmory", cmd_x },
  { "p", "Calculate the value of expression", cmd_p},
  { "w", "Set the watchpoint", cmd_w},
  { "d", "Delete the watchpoint", cmd_d},
  { "detach", "Exit difftest mode", cmd_detach},
  { "attach", "Start difftest mode", cmd_attach},
  { "save", "Save snapshot", cmd_save},
  { "load", "Load snapshot", cmd_load},
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
