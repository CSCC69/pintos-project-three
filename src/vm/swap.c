#include "devices/block.h"
#include "kernel/bitmap.h"
#include "kernel/hash.h"
#include "random.h"
#include "swap.h"
#include "threads/thread.h"
#include "frame.h"
#include "debug.h"
#include "threads/vaddr.h"
#include "threads/malloc.h"
#include "userprog/pagedir.h"
#include "stdio.h"

struct bitmap *used_map = NULL;

void 
swap_init(void)
{
  if (block_get_role(BLOCK_SWAP) == NULL) 
    return;
  block_sector_t size = block_size(block_get_role(BLOCK_SWAP));
  used_map = bitmap_create(size);
}

void
swap_evict(void)
{
  struct frame *f = frame_get_victim();
  // printf("f->start_addr: %p\n", f->start_addr);
  // printf("hash size: %d\n", hash_size(get_frame_table()));
  struct block *swap_disk = block_get_role(BLOCK_SWAP);

  // printf("swap_evict\n");

  size_t idx = bitmap_scan_and_flip(used_map, 0, 1, false);
  if (idx == BITMAP_ERROR)
    PANIC("Swap is full");
  // printf("swap_evict 1\n");
  void* page_start = f->start_addr;
  for (int i = 0; i < PGSIZE / BLOCK_SECTOR_SIZE; i++)
  {
    // printf("swap_evict 2 %d\n", i);
    block_write(swap_disk, idx + i, page_start);
    page_start += BLOCK_SECTOR_SIZE;
  }
//  printf("swap_evict 3\n");
  f->spt_entry->swap_slot = idx;
//  printf("swap_evict 4\n");
  hash_delete(get_frame_table(), &f->elem);
  list_remove(&f->list_elem);
  printf("swap_evict 5\n");
  falloc_free_frame(f);
  // printf("swap_evict 6\n");
}

void
swap_load(struct spt_entry *spt_entry)
{
  void *frame = falloc_get_frame(PAL_USER, spt_entry);
    for (int i = 0; i < PGSIZE / BLOCK_SECTOR_SIZE; i++)
    {
      block_read(block_get_role(BLOCK_SWAP), spt_entry->swap_slot + i, frame);
      frame += BLOCK_SECTOR_SIZE;
    }
    pagedir_set_page(thread_current()->pagedir, spt_entry->upage, frame, true);
}

struct frame*
frame_get_victim(void)
{
  // Randomly select a frame to evict
  struct list *frame = get_frame_list();
  int size = list_size(frame);
  int r = random_ulong() % size;
  struct list_elem *e;
  for(int i = 0; i < r || list_entry(e, struct frame, list_elem)->spt_entry->kpage == NULL; i++)
  {
    // printf("i: %d\n", i);
    e = list_pop_front(frame);
    list_push_back(frame, e);
  }
  struct frame *f = list_entry(e, struct frame, list_elem);

  return f;


  // struct hash frame_table = *get_frame_table();
  // int frame_table_size = hash_size(&frame_table);
  
  // int r = random_ulong() % frame_table_size;

  // struct hash_iterator i;
  // hash_first (&i, get_frame_table());
  // for (int j = 0; j < r - 1; j++)
  //   hash_next(&i);
  // struct frame *f = hash_entry(hash_cur(&i), struct frame, elem);

  // if(f->spt_entry->kpage == NULL || f->start_addr == NULL)
  //   return frame_get_victim();

  // return f;

  // struct list frame = *get_frame_list();
  // int size = list_size(&frame);
  // int r = random_ulong() % size;
  // return list_entry(list_begin(&frame), struct frame, list_elem);
  

  // struct hash_iterator i;
  // hash_first (&i, get_frame_table());
  // return hash_entry(hash_cur(&i), struct frame, elem);
}
