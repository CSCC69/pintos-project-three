void swap_init(void);
struct frame *frame_evict_page();
//void frame_load_page(struct spt_entry *spt_entry);
struct frame* frame_get_victim(void);
