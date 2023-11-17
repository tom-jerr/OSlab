// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

// 物理页引用计数
struct {
  struct spinlock lock;
  int count[PHYSTOP / PGSIZE + 1];
} refcnt;

void increase_refcnt(void *pa)
{
  acquire(&refcnt.lock);
  refcnt.count[(uint64)pa / PGSIZE]++;
  release(&refcnt.lock);
}

void decrease_refcnt(void *pa)
{
  acquire(&refcnt.lock);
  refcnt.count[(uint64)pa / PGSIZE]--;
  release(&refcnt.lock);
}

int get_refcnt(void* pa)
{
  int cnt;
  acquire(&refcnt.lock);
  cnt = refcnt.count[(uint64)pa / PGSIZE];
  release(&refcnt.lock);
  return cnt;
}

// 初始化引用计数 = 1
void refcnt_init(void *pa) 
{
  acquire(&refcnt.lock);
  refcnt.count[(uint64)pa / PGSIZE] = 1;
  release(&refcnt.lock);
}

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&refcnt.lock, "refcnt");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    // 释放前，将引用计数置为1
    refcnt.count[(uint64)p / PGSIZE] = 1;
    kfree(p);
  }
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  // 引用计数减1，只有引用计数归零才进行释放内存
  acquire(&kmem.lock);
  decrease_refcnt(pa);
  if(get_refcnt(pa) > 0) {
    release(&kmem.lock);
    return;
  }
  release(&kmem.lock);

  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
  
  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r) {
    kmem.freelist = r->next;
    refcnt_init(r);
  }
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
