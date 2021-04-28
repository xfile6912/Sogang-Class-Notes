#include <stdio.h>
#include <stdlib.h>
#include <bitmap.h>
#include "threads/palloc.h"
#include "threads/thread.h"
//#include "vm/swap.h"
#include "threads/vaddr.h"
#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"
#include "devices/block.h"
const size_t BLOCKS_PER_PAGE=PGSIZE/BLOCK_SECTOR_SIZE;//한 페이지당 블락 수
struct block *swap_disk;
bool *swap_used;//swapdisk의 index위치에  현재 저장되어 있는지
void swap_init()
{
	swap_disk=block_get_role(BLOCK_SWAP);
	int block_num=block_size(swap_disk)/BLOCKS_PER_PAGE;//swap disk에 들어갈 수 있는 최대page수
	if(swap_disk)
	{
		swap_used=(bool*)malloc(sizeof(bool)*block_num);
		int i;
		for(i=0; i<block_num; i++)
		{
			swap_used[i]=false;//swap disk에 들어와있는 페이지 하나도 없음.
		}
	}

}
void swap_in(void *upage, void *kpage, int swap_idx)
{
	if(swap_used[swap_idx])
	{
		int i;
		int start=BLOCKS_PER_PAGE*swap_idx;
		for(i=0; i<BLOCKS_PER_PAGE; i++)
		{
			block_read(swap_disk, start+i, BLOCK_SECTOR_SIZE*i+kpage);
		}
		swap_used[swap_idx]=false;
		struct thread* cur=thread_current();
		struct hash *page_table=&(cur->page_table);
		struct page_entry *page=get_page_entry(page_table, upage);

		page->swap_idx=-1;
		pagedir_set_page(cur->pagedir, pg_round_down(upage), pg_round_down(kpage), page->writable);
	}
}
int swap_out(void *upage, void *kpage)
{
	int i;
	int idx;
	int block_num=block_size(swap_disk)/BLOCKS_PER_PAGE;
	for(idx=0; idx<block_num; idx++)
	{
		if(swap_used[idx]==false)
			break;
	}
	int start=BLOCKS_PER_PAGE*idx;
	for(i=0; i<BLOCKS_PER_PAGE;i++)
	{
		block_write(swap_disk, start+i, BLOCK_SECTOR_SIZE*i+kpage);
	}

	swap_used[idx]=true;

	return idx;
	//	struct frame_entry* frame= get_frame_entry(void *paddr)	
}
