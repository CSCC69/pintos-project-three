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
    int read_bytes;
    int zero_bytes;
    off_t ofs;
    int remaining_page_count;
    struct hash_elem elem;
  };

void mmap_table_init(void);
unsigned mmap_hash(const struct hash_elem *hash_elem, void *aux UNUSED);
bool mmap_less(const struct hash_elem *elem1, const struct hash_elem *elem2, void *aux UNUSED);
struct hash* get_mmap_table(void);

struct spt_entry *create_spt_entry(void *upage, void *kpage, int swap_slot, struct executable_data *executable_data, struct mmap_data *mmap_data, struct thread *owner);

struct executable_data *create_executable_data(struct file *file, off_t ofs, uint8_t *upage, uint32_t read_bytes, uint32_t zero_bytes, bool writable);

struct mmap_data *create_mmap_data(struct file *file, int id, void *addr, int read_bytes, int zero_bytes, off_t ofs, int remaining_pages);
