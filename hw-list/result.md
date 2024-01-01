# hw1
## pthread
### Is the program’s output the same each time it is run? Why or why not?
- 每次输出均不同；因为每次为变量分配空间都是从堆上进行分配，这个过程是随机的
### Based on the program’s output, do multiple threads share the same stack?
- 不同的线程会修改地址中内容，如果多个线程共用一个栈进行读写可能造成不期望从出现的结果
### Based on the program’s output, do multiple threads have separate copies of global variables?
- 如果每个线程有一个全局变量的副本，不会修改全局变量，只会修改自己的副本
- 这种变量也被称为线程局部变量(TLS)
### Based on the program’s output, what is the value of void *threadid? How does this relate to the variable’s type (void *)?
- void *threadid就是线程id号
- 将Long类型强制转换为无符号数
### Using the first command line argument, create a large number of threads in pthread. Do all threads run before the program exits? Why or why not?
- 不一定，因为线程可能执行慢的情况下，主线程并没有使用pthread_join进行线程回收
- 线程各自使用pthread_exit退出，部分线程可能在主线程退出后才退出

## 多线程计算word count
- 需要初始化锁