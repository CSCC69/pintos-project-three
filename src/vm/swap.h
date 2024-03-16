#include "vm/page.h"

void swap_init(void);
void swap_evict(void);
void swap_load(struct spt_entry *spt_entry);
struct frame* frame_get_victim(void);
