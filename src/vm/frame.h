#include "kernel/hash.h"
#include "stddef.h"
#include "../threads/palloc.h"

struct frame
  {
    struct hash_elem elem;
    void *start_addr;
  };

struct hash get_frame_table(void);

void falloc_init (void);
void *falloc_get_frame (enum palloc_flags);
void falloc_free_frame (void *);

unsigned frame_hash (const struct hash_elem *frame_elem, void *aux);
bool frame_less (const struct hash_elem *frame_elem_1, const struct hash_elem *frame_elem_2, void *aux);
