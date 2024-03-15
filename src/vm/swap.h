#include "vm/page.h"

void swap_init(void);
void frame_evict_page(void);
void frame_load_page(struct spt_entry *spt_entry);
struct frame* frame_get_victim(void);
