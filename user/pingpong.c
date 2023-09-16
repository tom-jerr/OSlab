#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int chan[2];

  if (argc != 1) {
    fprintf(2, "Usage: pingpong\n");
    exit(2);
  }

  if (pipe(chan) < 0) {
    fprintf(2, "pipe failed\n");
    exit(2);
  }

  int pid = fork();
  if (pid == 0) {
    // child process
    char buf[5];
    if (read(chan[0], buf, 5) < 0) {
      fprintf(2, "read failed\n");
      exit(3);
    }
    printf("%d: received %s\n", getpid(), buf);

    write(chan[1], "pong", 5);
  } else if (pid > 0) {
    // parent process
    write(chan[1], "ping", 5);
    char buf[5];
    if (read(chan[0], buf, 5) < 0) {
      fprintf(2, "read failed\n");
      exit(3);
    }
    printf("%d: received %s\n", getpid(), buf);
  } else {
    fprintf(2, "fork failed\n");
    exit(4);
  }
  exit(0);
}