#include "devices/block.h"
#include "kernel/bitmap.h"
#include "kernel/hash.h"
#include "swap.h"
#include "frame.h"
#include "debug.h"
#include "threads/vaddr.h"
struct bitmap *used_map;

void 
swap_init(void)
{
  used_map = bitmap_create(block_size(block_get_role(BLOCK_SWAP)));
}

struct frame*
frame_evict_page()
{
  struct frame *f = frame_get_victim();
  struct block *swap_disk = block_get_role(BLOCK_SWAP);

  size_t idx = bitmap_scan_and_flip(used_map, 0, 1, false);
  if (idx == BITMAP_ERROR)
    PANIC("Swap is full");

  void* page_start = f->start_addr;
  for (int i = 0; i < PGSIZE / BLOCK_SECTOR_SIZE; i++)
  {
    block_write(swap_disk, idx, page_start);
    page_start += BLOCK_SECTOR_SIZE;
  }

  return f;
}

/*void
frame_load_page(void *frame)
{
}*/

struct frame*
frame_get_victim(void)
{
  struct hash frame_table = get_frame_table();
  int frame_table_size = hash_size(&frame_table);
}
