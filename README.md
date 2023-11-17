# lab4-traps
## Alarm
- 需要在proc中添加时钟中断的字段
- 需要保护中断时的寄存器，这里直接保存所有的寄存器，类似于swtch函数
~~~c
int alarm_interval;                  // Alarm interval 
void(*alarm_handler)();              // Alarm handler
int alarm_ticks;                     // left time
struct trapframe *alarm_trapframe;   // A copy of trapframe 
int alarm_enabled;                   // enabled alarm
~~~
- usertrap中加入对时钟中断的处理
  - 中断执行过程需要避免其他中断干扰 (enabled来判断)
  - interval为0直接出让CPU

~~~c
// give up the CPU if this is a timer interrupt.
if(which_dev == 2) {
  // 如果设置时钟中断间隔；进行倒计时，如果此时没有其他倒计时，就正常执行
  // 保存trapframe，等待sigreturn的恢复
  if(p->alarm_interval != 0) {
    if(--p->alarm_ticks <= 0) {
      if(p->alarm_enabled) {
        p->alarm_ticks = p->alarm_interval;
        p->alarm_enabled = 0;
        memmove(p->alarm_trapframe, p->trapframe, sizeof(struct trapframe));
        p->trapframe->epc = (uint64)p->alarm_handler;
      }
    }
  }
  // 出让CPU
  yield();
}
~~~
- sigreturn时需要返回此时的a0寄存器
  - syscall会将系统调用号赋值给a0寄存器，造成错误的改变
  - 这里需要保护现场
~~~c
int _sigreturn(void) {
  // 将 trapframe 恢复到时钟中断之前的状态，恢复原本正在执行的程序流
  struct proc *p = myproc();
  memmove(p->trapframe, p->alarm_trapframe, sizeof(struct trapframe));
  p->alarm_enabled = 1;
  // syscall会改变a0的值，所以返回p->frame->a0来保证a0不改变
 return p->trapframe->a0;
  // return 0;
}
~~~