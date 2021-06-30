/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"
#define SBUFSIZE 1024
typedef struct stock_item * stock_link;
typedef struct stock_item{
	int stock_id;
	int left_stock;
	int stock_price;
	int stock_readcnt;
	sem_t mutex;
	sem_t writer;
	stock_link left;
	stock_link right;
	stock_link next;//임시로 list만들 때 쓰임
}STOCK_ITEM;

int total_stock_num=0;//전체 주식종목 개수 저장
STOCK_ITEM * stock_head=NULL;
STOCK_ITEM * stock_tail=NULL;
STOCK_ITEM * root=NULL;

sem_t file_mutex;

typedef struct {
    int *buf;
	int n;
	int front;
	int rear;
	sem_t mutex; 
	sem_t slots;
	sem_t items;
} sbuf_t;
sbuf_t sbuf;

void sbuf_init(sbuf_t *sp, int n)
{
	sp->buf = Calloc(n, sizeof(int));
	sp->n = n;
	sp->front = sp->rear = 0;
 	Sem_init(&sp->mutex, 0, 1); 
 	Sem_init(&sp->slots, 0, n); 
	Sem_init(&sp->items, 0, 0);
}
void sbuf_deinit(sbuf_t *sp)
{
	Free(sp->buf);
}
void sbuf_insert(sbuf_t *sp, int item)
{
	P(&sp->slots);
	P(&sp->mutex);
	sp->buf[(++sp->rear)%(sp->n)] = item;
	V(&sp->mutex);
	V(&sp->items);
}
int sbuf_remove(sbuf_t *sp)
{
	int item;
	P(&sp->items);
	P(&sp->mutex);
	item = sp->buf[(++sp->front)%(sp->n)];
	V(&sp->mutex);
    V(&sp->slots);
    return item;
}
void free_tree(STOCK_ITEM *ptr)
{
	if(ptr)
	{
		free_tree(ptr->left);
		free_tree(ptr->right);
		free(ptr);
	}
}

void sig_int_handler(int sig)
{
	update_file();
	sbuf_deinit(&sbuf);
	free_tree(root);
	exit(0);
}


void stock_add_to_list(int stock_id, int left_stock, int stock_price)
{
	STOCK_ITEM *item=(STOCK_ITEM*)malloc(sizeof(STOCK_ITEM));
	item->stock_id=stock_id;
	item->left_stock=left_stock;
	item->stock_price=stock_price;
	total_stock_num++;
	if(stock_head==NULL)//fd list에 아무것도 없으면 head와 tail이 fd_item을 가리킴
	{
		stock_head=item;
		stock_tail=item;
	}
	else
	{
		stock_tail->next=item;
		stock_tail=item;
	}
}
int less(void *a, void *b)
{
	return (*(STOCK_ITEM*)a).stock_id - (*(STOCK_ITEM*)b).stock_id;
}

STOCK_ITEM* stock_arr_to_bst(STOCK_ITEM *stock_arr, int start, int end)
{
	if(start>end)//재귀를 벗어나는 조건
		return NULL;

	int mid=(start+end)/2;

	STOCK_ITEM *item=(STOCK_ITEM*)malloc(sizeof(STOCK_ITEM));
	item->stock_id=stock_arr[mid].stock_id;
	item->left_stock=stock_arr[mid].left_stock;
	item->stock_price=stock_arr[mid].stock_price;
	item->next=NULL;
	item->stock_readcnt=0;
	Sem_init(&item->mutex, 0, 1);
	Sem_init(&item->writer, 0, 1);

	item->left=stock_arr_to_bst(stock_arr, start, mid-1);
	item->right=stock_arr_to_bst(stock_arr, mid+1, end);

	return item;
}

