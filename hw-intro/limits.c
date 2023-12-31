#include <stdio.h>
#include <sys/resource.h>

int main() {
    struct rlimit lim_stack, lim_proc, lim_fd;
    getrlimit(RLIMIT_STACK, &lim_stack);
    getrlimit(RLIMIT_NPROC, &lim_proc);
    getrlimit(RLIMIT_NOFILE, &lim_fd);
    printf("stack size: %ld\n", lim_stack.rlim_cur);
    printf("process limit: %ld\n", lim_proc.rlim_cur);
    printf("max file descriptors: %ld\n", lim_fd.rlim_cur);
    return 0;
}
