#include "kernel/hash.h"

struct spt_entry
  {
    void *page;
    int swap_slot;
    struct hash_elem elem;
  };
