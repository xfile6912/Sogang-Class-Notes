#include <stdio.h>
#include <stdlib.h>
#include "threads/thread.h"
#include "threads/palloc.h"
#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"
#include "threads/vaddr.h"

void frame_init()
{
	list_init(&frame_table);
}
struct frame_entry* frame_insert(void *vaddr, void *paddr, bool writable)
{
	struct frame_entry *frame=(struct frame_entry*)malloc(sizeof(struct frame_entry));
	struct thread* cur=thread_current();
	//struct hash *page_table=&(cur->page_table);
	//struct page_entry *page=get_page_entry(page_table, vaddr);
	frame->vaddr=pg_round_down(vaddr);
	frame->paddr=pg_round_down(paddr);
	frame->frame_thread=cur;
	//frame->page_entry=page;
	if(paddr!=NULL)
	{
		list_push_back(&frame_table, &frame->elem);
		return frame;
	}

	paddr=palloc_get_page(PAL_USER);
	if(paddr==NULL)
	{
		evict_frame();
		paddr=palloc_get_page(PAL_USER);
	}
	frame->paddr=pg_round_down(paddr);
	list_push_back(&frame_table, &frame->elem);


	return frame;

}
bool frame_delete(struct frame_entry * frame)
{
	list_remove(&frame->elem);
}
struct frame_entry* get_frame_entry(void *paddr)
{
	struct list_elem *elePtr;
	struct frame_entry *frame;
	for(elePtr=list_begin(&frame_table); elePtr!=list_end(&frame_table); elePtr=list_next(elePtr))
	{
		frame=list_entry(elePtr, struct frame_entry, elem);
		if(pg_round_down(paddr)==frame->paddr)
		{
			return frame;
		}
	}
	return NULL;
}
bool evict_frame()
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

}