void stock_list_to_bst()
{
	STOCK_ITEM *stock_arr=(STOCK_ITEM*)malloc(sizeof(STOCK_ITEM)*total_stock_num);
	STOCK_ITEM *ptr=stock_head;
	int i;
	for(i=0; i<total_stock_num; i++)
	{
		STOCK_ITEM* prev_ptr=ptr;
		stock_arr[i].stock_id=ptr->stock_id;
		stock_arr[i].left_stock=ptr->left_stock;
		stock_arr[i].stock_price=ptr->stock_price;
		ptr=ptr->next;
		free(prev_ptr);//array로 옮겨준 뒤 list는 하나씩 제거해줌
	}
	stock_head=NULL;
	stock_tail=NULL;
	//arr를 id 순서대로 sorting 해줌
	qsort(stock_arr, total_stock_num, sizeof(STOCK_ITEM), less);
	//arr를 기반으로 bst 만들어줌
	root=stock_arr_to_bst(stock_arr, 0, total_stock_num-1);

	free(stock_arr);//root(bst)를 만들었으니 array는 free해줌
}

void inorder(char *stocks, STOCK_ITEM *ptr)
{
	char temp[100];
	if(ptr)
	{
		inorder(stocks, ptr->left);

		P(&(ptr->mutex));
		ptr->stock_readcnt++;
		if(ptr->stock_readcnt==1)//한명이 주식을 읽고 있음
			P(&(ptr->writer));
		V(&(ptr->mutex));

		//Critical Section, Reading
		sprintf(temp, "%d %d %d	", ptr->stock_id, ptr->left_stock, ptr->stock_price);
		strcat(stocks, temp);
		//Critical Section, Reading

		P(&(ptr->mutex));
		ptr->stock_readcnt--;
		if(ptr->stock_readcnt==0)
			V(&(ptr->writer));
		V(&(ptr->mutex));

		inorder(stocks, ptr->right);
	}
}

void show(int fd)
{
	char stocks[MAXLINE];
	stocks[0]='\0';
	inorder(stocks, root);
	strcat(stocks, "\n");
	//printf("%s", stocks);
	Rio_writen(fd, stocks, strlen(stocks));
}
void buy(int fd, int stock_id, int stock_num)
{
	STOCK_ITEM * ptr=root;
	while(ptr)//binary search tree에서 stock_id 검색
	{
		if(ptr->stock_id==stock_id)
		{
			break;
		}
		else if(stock_id<ptr->stock_id)
		{
			ptr=ptr->left;
		}
		else
		{
			ptr=ptr->right;
		}
	}
	if(ptr==NULL)//stock_id가 존재 하지 않음
	{
		Rio_writen(fd, "stock_id not exists\n", strlen("stock_id not exists\n"));
	}
	else//stock_id가 존재하는 경우
	{

		P(&(ptr->writer));
		//Critical Section, Writing
		//
		if(ptr->left_stock>=stock_num)//잔여 수량이 남아 있는경우
		{
			ptr->left_stock-=stock_num;
			Rio_writen(fd, "[buy] success\n", strlen("[buy] success\n"));
		}
		else//잔여수량이 남아있지 않는 경우
		{
			Rio_writen(fd, "Not enough left stocks\n", strlen("Not enough left stocks\n"));
		}
		//
		//Critical Section, Writing
		V(&(ptr->writer));
	}
}

void sell(int fd, int stock_id, int stock_num)
{
	STOCK_ITEM * ptr=root;
	while(ptr)//binary search tree에서 stock_id 검색
	{
		if(ptr->stock_id==stock_id)
		{
			break;
		}
		else if(stock_id<ptr->stock_id)
		{
			ptr=ptr->left;
		}
		else
		{
			ptr=ptr->right;
		}
	}
	if(ptr==NULL)//stock_id가 존재 하지 않음
	{
		Rio_writen(fd, "stock_id not exists\n", strlen("stock_id not exists\n"));
	}
	else//stock_id가 존재하는 경우
	{
		P(&(ptr->writer));
		//Critical Section, Writing
		//
		ptr->left_stock+=stock_num;
		Rio_writen(fd, "[sell] success\n", strlen("[sell] success\n"));
		//
		//Critical Section, Writing
		V(&(ptr->writer));
	}
}

