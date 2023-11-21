# lab8 locks
## Memory allocator
- 为每个CPU都分配一个freelist和对应的lock
- 当前CPU窃取其他CPU的内存，如果可以直接窃取多个页；进一步减少冲突
- 窃取页数steal_num, 设置为8, 不能通过kalloc test3; 这里设置为16
~~~c
struct {
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU]; // one per CPU
~~~
### kalloc
- 如果当前CPU的freelist为空，从其他CPU中窃取物理页
- 正常来进行kalloc

### kfree
- 使用cpuid获取当前CPU号
- 正常将空闲内存释放回对应的freelist
- 修改kernel/kalloc.c

## Buffer Cache
### 原始设计
- 一个循环双向链表维护所有的buffer；只有一把大锁
- 高并发下，瓶颈在于对锁的获取
~~~c
struct {
  struct buf head;
  struct spinlock lock;
  struct buf buf[NBUF];
} bcache;
~~~
### 改进设计
- 构造多个bucket，就是哈希链表来维护buffer
- 一个bcache循环双向链表维护整个buf
- 多个bucket维护各自的哈希双向链表
- 使用ticks来改变原来的LRU算法
![Alt text](image.png)
~~~c
struct {
  struct spinlock lock;
  struct buf head;
} buckets[NBUCKETS];

struct {
  struct spinlock lock;
  struct buf buf[NBUF];
} bcache;

extern uint ticks;
~~~
- 修改kernel/bio.c, buf.h
## 结果
~~~shell
== Test running kalloctest == 
$ make qemu-gdb
(48.7s) 
== Test   kalloctest: test1 == 
  kalloctest: test1: OK 
== Test   kalloctest: test2 == 
  kalloctest: test2: OK 
== Test   kalloctest: test3 == 
  kalloctest: test3: OK 
== Test kalloctest: sbrkmuch == 
$ make qemu-gdb
kalloctest: sbrkmuch: OK (4.9s) 
== Test running bcachetest == 
$ make qemu-gdb
(10.5s) 
== Test   bcachetest: test0 == 
  bcachetest: test0: OK 
== Test   bcachetest: test1 == 
  bcachetest: test1: OK 
== Test usertests == 
$ make qemu-gdb
usertests: OK (40.8s) 
== Test time == 
time: OK 
Score: 80/80
~~~