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
  struct block *swap_disk = block_get_role(BLOCK_SWAP);

  size_t idx = bitmap_scan_and_flip(used_map, 0, 1, false);
  if (idx == BITMAP_ERROR)
    PANIC("Swap is full");

  void* page_start = f->start_addr;
  for (int i = 0; i < PGSIZE / BLOCK_SECTOR_SIZE; i++)
  {
    block_write(swap_disk, idx + i, page_start);
    page_start += BLOCK_SECTOR_SIZE;
  }


  // struct spt_entry entry_to_find = { .page = f->start_addr };
  // struct hash_elem *elem = hash_find(&thread_current()->spt, &entry_to_find.elem);
  // struct spt_entry *found = hash_entry(elem, struct spt_entry, elem);
  // found->swap_slot = idx;
  
  f->spt_entry->swap_slot = idx;

  palloc_free_page(f->start_addr);
  hash_delete(get_frame_table(), &f->elem);
  free(f);
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
    pagedir_set_page(thread_current()->pagedir, spt_entry->page, frame, true);
}

struct frame*
frame_get_victim(void)
{
  // struct hash frame_table = *get_frame_table();
  // int frame_table_size = hash_size(&frame_table);
  
  // int r = random_ulong() % frame_table_size;

  // struct hash_iterator i;
  // hash_first (&i, get_frame_table());
  // for (int j = 0; j < r - 1; j++)
  //   hash_next(&i);
  // struct frame *f = hash_entry(hash_cur(&i), struct frame, elem);
  // return f;

  struct list frame = *get_frame_list();
  return list_entry(list_pop_front(&frame), struct frame, list_elem);
  

  // struct hash_iterator i;
  // hash_first (&i, get_frame_table());
  // return hash_entry(hash_cur(&i), struct frame, elem);
}
