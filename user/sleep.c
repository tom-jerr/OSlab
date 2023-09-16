#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(2, "Usage: sleep seconds...\n");
    exit(1);
  }

  int seconds = atoi(argv[1]);
  if (sleep(seconds) < 0) {
    fprintf(2, "sleep failed\n");
    exit(2);
  }

  exit(0);
}
