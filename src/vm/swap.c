#include "swap.h"
#include "debug.h"
#include "devices/block.h"
#include "frame.h"
#include "kernel/bitmap.h"
#include "kernel/hash.h"
#include "random.h"
#include "stdio.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"

struct bitmap *used_map = NULL;

/* Initializes the swap bitmap */
void
swap_init (void)
{
  if (block_get_role (BLOCK_SWAP) == NULL)
    return;
  block_sector_t size = block_size (block_get_role (BLOCK_SWAP));
  used_map = bitmap_create (size);
}

/* Finds an allocated frame and evicts it from memory into swap or disk */
void
swap_evict (void)
{
  struct frame *f = frame_get_victim ();

  if (pagedir_is_dirty (f->spt_entry->owner->pagedir, f->spt_entry->upage))
    {
      if (f->spt_entry->mmap_data != NULL)
        {
          // We would write back to file if we could load the file properly
        }
      else
        {
          struct block *swap_disk = block_get_role (BLOCK_SWAP);

          size_t idx = bitmap_scan_and_flip (used_map, 0, 1, false);
          if (idx == BITMAP_ERROR)
            PANIC ("Swap is full");
          void *page_start = f->start_addr;
          for (int i = 0; i < PGSIZE / BLOCK_SECTOR_SIZE; i++)
            {
              block_write (swap_disk, idx + i, page_start);
              page_start += BLOCK_SECTOR_SIZE;
            }
          f->spt_entry->swap_slot = idx;
        }
    }

  hash_delete (get_frame_table (), &f->elem);
  list_remove (&f->list_elem);
  falloc_free_frame (f);
}

/* Loads a page from swap into a newly allocated frame */
void
swap_load (struct spt_entry *spt_entry)
{
  void *frame = falloc_get_frame (PAL_USER, spt_entry);
  spt_entry->kpage = frame;
  for (int i = 0; i < PGSIZE / BLOCK_SECTOR_SIZE; i++)
    {
      block_read (block_get_role (BLOCK_SWAP), spt_entry->swap_slot + i,
                  frame);
      frame += BLOCK_SECTOR_SIZE;
    }
  pagedir_set_page (thread_current ()->pagedir, spt_entry->upage, frame, true);
}

/* Finds a frame to evict based on the second-chance algorithm */
struct frame *
frame_get_victim (void)
{
  struct list *frame = get_frame_list();
  int size = list_size(frame);

  struct list_elem *e = list_pop_front(frame);
  struct frame *f = list_entry(e, struct frame, list_elem);
  while(pagedir_is_accessed(f->spt_entry->owner, f->spt_entry->upage))
    {
      pagedir_set_accessed(f->spt_entry->owner, f->spt_entry->upage, false);
      list_push_back(frame, e);

      e = list_pop_front(frame);
      f = list_entry(e, struct frame, list_elem);
    }

  return f;
}