void load_stock_to_memory()
{
	FILE *fp=fopen("stock.txt", "r");
	int res;
	int stock_id;
	int left_stock;
	int stock_price;
	while(1)
	{
		res=fscanf(fp, "%d %d %d", &stock_id, &left_stock, &stock_price);
		if(res==EOF)
			break;
		stock_add_to_list(stock_id, left_stock, stock_price);//list에 임시로 저장
	}
	stock_list_to_bst();
	fclose(fp);
}

void inorder_print(STOCK_ITEM *ptr, FILE *fp)
{
	if(ptr)
	{
		inorder_print(ptr->left, fp);

		P(&(ptr->mutex));
		ptr->stock_readcnt++;
		if(ptr->stock_readcnt==1)//한명이 주식을 읽고 있음
			P(&(ptr->writer));
		V(&(ptr->mutex));
		//Critical Section, Reading

		fprintf(fp, "%d %d %d\n", ptr->stock_id, ptr->left_stock, ptr->stock_price);

		//Critical Section, Reading

		P(&(ptr->mutex));
		ptr->stock_readcnt--;
		if(ptr->stock_readcnt==0)
			V(&(ptr->writer));
		V(&(ptr->mutex));

		inorder_print(ptr->right, fp);
	}
}

void update_file()
{

	P(&file_mutex);
	//FILE WRITE, Critical Section
	FILE *fp=fopen("stock.txt", "w");
	inorder_print(root, fp);
	fclose(fp);
	//FILE WRITE, Critical Section
	V(&file_mutex);
}

void execute_command(int fd, char *command)
{
	char order[MAXLINE];//명령
	int stock_id;//주식 id(Char type)
	int stock_num;//주식 개수
	//Rio_writen(fd, command, strlen(command));
	sscanf(command, "%s %d %d", order, &stock_id, &stock_num);
	//printf("%s %d %d\n", order, stock_id, stock_num);
	if(!strcmp(order, "show"))
	{
		show(fd);
	}
	else if(!strcmp(order, "buy"))
	{
		buy(fd, stock_id, stock_num);
	}
	else if(!strcmp(order, "sell"))
	{
		sell(fd, stock_id, stock_num);
	}
	else
	{
		Rio_writen(fd, "invalid command\n", strlen("invalid command\n"));
	}
}

void *thread(void *vargp)
{
	int n;
	rio_t rio;
	char command[MAXLINE]; 
	Pthread_detach(pthread_self());
	while(1)
	{
		int connfd=sbuf_remove(&sbuf);
		while(1)
		{
			int i;
			for(i=0; i<MAXLINE; i++)
				command[i]='\0';
			Rio_readinitb(&rio, connfd);
			if((n = Rio_readlineb(&rio, command, MAXLINE)) != 0)
			{//명령이 들어온 경우
				printf("server received %d bytes\n", n);

				if(!strcmp(command, "exit\n"))
				{
					update_file();
					Rio_writen(connfd, "exit\n", strlen("exit\n"));
					Close(connfd);
					break;
				}
				else if(!strcmp(command, "\n"))
				{
					Rio_writen(connfd, "\n", strlen("\n"));
				}
				else
					execute_command(connfd, command);
			}
			else//client가 connection을 종료한 경우
			{
				update_file();
				Close(connfd);
				break;
			}
		}
	}
}

int main(int argc, char **argv) 
{
	int i, listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
	char client_hostname[MAXLINE], client_port[MAXLINE];


	pthread_t tid;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}
	//load stock to memory
	Signal(SIGINT, sig_int_handler);

	load_stock_to_memory();
	Sem_init(&file_mutex, 0, 1);

	listenfd = Open_listenfd(argv[1]);
	sbuf_init(&sbuf, SBUFSIZE);
	for(i=0; i<SBUFSIZE; i++)
		Pthread_create(&tid, NULL, thread, NULL);//처음에 여러개의 thread 만들어놈
	while (1) 
	{
		clientlen = sizeof(struct sockaddr_storage); 
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);//요청을 accept
		Getnameinfo((SA *) &clientaddr, clientlen,client_hostname, MAXLINE, client_port, MAXLINE, 0); 
		printf("Connected to (%s, %s)\n", client_hostname, client_port);
		sbuf_insert(&sbuf, connfd);
	}
}
/* $end echoserverimain */
