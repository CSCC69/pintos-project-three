#include "page.h"
#include "threads/malloc.h"

struct hash mmap_table;

void 
mmap_table_init(void) {
  hash_init(&mmap_table, mmap_hash, mmap_less, NULL);
}

struct hash*
get_mmap_table(void) {
  return &mmap_table;
}

unsigned 
mmap_hash(const struct hash_elem *hash_elem, void *aux UNUSED) {
  const struct mmap_data *mmap_data = hash_entry(hash_elem, struct mmap_data, elem);
  return hash_int((uint32_t)mmap_data->addr);
}

bool
mmap_less(const struct hash_elem *elem1, const struct hash_elem *elem2, void *aux UNUSED) {
  return hash_entry(elem1, struct mmap_data, elem)->addr < hash_entry(elem2, struct mmap_data, elem)->addr; 
}

struct spt_entry *create_spt_entry(void *upage, void *kpage, int swap_slot, struct executable_data *executable_data, struct mmap_data *mmap_data, struct thread *owner){
    struct spt_entry *spt_entry = malloc(sizeof(struct spt_entry));
    if (spt_entry == NULL) {
        return NULL;
    }

    // Initialize the fields of the struct
    spt_entry->upage = upage;
    spt_entry->kpage = kpage;
    spt_entry->swap_slot = swap_slot;
    spt_entry->executable_data = executable_data;
    spt_entry->mmap_data = mmap_data;
    spt_entry->owner = owner;

    return spt_entry;
}

struct executable_data *create_executable_data(struct file *file, off_t ofs, uint8_t *upage, uint32_t read_bytes, uint32_t zero_bytes, bool writable){
    struct executable_data *data = malloc(sizeof(struct executable_data));
    if (data == NULL) {
        return NULL;
    }

    // Initialize the fields of the struct
    data->file = file;
    data->ofs = ofs;
    data->upage = upage;
    data->read_bytes = read_bytes;
    data->zero_bytes = zero_bytes;
    data->writable = writable;

    return data;
}

struct mmap_data *create_mmap_data(struct file *file, int id, void *addr){
    struct mmap_data *data = malloc(sizeof(struct mmap_data));
    if (data == NULL) {
        return NULL;
    }

    // Initialize the fields of the struct
    data->file = file;
    data->id = id;
    data->addr = addr;

    return data;
}
