#include "filesys/off_t.h"
#include "kernel/hash.h"

struct spt_entry
  {
    void *upage;
    void *kpage;
    int swap_slot;
    struct executable_data *executable_data;
    struct hash_elem elem;
  };

struct executable_data
  {
    struct file *file;
    off_t ofs;
    uint8_t *upage;
    uint32_t read_bytes;
    uint32_t zero_bytes;
    bool writable;
  };
