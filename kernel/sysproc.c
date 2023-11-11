#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


// #ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 uaddr;
  int plen;
  uint64 buf;

  // 获取参数
  argaddr(0, &uaddr);
  argint(1, &plen);
  argaddr(2, &buf);
  
  // 设置最大数量的页查询
  if(plen > 32) return -1;

  // 判断是否访问过页
  struct proc *p = myproc();
  // 内核中的mask
  uint64 bufmask = 0;
  for(uint64 i = 0; i < plen; ++i) {
    pte_t *pte;
    pte = walk(p->pagetable, uaddr + PGSIZE * i, 0);
    // vmprint(p->pagetable);  // 打印虚拟内存
    if(pte != 0 && ((*pte) & PTE_A)) {
      // buf中该页对应的位置为1；重新设置PTE_A为0
      bufmask |= (1 << i);
      *pte &= ~PTE_A;
    }
  }

  // 将buf拷贝到用户空间
  if(copyout(p->pagetable, buf, (char *)&bufmask, sizeof(buf)) < 0)
    return -1;

  return 0;
}
// #endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
