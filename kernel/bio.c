// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKETS 13

struct {
  struct spinlock lock;
  struct buf head;
} buckets[NBUCKETS];

struct {
  struct spinlock lock;
  struct buf buf[NBUF];
} bcache;

extern uint ticks;

void
binit(void)
{
  struct buf *b;

  initlock(&bcache.lock, "bcache");

  for (int i = 0; i < NBUCKETS; i++){
    initlock(&buckets[i].lock, "bcache.bucket");
    buckets[i].head.prev = &buckets[i].head;
    buckets[i].head.next = &buckets[i].head;
  }
  
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    b->tick = -1;
    initsleeplock(&b->lock, "buffer");
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  struct buf *lru; // Potential LRU.
  int idx = blockno % NBUCKETS;
  acquire(&buckets[idx].lock);
  
  // Is the block already cached?
  for(b = buckets[idx].head.next; b != &buckets[idx].head; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->tick = ticks;
      b->refcnt++;
      release(&buckets[idx].lock);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Search in the bucket for LRU.
  lru = 0;
  for(b = buckets[idx].head.next; b != &buckets[idx].head; b = b->next)
    if(b->refcnt == 0 && (lru == 0 || lru->tick > b->tick))
      lru = b;
   
  if (lru){
    lru->dev = dev;
    lru->blockno = blockno;
    lru->valid = 0;
    lru->refcnt = 1;
    lru->tick = ticks;
    release(&buckets[idx].lock);
    acquiresleep(&lru->lock);
    return lru;
  }

  // Recycle the least recently used (LRU) unused buffer.
  int new_idx;

  for (b = bcache.buf; b < bcache.buf + NBUF; b++){
    if (b->refcnt == 0){ // With buckets[no].lock held and the previous for loop passed, it can be said that this buffer won't be in bucket buckets[no].
      // flag = 1;
      if (lru == 0 || lru->tick > b->tick)
        lru = b;
    }
  }

  if (lru){
    // this buffer is not in any bucket.
    if (lru->tick == -1) {
      acquire(&bcache.lock);
      if (lru->refcnt == 0){ // Lucky! The buffer is still available.
        lru->dev = dev;
        lru->blockno = blockno;
        lru->valid = 0;
        lru->refcnt = 1;
        lru->tick = ticks;

        lru->next = buckets[idx].head.next;
        lru->prev = &buckets[idx].head;

        buckets[idx].head.next->prev = lru;
        buckets[idx].head.next = lru;

        release(&bcache.lock);
        release(&buckets[idx].lock);
        acquiresleep(&lru->lock);
        return lru;

      } 
      // Unlucky! Go and search for another buffer.
      release(&bcache.lock);
      
    } else {  // So this buffer is in some buffer else.
      new_idx = (lru->blockno) % NBUCKETS;
      acquire(&buckets[new_idx].lock);
      if (lru->refcnt == 0){ // Lucky! The buffer is still available.
        lru->dev = dev;
        lru->blockno = blockno;
        lru->valid = 0;
        lru->refcnt = 1;
        lru->tick = ticks;
        
        // Remove from old bucket.
        lru->next->prev = lru->prev;
        lru->prev->next = lru->next;
        // Add to new bucket.
        lru->next = buckets[idx].head.next;
        lru->prev = &buckets[idx].head;
        buckets[idx].head.next->prev = lru;
        buckets[idx].head.next = lru;

        release(&buckets[new_idx].lock);
        release(&buckets[idx].lock);
        acquiresleep(&lru->lock);
        return lru;
      } 
      // Unlucky! Go and search for another buffer.
      release(&buckets[new_idx].lock);
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  int ou = (b->blockno)%NBUCKETS;

  acquire(&buckets[ou].lock);
  b->refcnt--;
  release(&buckets[ou].lock);
}

void
bpin(struct buf *b) { 
  int ou = (b->blockno)%NBUCKETS;

  acquire(&buckets[ou].lock);
  b->refcnt++;
  release(&buckets[ou].lock);
}

void
bunpin(struct buf *b) { 
  int ou = (b->blockno)%NBUCKETS;

  acquire(&buckets[ou].lock);
  b->refcnt--;
  release(&buckets[ou].lock);
}