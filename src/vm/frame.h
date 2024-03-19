#include "../threads/palloc.h"
#include "kernel/hash.h"
#include "stddef.h"

struct frame
{
  struct hash_elem elem;
  struct list_elem list_elem;
  void *start_addr;
  struct spt_entry *spt_entry;
  struct lock *frame_lock;
};

struct hash *get_frame_table (void);
struct list *get_frame_list (void);

void falloc_init (void);
void *falloc_get_frame (enum palloc_flags, struct spt_entry *spt_entry);
void falloc_free_frame (struct frame *f);

unsigned frame_hash (const struct hash_elem *frame_elem, void *aux);
bool frame_less (const struct hash_elem *frame_elem_1,
                 const struct hash_elem *frame_elem_2, void *aux);
