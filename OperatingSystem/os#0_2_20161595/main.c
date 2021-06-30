#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bitmap.h"
#include "debug.h"
#include "hash.h"
#include "hex_dump.h"
#include "limits.h"
#include "list.h"
#include "round.h"



unsigned hash_func(const struct hash_elem* e, void* aux)
{
	struct hash_item* itemPtr = hash_entry(e, struct hash_item, elem);
	return hash_int(itemPtr->data);

}
bool hash_less(const struct hash_elem* a,const struct hash_elem* b, void* aux)
{
	struct hash_item* itemPtr1 = hash_entry(a, struct hash_item, elem);
	struct hash_item* itemPtr2 = hash_entry(b, struct hash_item, elem);
	if (itemPtr1->data >= itemPtr2->data)
		return false;
	return true;
}
void hash_delete_func(struct hash_elem* e, void* aux)
{
	struct hash_item* itemPtr = hash_entry(e, struct hash_item, elem);
	free(itemPtr);
}
void hash_square_func(struct hash_elem* e, void* aux)
{
	struct hash_item* itemPtr = hash_entry(e, struct hash_item, elem);
	int data = itemPtr->data;
	itemPtr->data = data * data;
}
void hash_triple_func(struct hash_elem* e, void* aux)
{
	struct hash_item* itemPtr = hash_entry(e, struct hash_item, elem);
	int data = itemPtr->data;
	itemPtr->data = data * data*data;
}
void hash_clear_func(struct hash_elem* e, void* aux)
{
	struct hash_item* itemPtr = hash_entry(e, struct hash_item, elem);
	free(itemPtr);
}
bool list_less(const struct list_elem* a, const struct list_elem* b, void* aux)
{
	struct list_item* itemPtr1 = list_entry(a, struct list_item, elem);
	struct list_item* itemPtr2 = list_entry(b, struct list_item, elem);
	if (itemPtr1->data >= itemPtr2->data)
		return false;
	return true;
}
int main(void)
{
	struct list lists[10];
	struct hash hashs[10];
	struct bitmap* bitmaps[10];

	char mainOrder[25], dataType[10], name[10], func[7], name2[10];
	int op1, op2, op3;
	while (1)
	{
		scanf("%s", mainOrder);
		if (strcmp(mainOrder, "quit") == 0)
			break;
		else if (strcmp(mainOrder, "create") == 0)
		{
			scanf("%s%s", dataType, name);
			if (strcmp(dataType, "bitmap") == 0)
			{
				//mainOrder, dataType, name, op1
				scanf("%d", &op1);
				bitmaps[name[2] - '0'] = bitmap_create(op1);

			}
			else if (strcmp(dataType, "hashtable") == 0)
			{
				//mainOrder, dataType, name
				hash_init(&hashs[name[4] - '0'], hash_func, hash_less, NULL);
			}
			else if (strcmp(dataType, "list") == 0)
			{
				//mainOrder, dataType, name
				list_init(&lists[name[4] - '0']);

			}
		}
		else if (strcmp(mainOrder, "dumpdata") == 0)
		{
			scanf("%s", name);
			//mainOrder, name
			if (name[0] == 'b' && name[1] == 'm')
			{
				struct bitmap* bitmapAddress = bitmaps[name[2] - '0'];
				size_t i;
				int flag = 0;
				for (i = 0; i < bitmap_size(bitmapAddress); i++)
				{
					flag = 1;
					if (bitmap_test(bitmapAddress, i))
						printf("1");
					else
						printf("0");
				}
				if (flag == 1)
					printf("\n");
			}
			else if (name[0] == 'h' && name[1] == 'a' && name[2] == 's' && name[3] == 'h')
			{


				struct list_elem* elePtr;
				struct list_item* itemPtr;
				struct hash* hashAddress = &hashs[name[4] - '0'];
				int i, j;
				int flag = 0;//¿¿¿ ¿¿¿ ¿¿¿¿ ¿¿¿
				for (i = 0; i < hashAddress->bucket_cnt; i++)
				{
					struct list* bucket = &(hashAddress->buckets[i]);
					for (elePtr = list_begin(bucket); elePtr != list_end(bucket); elePtr = list_next(elePtr))
					{
						flag = 1;
						itemPtr = list_entry(elePtr, struct list_item, elem);
						printf("%d ", itemPtr->data);
					}
				}
				if (flag == 1)
				{
					printf("\n");
				}
			}
			else if (name[0] == 'l' && name[1] == 'i' && name[2] == 's' && name[3] == 't')
			{

				struct list_elem* elePtr;
				struct list_item* itemPtr;
				struct list* listAddress = &lists[name[4] - '0'];
				int flag = 0;//¿¿ ¿¿¿ ¿¿¿¿ ¿¿¿
				for (elePtr = list_begin(listAddress); elePtr != list_end(listAddress); elePtr = list_next(elePtr))
				{
					flag = 1;//¿¿¿ ¿¿
					itemPtr = list_entry(elePtr, struct list_item, elem);
					printf("%d ", itemPtr->data);
				}
				if (flag == 1)
				{
					printf("\n");
				}
			}
		}
		else if (strcmp(mainOrder, "delete") == 0)
		{
			scanf("%s", name);
			//mainOrder, name
			if (name[0] == 'b' && name[1] == 'm')
			{
				struct bitmap* bitmapAddress = bitmaps[name[2] - '0'];
				bitmap_destroy(bitmapAddress);
			}
			else if (name[0] == 'h' && name[1] == 'a' && name[2] == 's' && name[3] == 'h')
			{
				struct hash* hashAddress = &hashs[name[4] - '0'];
				hash_destroy(hashAddress, hash_delete_func);
			}
			else if (name[0] == 'l' && name[1] == 'i' && name[2] == 's' && name[3] == 't')
			{
				struct list* listAddress = &lists[name[4] - '0'];
				while (list_size(listAddress) != 0)
				{
					struct list_elem * elePtr=list_pop_front(listAddress);
					struct list_item* itemPtr = list_entry(elePtr, struct list_item, elem);
					free(itemPtr);
				}
			}
		}
		else if (strcmp(mainOrder, "bitmap_mark") == 0)
		{
			scanf("%s %d", name, &op1);
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			bitmap_mark(bitmapAddress, op1);
		}
		else if (strcmp(mainOrder, "bitmap_all") == 0)
		{
			scanf("%s %d %d", name, &op1, &op2);
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			if(bitmap_all(bitmapAddress, op1, op2))
				printf("true\n");
			else
				printf("false\n");
		}
		else if (strcmp(mainOrder, "bitmap_any") == 0)
		{
			scanf("%s %d %d", name, &op1, &op2);
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			if(bitmap_any(bitmapAddress, op1, op2))
				printf("true\n");
			else
				printf("false\n");

		}
		else if (strcmp(mainOrder, "bitmap_contains") == 0)
		{
			scanf("%s %d %d %s", name, &op1, &op2, func);
			bool value;
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			if(strcmp(func, "true")==0)
				value=true;
			else
				value=false;
			if(bitmap_contains(bitmapAddress, op1, op2, value))
				printf("true\n");
			else
				printf("false\n");
		}
		else if (strcmp(mainOrder, "bitmap_count") == 0)
		{
			scanf("%s %d %d %s", name, &op1, &op2, func);
			bool value; 
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			if(strcmp(func, "true")==0)
				value=true;
			else  
				value=false;
			printf("%zu\n", bitmap_count(bitmapAddress, op1, op2, value));
		}
		else if (strcmp(mainOrder, "bitmap_dump") == 0)
		{
			scanf("%s", name);
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			bitmap_dump(bitmapAddress);
		}
		else if (strcmp(mainOrder, "bitmap_expand") == 0)
		{
			scanf("%s %d", name, &op1);
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			bitmaps[name[2]-'0']=bitmap_expand(bitmapAddress,op1);
		}
		else if (strcmp(mainOrder, "bitmap_flip") == 0)
		{
			scanf("%s %d", name, &op1);
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			bitmap_flip(bitmapAddress, op1);
		}
		else if (strcmp(mainOrder, "bitmap_none") == 0)
		{
			scanf("%s %d %d", name, &op1, &op2);
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			if(bitmap_none(bitmapAddress, op1, op2))
				printf("true\n");
			else
				printf("false\n");

		}
		else if (strcmp(mainOrder, "bitmap_reset") == 0)
		{
			scanf("%s %d", name, &op1);
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			bitmap_reset(bitmapAddress, op1);
		}
		else if (strcmp(mainOrder, "bitmap_scan") == 0)
		{
			scanf("%s %d %d %s", name, &op1, &op2, func);
			struct bitmap* bitmapAddress=bitmaps[name[2]-'0'];
			bool value;
			if(strcmp(func, "true")==0)
				value=true;
			else
				value=false;
			printf("%zu\n", bitmap_scan(bitmapAddress, op1, op2, value));
		}
		else if (strcmp(mainOrder, "bitmap_scan_and_flip") == 0)
		{
			scanf("%s %d %d %s", name, &op1, &op2, func);
			struct bitmap* bitmapAddress=bitmaps[name[2]-'0'];
			bool value;
			if(strcmp(func, "true")==0)
				value=true;
			else
				value=false;
			printf("%zu\n", bitmap_scan_and_flip(bitmapAddress, op1, op2, value));
		}
		else if (strcmp(mainOrder, "bitmap_set") == 0)
		{
			scanf("%s %d %s", name, &op1, func);
			bool value;
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			if(strcmp(func, "true")==0)
				value=true;
			else  
				value=false;
			bitmap_set(bitmapAddress, op1, value);

		}
		else if (strcmp(mainOrder, "bitmap_set_all") == 0)
		{
			scanf("%s%s", name, func);
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			bool value;
			if(strcmp(func, "true")==0)
				value=true;
			else
				value=false;
			bitmap_set_all(bitmapAddress, value);

		}
		else if (strcmp(mainOrder, "bitmap_set_multiple") == 0)
		{
			scanf("%s %d %d %s", name, &op1, &op2, func);
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			bool value;
			if(strcmp(func, "true")==0)
				value=true;
			else
				value=false;
			bitmap_set_multiple(bitmapAddress, op1, op2, value);

		}
		else if (strcmp(mainOrder, "bitmap_size") == 0)
		{
			scanf("%s", name);
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			printf("%zu\n", bitmap_size(bitmapAddress));
		}
		else if (strcmp(mainOrder, "bitmap_test") == 0)
		{
			scanf("%s %d", name, &op1);
			struct bitmap* bitmapAddress= bitmaps[name[2]-'0'];
			if(bitmap_test(bitmapAddress, op1))
				printf("true\n");
			else
				printf("false\n");
		}
		//////////////////////////
		else if (strcmp(mainOrder, "hash_insert") == 0)
		{
			scanf("%s %d", name, &op1);
			struct hash* hashAddress = &hashs[name[4] - '0'];
			struct hash_item* temp = (struct hash_item*)malloc(sizeof(struct hash_item));
			temp->data = op1;
			hash_insert(hashAddress, &(temp->elem));
		}
		else if (strcmp(mainOrder, "hash_apply") == 0)
		{
			scanf("%s%s", name, func);
			if (strcmp(func, "square") == 0)
			{
				struct hash* hashAddress = &hashs[name[4] - '0'];
				hash_apply(hashAddress, hash_square_func);
			}
			else if (strcmp(func, "triple") == 0)
			{
				struct hash* hashAddress = &hashs[name[4] - '0'];
				hash_apply(hashAddress, hash_triple_func);
			}
		}
		else if (strcmp(mainOrder, "hash_delete") == 0)
		{
			scanf("%s %d", name, &op1);
			struct hash* hashAddress = &hashs[name[4] - '0'];
			struct hash_item temp;
			temp.data = op1;
			hash_delete(hashAddress, &(temp.elem));
		}
		else if (strcmp(mainOrder, "hash_empty") == 0)
		{
			scanf("%s", name);
			struct hash* hashAddress = &hashs[name[4] - '0'];
			if (hash_empty(hashAddress))
				printf("true\n");
			else
				printf("false\n");
		}
		else if (strcmp(mainOrder, "hash_size") == 0)
		{
			scanf("%s", name);
			struct hash* hashAddress = &hashs[name[4] - '0'];
			printf("%zu\n", hash_size(hashAddress));

		}
		else if (strcmp(mainOrder, "hash_clear") == 0)
		{
			scanf("%s", name);
			struct hash* hashAddress = &hashs[name[4] - '0'];
			hash_clear(hashAddress, hash_clear_func);
		}
		else if (strcmp(mainOrder, "hash_find") == 0)
		{
			scanf("%s %d", name, &op1);
			struct hash* hashAddress = &hashs[name[4] - '0'];
			struct hash_item temp;
			temp.data = op1;
			struct hash_elem* elePtr= hash_find(hashAddress, &(temp.elem));
			if (elePtr != NULL)
			{
				struct hash_item* itemPtr = hash_entry(elePtr, struct hash_item, elem);
				printf("%d\n", itemPtr->data);
			}
		}
		else if (strcmp(mainOrder, "hash_replace") == 0)
		{
			scanf("%s %d", name, &op1);
			struct hash* hashAddress = &hashs[name[4] - '0'];
			struct hash_item* temp = (struct hash_item*)malloc(sizeof(struct hash_item));
			temp->data = op1;
			hash_replace(hashAddress, &(temp->elem));
		}
		else if (strcmp(mainOrder, "list_insert") == 0)
		{
			scanf("%s %d %d", name, &op1, &op2);
			struct list_elem* elePtr;
			struct list_item* temp = (struct list_item*)malloc(sizeof(struct list_item));
			struct list* listAddress = &lists[name[4] - '0'];
			temp->data = op2;
			int count = 0;
			for (elePtr = list_begin(listAddress); count<op1; elePtr = list_next(elePtr))
			{
				count++;
			}
			list_insert(elePtr, &(temp->elem));
		}
		else if (strcmp(mainOrder, "list_push_back") == 0)
		{
			scanf("%s %d", name, &op1);
			struct list_item* temp = (struct list_item*)malloc(sizeof(struct list_item));
			struct list* listAddress = &lists[name[4] - '0'];
			temp->data = op1;
			list_push_back(listAddress, &(temp->elem));
		}
		else if (strcmp(mainOrder, "list_push_front") == 0)
		{
			scanf("%s %d", name, &op1);
			struct list_item* temp = (struct list_item*)malloc(sizeof(struct list_item));
			struct list* listAddress = &lists[name[4] - '0'];
			temp->data = op1;
			list_push_front(listAddress, &(temp->elem));
		}
		else if (strcmp(mainOrder, "list_front") == 0)
		{
			scanf("%s", name);
			struct list* listAddress = &lists[name[4] - '0'];
			struct list_elem* elePtr = list_front(listAddress);
			struct list_item* itemPtr=list_entry(elePtr, struct list_item, elem);
			printf("%d\n", itemPtr->data);
		}
		else if (strcmp(mainOrder, "list_back") == 0)
		{
			scanf("%s", name);
			struct list* listAddress = &lists[name[4] - '0'];
			struct list_elem* elePtr = list_back(listAddress);
			struct list_item* itemPtr = list_entry(elePtr, struct list_item, elem);
			printf("%d\n", itemPtr->data);
		}
		else if (strcmp(mainOrder, "list_pop_back") == 0)
		{
			scanf("%s", name);
			struct list* listAddress = &lists[name[4] - '0'];
			struct list_item* itemPtr = list_entry(list_pop_back(listAddress), struct list_item, elem);
			free(itemPtr);
		}
		else if (strcmp(mainOrder, "list_pop_front") == 0)
		{
			scanf("%s", name);
			struct list* listAddress = &lists[name[4] - '0'];
			struct list_item* itemPtr = list_entry(list_pop_front(listAddress), struct list_item, elem);
			free(itemPtr);
		}
		else if (strcmp(mainOrder, "list_insert_ordered") == 0)
		{
			scanf("%s %d", name, &op1);
			struct list_elem* elePtr;
			struct list_item* temp = (struct list_item*)malloc(sizeof(struct list_item));
			temp->data = op1;
			struct list* listAddress = &lists[name[4] - '0'];
			list_insert_ordered(listAddress, &(temp->elem), list_less,NULL);
		}
		else if (strcmp(mainOrder, "list_empty") == 0)
		{
			scanf("%s", name);
			struct list* listAddress = &lists[name[4] - '0'];
			if (list_empty(listAddress))
				printf("true\n");
			else
				printf("false\n");
		}
		else if (strcmp(mainOrder, "list_size") == 0)
		{
			scanf("%s", name);
			struct list* listAddress = &lists[name[4] - '0'];
			printf("%zu\n", list_size(listAddress));
		}
		else if (strcmp(mainOrder, "list_max") == 0)
		{
			scanf("%s", name);
			struct list* listAddress = &lists[name[4] - '0'];
			struct list_elem* elePtr=list_max(listAddress, list_less, NULL);
			struct list_item* itemPtr = list_entry(elePtr, struct list_item, elem);
			printf("%d\n", itemPtr->data);
		}
		else if (strcmp(mainOrder, "list_min") == 0)
		{
			scanf("%s", name);
			struct list* listAddress = &lists[name[4] - '0'];
			struct list_elem* elePtr = list_min(listAddress, list_less, NULL);
			struct list_item* itemPtr = list_entry(elePtr, struct list_item, elem);
			printf("%d\n", itemPtr->data);
		}
		else if (strcmp(mainOrder, "list_remove") == 0)
		{
			scanf("%s %d", name, &op1);
			struct list_elem* elePtr;
			struct list* listAddress = &lists[name[4] - '0'];
			int count = 0;
			for (elePtr = list_begin(listAddress); count < op1; elePtr = list_next(elePtr))
			{
				count++;
			}
			list_remove(elePtr);

			struct list_item* itemPtr = list_entry(elePtr, struct list_item, elem);
			free(itemPtr);
		}
		else if (strcmp(mainOrder, "list_reverse") == 0)
		{
			scanf("%s", name);
			struct list* listAddress = &lists[name[4] - '0'];
			list_reverse(listAddress);
		}
		else if (strcmp(mainOrder, "list_sort") == 0)
		{
			scanf("%s", name);
			struct list* listAddress = &lists[name[4] - '0'];
			list_sort(listAddress, list_less, NULL);
		}
		else if (strcmp(mainOrder, "list_splice") == 0)
		{
			scanf("%s %d %s %d %d", name, &op1, name2, &op2, &op3);
			struct list* listAddress1 = &lists[name[4] - '0'];
			struct list* listAddress2 = &lists[name2[4] - '0'];
			struct list_elem* before, *first, *last;
			int count = 0;
			for (before = list_begin(listAddress1); count < op1; before = list_next(before))
			{
				count++;
			}
			count = 0;
			for (first = list_begin(listAddress2); count < op2; first = list_next(first))
			{
				count++;
			}
			count = 0;
			for (last = list_begin(listAddress2); count < op3; last = list_next(last))
			{
				count++;
			}
			list_splice(before, first, last);
		}
		else if (strcmp(mainOrder, "list_swap") == 0)
		{
			scanf("%s %d %d", name, &op1, &op2);
			struct list* listAddress = &lists[name[4] - '0'];
			struct list_elem* elePtr1;
			struct list_elem* elePtr2;
			int count = 0;
			for (elePtr1 = list_begin(listAddress); count < op1; elePtr1 = list_next(elePtr1))
			{
				count++;
			}
			count = 0;
			for (elePtr2 = list_begin(listAddress); count < op2; elePtr2 = list_next(elePtr2))
			{
				count++;
			}
			list_swap(elePtr1, elePtr2);
			//TODO:
		}
		else if (strcmp(mainOrder, "list_shuffle") == 0)
		{
			scanf("%s", name);
			struct list* listAddress = &lists[name[4] - '0'];
			list_shuffle(listAddress);
			//TODO:
		}
		else if (strcmp(mainOrder, "list_unique") == 0)
		{
			char temp[100];
			scanf("%[^\n]", temp);
			if (1 == sscanf(temp, "%s %s", name, name2))
			{
				struct list* listAddress = &lists[name[4] - '0'];
				list_unique(listAddress, NULL, list_less, NULL);
			}
			else if(2 == sscanf(temp, "%s %s", name, name2))
			{
				struct list* listAddress1 = &lists[name[4] - '0'];
				struct list* listAddress2 = &lists[name2[4] - '0'];
				list_unique(listAddress1, listAddress2, list_less, NULL);
			}

		}
	}
}
