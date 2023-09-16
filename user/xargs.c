#include "kernel/types.h"

#include "kernel/param.h"
#include "kernel/stat.h"

#include "user/user.h"

#define MAXLEN 128

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(2, "Usage: xargs [options] [command [initial-arguments]]\n");
    exit(1);
  }

  char *args[MAXARG];
  int args_cnt = 0;
  for (int i = 1; i < argc; i++) args[args_cnt++] = argv[i];
  for (int i = argc - 1; i < MAXARG; i++) args[i] = (char *)malloc(sizeof(MAXLEN));
  // 临时保存读入的字符
  char buf;
  int _read;

  // 换行就执行一次命令
  // 可能有多行命令, 用'\n'分隔
  while (1) {
    // 每一组参数中的位置
    int buf_len = 0;
    while ((_read = read(0, &buf, 1)) != 0) {
      if (buf == ' ') {
        args[args_cnt++][buf_len] = 0;
        buf_len = 0;
      } else if (buf == '\n') {
        args[args_cnt++][buf_len] = 0;
        buf_len = 0;
        break;
      } else {
        args[args_cnt][buf_len++] = buf;
      }
    }
    // 重新初始化j的位置
    // argv [argc] 始终为 NULL
    args[args_cnt] = 0;
    args_cnt = argc - 1;
    int pid;
    if ((pid = fork()) == 0) {
      exec(args[0], args);
    } else {
      wait(0);
    }

    if (_read <= 0) break;
  }
  exit(0);
}