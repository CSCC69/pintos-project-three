#include "filesys/off_t.h"
#include "kernel/hash.h"
#include "threads/thread.h"
#include "userprog/syscall.h"

struct spt_entry
  {
    void *upage;
    void *kpage;
    int swap_slot;
    struct executable_data *executable_data;
    struct mmap_data *mmap_data;
    struct hash_elem elem;
    struct thread *owner;
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

struct mmap_data
  {
    struct file *file;
    int id;
    void *addr;
  };

struct spt_entry *create_spt_entry(void *upage, void *kpage, int swap_slot, struct executable_data *executable_data, struct mmap_data *mmap_data, struct thread *owner);

struct executable_data *create_executable_data(struct file *file, off_t ofs, uint8_t *upage, uint32_t read_bytes, uint32_t zero_bytes, bool writable);

struct mmap_data *create_mmap_data(struct file *file, int id, void *addr);
