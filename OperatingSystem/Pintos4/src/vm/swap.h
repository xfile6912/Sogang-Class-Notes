#ifndef VM_SWAP_H
#define VM_SWAP_H

void swap_init();
void swap_in(void *upage, void *kpage, int swap_idx);//swap_idx로부터 frame으로 가져옴.
int swap_out(void *upage, void *kpage);//swap idx 반환

#endif
