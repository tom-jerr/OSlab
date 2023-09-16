#include "kernel/types.h"
#include "user/user.h"

// 递归过程；父进程接收左边的数字，过滤后打印一个素数，将剩下的数传递到子进程
// 子进程递归上述过程
void pipeprime(int *fd) {
  close(fd[1]);
  int next_fd[2];
  int prime;

  pipe(next_fd);

  // 终止条件
  if (read(fd[0], &prime, 4) == 0) {
    exit(0);
  }

  printf("prime %d\n", prime);
  int child_pid = fork();
  if (child_pid == 0) {
    pipeprime(next_fd);
    // 正常退出
    // exit(0);
  } else if (child_pid > 0) {
    close(next_fd[0]);
    int num;
    while (read(fd[0], &num, 4) == 4) {
      if ((num % prime) != 0) {
        if (write(next_fd[1], &num, 4) != 4) {
          fprintf(2, "write failed");
          exit(4);
        }
      }
    }
    close(next_fd[1]);
    wait(0);
  } else {
    fprintf(2, "fork failed\n");
    exit(3);
  }
}
int main(int argc, char *argv[]) {
  int fd[2];
  if (pipe(fd) < 0) {
    fprintf(2, "pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid == 0) {
    pipeprime(fd);
    exit(0);
  } else if (pid > 0) {
    for (int i = 2; i <= 35; i++) {
      if (write(fd[1], &i, 4) != 4) {
        fprintf(2, "write failed");
        exit(4);
      }
    }
    close(fd[1]);
    wait(0);
    exit(0);
  } else {
    fprintf(2, "fork failed\n");
    exit(3);
  }
}