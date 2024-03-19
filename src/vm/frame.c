#include "frame.h"
#include "debug.h"
#include "kernel/hash.h"
#include "stdio.h"
#include "swap.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include <stdlib.h>

struct hash frame_table;
struct list frame_list;

int mycount = 0;

struct hash *
get_frame_table (void)
{
  return &frame_table;
}

struct list *
get_frame_list (void)
{
  return &frame_list;
}

void
falloc_init (void)
{
  hash_init (&frame_table, frame_hash, frame_less, NULL);
  list_init (&frame_list);
}

void *
falloc_get_frame (enum palloc_flags flags, struct spt_entry *spt_entry)
{
  
  struct frame *f = malloc (sizeof (struct frame));
  lock_init (&f->frame_lock);
  lock_acquire (&f->frame_lock);
  void *page = NULL;

  while ((page = palloc_get_page (flags)) == NULL)
    swap_evict ();
  spt_entry->kpage = page;

  f->start_addr = page;
  f->spt_entry = spt_entry;
  hash_insert (&frame_table, &f->elem);
  list_push_back (&frame_list, &f->list_elem);

  lock_release (&frame_lock);
  return page;
}

void
falloc_free_frame (struct frame *f)
{
  lock_acquire (&f->frame_lock);
  if (f->spt_entry->owner->pagedir != (void *)0xcccccccc)
    {
      ASSERT (f->spt_entry->owner != NULL);
      pagedir_clear_page (f->spt_entry->owner->pagedir, f->spt_entry->upage);
    }

  if (f != NULL && f->spt_entry->kpage != NULL)
    palloc_free_page (f->spt_entry->kpage);
  if (f != NULL)
    free (f);
  lock_release (&f->frame_lock);
}

unsigned
frame_hash (const struct hash_elem *frame_elem, void *aux UNUSED)
{
  const struct frame *frame = hash_entry (frame_elem, struct frame, elem);
  return hash_int ((uint32_t)frame->start_addr);
}

bool
frame_less (const struct hash_elem *frame_elem_1,
            const struct hash_elem *frame_elem_2, void *aux UNUSED)
{
  return hash_entry (frame_elem_1, struct frame, elem)->start_addr
         < hash_entry (frame_elem_2, struct frame, elem)->start_addr;
}
