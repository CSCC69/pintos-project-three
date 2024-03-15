#include "devices/block.h"
#include "kernel/bitmap.h"
#include "kernel/hash.h"
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
  used_map = bitmap_create(block_size(block_get_role(BLOCK_SWAP)));
}

void
frame_evict_page(void)
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


  struct spt_entry entry_to_find = { .page = f->start_addr };
  struct hash_elem *elem = hash_find(&thread_current()->spt, &entry_to_find.elem);
  struct spt_entry *found = hash_entry(elem, struct spt_entry, elem);
  found->swap_slot = idx;

  palloc_free_page(f->start_addr);
  hash_delete(get_frame_table(), &f->elem);
  free(f);
}

void
frame_load_page(struct spt_entry *spt_entry)
{
}

struct frame*
frame_get_victim(void)
{
  struct hash frame_table = *get_frame_table();
  int frame_table_size = hash_size(&frame_table);
}
