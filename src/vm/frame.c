#include "frame.h"
#include "kernel/hash.h"
#include "threads/palloc.h"
#include "debug.h"
#include <stdlib.h>
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include "swap.h"

struct hash frame_table;
struct list frame_list;

int mycount = 0;

struct hash*
get_frame_table(void)
{
  return &frame_table;
}

struct list*
get_frame_list(void)
{
  return &frame_list;
}

void
falloc_init (void)
{
  hash_init(&frame_table, frame_hash, frame_less, NULL);
  list_init(&frame_list);
}

void *
falloc_get_frame (enum palloc_flags flags, struct spt_entry *spt_entry)
{
  struct frame *f = malloc(sizeof(struct frame));
  void *page = NULL;

  int c = 0;
  while ((page = palloc_get_page(flags | PAL_USER)) == NULL)
  {
    // printf("swapping %d !!!!!!!! \n", c++);
    swap_evict();
    // printf("finished swappings\n");
  }
  // printf("found page %p\n", page);

  // printf("count %d\n", mycount++);
  f->start_addr = page;
  f->spt_entry = spt_entry;
  hash_insert(&frame_table, &f->elem);
  list_push_back(&frame_list, &f->list_elem);
  
  // printf("returning\n");
  return page;
}

void
falloc_free_frame (struct frame *f)
{
 if (f != NULL && f->spt_entry->kpage != NULL){
    palloc_free_page(f->spt_entry->kpage);
  } 
  if (f != NULL){
    free(f);   
  }
  pagedir_clear_page(f->spt_entry->owner->pagedir, f->spt_entry->upage);
}
// {
//   // printf("falloc_free_frame 0\n");
//   struct frame f_to_find = { .start_addr = page};
//   // printf("falloc_free_frame 0.5\n");
//   // f_to_find->start_addr = page;
//   // printf("falloc_free_frame 1\n");
//   struct frame *f = hash_entry(hash_find(&frame_table, &f_to_find.elem), struct frame, elem);
//   // printf("falloc_free_frame 2\n");
//  if (f != NULL && f->start_addr != NULL){
//     // printf("falloc_free_frame 3\n");
//     palloc_free_page(f->start_addr);
//   } 
//   // printf("falloc_free_frame 4\n");
//   free(f);
//   // printf("falloc_free_frame 5\n");
// }

unsigned frame_hash (const struct hash_elem *frame_elem, void *aux UNUSED)
{
  const struct frame *frame = hash_entry(frame_elem, struct frame, elem);
  return hash_int((uint32_t)frame->start_addr);
}

bool
frame_less (const struct hash_elem *frame_elem_1, const struct hash_elem *frame_elem_2, void *aux UNUSED)
{
  return hash_entry(frame_elem_1, struct frame, elem)->start_addr < hash_entry(frame_elem_2, struct frame, elem)->start_addr;
}
