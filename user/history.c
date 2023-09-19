#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "user/user.h"
// 只能显示上一条非history命令
int main(int argc, char **argv) {
  char *cnt = argv[1];
  int history_fd;
  char buf[512];
  int command_cnt = 0;
  int flag_cnt;

  while (*cnt != '\0') {
    int i = *cnt - '0';
    command_cnt = command_cnt * 10 + i;
    cnt++;
  }
  if ((history_fd = open(argv[1], O_RDWR)) < 0) {
    fprintf(2, "open failed\n");
    exit(1);
  }

  if (read(history_fd, buf, sizeof(buf)) < 0) {
    fprintf(2, "read failed\n");
    exit(1);
  }
  printf("%s\n", buf);

  exit(0);
}