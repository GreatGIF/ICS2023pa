#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>
// extern "C" {
//   #include "expr.h"
// }

#define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))
#define NR_CMD ARRLEN(cmd_table)

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

// static int cmd_help(char *args);
// static int cmd_echo(char *args);

// static struct {
//   const char *name;
//   const char *description;
//   int (*handler) (char *);
// } cmd_table [] = {
//   { "help", "Display information about all supported commands", cmd_help },
//   { "echo", "print to cmd", cmd_echo },
// };

// static int cmd_help(char *args) {
//   /* extract the first argument */
//   char *arg = strtok(NULL, " ");
//   int i;

//   if (arg == NULL) {
//     /* no argument given */
//     for (i = 0; i < NR_CMD; i ++) {
//       sh_printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
//     }
//   }
//   else {
//     for (i = 0; i < NR_CMD; i ++) {
//       if (strcmp(arg, cmd_table[i].name) == 0) {
//         printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
//         return 0;
//       }
//     }
//     printf("Unknown command '%s'\n", arg);
//   }
//   return 0;
// }

// // extern uint32_t expr(char *e, bool *success);
// static int cmd_echo(char *args) {
//   char *arg = strtok(NULL, "\n");
//   bool success;
//   printf("%s\n", arg);
//   // int ret = expr(args, &success);
//   // printf("%d", ret);
//   return 0;
// }

static void sh_handle_cmd(const char *cmd) {
  // char *temp = strtok((char *)cmd, " ");
  // if (temp == NULL) { return; }
  // int i;
  // for (i = 0; i < NR_CMD; i ++) {
  //   if (strcmp(cmd, cmd_table[i].name) == 0) {
  //     if (cmd_table[i].handler(temp) < 0) { return; }
  //     break;
  //   }
  // }

  // if (i == NR_CMD) { printf("Unknown command '%s'\n", temp); }
  char *temp = (char *)cmd;
  // while(*temp == ' ') {temp++;}
  int len = strlen(temp);
  temp[--len] = '\0';//remove '\n'
  
  int argc = 0;
  for(int i = 0; i < len; i++){
    if(temp[i] == ' ') {
      argc++;
    }
  }
  // printf("argc=%d\n", argc);

  char *filename = strtok(temp, " ");
  // printf("filename=%s\n", filename);

  char *argv[argc + 1];
  argv[argc] = NULL;

  if(argc != 0) {
    char *token;
    int i = 0;
    do {
      token = strtok(NULL, " ");
      argv[i++] = token;
      // printf("i=%d, token=%s\n", i,  argv[i-1]);
    } while (token != NULL);
  }

  // int len = strlen(cmd);
  // char *str = (char *)malloc(len);
  // strncpy(str, cmd, len - 1);
  // str[len - 1] = 0;
  // char *argv[] = {"--skip", NULL};
  execvp(filename, argv);
}

void builtin_sh_run() {
  sh_banner();
  if (setenv("PATH", "/bin:/usr/bin", 0) == 0) {
    sh_printf("PATH set to: %s\n", "/bin");
  } else {
    perror("setenv");
  }
  sh_prompt();
  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
