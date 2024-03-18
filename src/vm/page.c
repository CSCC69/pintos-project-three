#include "page.h"
#include "threads/malloc.h"

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