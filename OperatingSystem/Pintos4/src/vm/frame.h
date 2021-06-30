#ifndef VM_FRAME_H
#define VM_FRAME_H
#include <list.h>
#include "vm/page.h"
struct frame_entry{
	void *paddr;
	//struct page_entry *page_entry;
	void *vaddr;
	struct thread *frame_thread;
	struct list_elem elem;
};


struct list frame_table;

void frame_init();
struct frame_entry *frame_insert(void *vaddr, void *paddr, bool writable);
bool frame_delete(struct frame_entry* frame);
struct frame_entry* get_frame_entry(void *paddr);
bool evict_frame();
#endif

