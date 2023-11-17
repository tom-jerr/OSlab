# lab8 locks
## Memory allocator
- 为每个CPU都分配一个freelist和对应的lock
- 当前CPU窃取其他CPU的内存，如果可以直接窃取8页；进一步减少冲突