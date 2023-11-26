# lab9 file system
## Large files
- 构造二级间接块，整个系统最大文件可以占有NDIRECT + NIDIRECT + NIDIRECT * NIDIRECT
- 修改bmap映射和itrunc释放inode

## Symlink 
- 添加sys_symlink系统调用
- 功能为创建symlink的inode，向inode中写入path
- open系统调用需要修改，如果以O_NOFOLLOW形式打开
  - 需要递归查找symlink对应的文件并打开
  - 递归查找设置深度
  
## Results
~~~shell
make[1]: Leaving directory '/home/lzy/OSWorkspace/xv6-labs-2023'
== Test running bigfile == 
$ make qemu-gdb
running bigfile: OK (84.4s) 
== Test running symlinktest == 
$ make qemu-gdb
(0.4s) 
== Test   symlinktest: symlinks == 
  symlinktest: symlinks: OK 
== Test   symlinktest: concurrent symlinks == 
  symlinktest: concurrent symlinks: OK 
== Test usertests == 
$ make qemu-gdb
usertests: OK (125.8s) 
== Test time == 
time: OK 
Score: 100/100
~~~