#include <stdio.h>
#include <stdlib.h>
#include "threads/thread.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"
#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"

bool page_less_func(const struct hash_elem *p1, const struct hash_elem *p2, void *aux)
{
	struct page_entry* page_entry1 = hash_entry(p1, struct page_entry, elem);
	struct page_entry* page_entry2 = hash_entry(p2, struct page_entry, elem);
	if (page_entry1->vaddr >= page_entry2->vaddr)
		return false;
	return true;
}

unsigned page_hash_func(const struct hash_elem *p, void *aux)
{
	struct page_entry* page_entry = hash_entry(p, struct page_entry, elem);
	return hash_bytes(&page_entry->vaddr, sizeof(page_entry->vaddr));
}
void init_page_table(struct hash *page_table)
{
	hash_init(page_table, page_hash_func, page_less_func, NULL);
}
bool page_insert(void *vaddr, void *paddr, bool writable)
{
	struct page_entry *page=(struct page_entry*)malloc(sizeof(struct page_entry));
	page->vaddr=pg_round_down(vaddr);
	page->paddr=pg_round_down(paddr);
	page->swap_idx=-1;
	page->writable=writable;
	struct thread* cur=thread_current();
	struct hash *page_table=&(cur->page_table);
	if(!hash_find(page_table, &page->elem))
		hash_insert(page_table, &page->elem);
	else
	{
		//free(page);
		struct page_entry* ptr=page;
		page=hash_entry(hash_find(page_table, &page->elem), struct page_entry, elem);
		page->swap_idx=-1;
		free(ptr);
	}
	return true;
}
bool page_delete(struct hash *page_table, void *vaddr)
{
	struct thread* cur=thread_current();
	struct page_entry *page=get_page_entry(page_table, vaddr);
	if(page!=NULL)
	{
		hash_delete(page_table, &page->elem);
		return true;
	}
	else
	{
		return false;
	}
}
void page_delete_func(struct hash_elem* e, void* aux)
{
	struct page_entry* page = hash_entry(e, struct page_entry, elem);
	free(page);
}
struct page_entry* get_page_entry(struct hash *page_table, void *vaddr)
{
	struct page_entry page;
	page.vaddr=pg_round_down(vaddr);
	struct hash_elem *p=hash_find(page_table, &page.elem);
	if(p!=NULL)
		return hash_entry(p, struct page_entry, elem);
	else
		return NULL;

}
void page_table_destroy(struct hash *page_table)
{
	hash_destroy(page_table, page_delete_func);
}
/*bool evict_frame()
{
	struct list_elem *elePtr;
	struct frame_entry *victim_frame;
	elePtr=list_begin(&frame_table);
	victim_frame=list_entry(elePtr, struct frame_entry, elem);
	//victim->page_entry->swap_idx=swap_out(victim->page_entry->vaddr, victim->paddr);
	struct thread *victim_thread=victim_frame->frame_thread;
	struct thread *page_table=&victim_thread->page_table;
	struct page_entry * victim_page=get_page_entry(page_table, victim_frame->vaddr);
	victim_page->paddr=NULL;
	victim_page->swap_idx=swap_out(victim_frame->vaddr, victim_frame->paddr);
	pagedir_clear_page(victim_frame->frame_thread->pagedir, victim_frame->vaddr);
	palloc_free_page(victim_frame->paddr);
	list_remove(&victim_frame->elem);
	free(victim_frame);
	return true;

}*/
