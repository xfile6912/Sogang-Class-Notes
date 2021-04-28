#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <hash.h>

struct page_entry{
	void *vaddr;//virtual address
	void *paddr;//physical address
	int swap_idx;//-1이면 frame을 할당 받은것, >=0이면 swapslot에 있는ㄱ ㅓㅅ
	bool writable;	
	struct hash_elem elem;
};

bool page_less_func(const struct hash_elem *p1, const struct hash_elem *p2, void *aux);
unsigned page_hash_func(const struct hash_elem *p, void *aux);

void init_page_table(struct hash *page_table);
bool page_insert(void *vaddr, void *paddr, bool writable);
struct page_entry *get_page_entry(struct hash *page_table, void *vaddr);
void page_table_destroy(struct hash *page_table);
//bool evict_frame();
#endif
