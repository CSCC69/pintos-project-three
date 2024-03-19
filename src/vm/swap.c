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

  // if(pagedir_is_dirty(f->spt_entry->owner->pagedir, f->spt_entry->upage)) {
    if(f->spt_entry->mmap_data != NULL){
      //write to mem things 

    } else {
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
      f->spt_entry->swap_slot = idx;
    }
  // } 
  

  hash_delete(get_frame_table(), &f->elem);
  list_remove(&f->list_elem);
  falloc_free_frame(f);
}

void
swap_load(struct spt_entry *spt_entry)
{
  void *frame = falloc_get_frame(PAL_USER, spt_entry);
  spt_entry->kpage = frame;
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
    e = list_pop_front(frame);
    list_push_back(frame, e);
  }
  struct frame *f = list_entry(e, struct frame, list_elem);

  return f;
}
