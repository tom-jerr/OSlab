# lab6-Multithreading
## Uthread
- 内核并未实现malloc相关操作；定义的上下文结构因为是固定大小，不采用指针

~~~c
struct threadcontext {
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

struct thread {
  char       stack[STACK_SIZE];  /* the thread's stack */
  int        state;              /* FREE, RUNNING, RUNNABLE */
  struct threadcontext context; /* swtch() here to run thread */
};
~~~

- 线程初始化时，设置ra和sp
  - 线程切换后，直接返回到对应的ra

~~~c
memset(&t->context, 0, sizeof(t->context));
t->context.ra = (uint64)func;
t->context.sp = (uint64)t->stack + STACK_SIZE;
~~~

## 结果
~~~shell
== Test uthread == 
$ make qemu-gdb
uthread: OK (2.9s) 
== Test answers-thread.txt == 
answers-thread.txt: OK 
== Test ph_safe == make[1]: Entering directory '/home/lzy/OSWorkspace/xv6-labs-2023'
gcc -o ph -g -O2 -DSOL_THREAD -DLAB_THREAD notxv6/ph.c -pthread
make[1]: Leaving directory '/home/lzy/OSWorkspace/xv6-labs-2023'
ph_safe: OK (5.6s) 
== Test ph_fast == make[1]: Entering directory '/home/lzy/OSWorkspace/xv6-labs-2023'
make[1]: 'ph' is up to date.
make[1]: Leaving directory '/home/lzy/OSWorkspace/xv6-labs-2023'
ph_fast: OK (13.3s) 
== Test barrier == make[1]: Entering directory '/home/lzy/OSWorkspace/xv6-labs-2023'
gcc -o barrier -g -O2 -DSOL_THREAD -DLAB_THREAD notxv6/barrier.c -pthread
make[1]: Leaving directory '/home/lzy/OSWorkspace/xv6-labs-2023'
barrier: OK (3.3s) 
== Test time == 
time: OK 
Score: 60/60
~~~