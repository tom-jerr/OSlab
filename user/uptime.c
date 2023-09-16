#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int tricks = uptime();
  if (tricks < 0) {
    fprintf(2, "uptime failed\n");
    exit(1);
  }
  printf("%d\n", tricks);
  exit(0);
}