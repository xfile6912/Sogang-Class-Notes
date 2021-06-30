/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"

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

typedef struct fd_item * fd_link;
typedef struct fd_item{
	int fd;
	fd_link next;
}FD_ITEM;

FD_ITEM * fd_head=NULL;//현재 fd set에 관리항목으로 추가된 fd들의 list
FD_ITEM * fd_tail=NULL; 

void update_file();

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
	free_tree(root);
	exit(0);
}

void fd_add(int fd)
{
	FD_ITEM * fd_item=(FD_ITEM*)malloc(sizeof(FD_ITEM));
	fd_item->fd=fd;
	fd_item->next=NULL;

	if(fd_head==NULL)//fd list에 아무것도 없으면 head와 tail이 fd_item을 가리킴
	{
		fd_head=fd_item;
		fd_tail=fd_item;
	}
	else
	{
		fd_tail->next=fd_item;
		fd_tail=fd_item;
	}

}
void fd_delete(int fd)
{
	FD_ITEM *ptr;
	FD_ITEM *prev_ptr=fd_head;

	for(ptr=fd_head; ptr!=NULL; ptr=ptr->next)
	{
		if(ptr->fd==fd)//fd list에서 fd를 찾은 경우
		{
			break;
		}
		prev_ptr=ptr;
	}
	if(ptr==fd_head && ptr==fd_tail)//하나밖에 없었던 경우에 이제 fd_list는 비어있게 된다
	{
		fd_head=NULL;
		fd_tail=NULL;
		free(ptr);
	}
    else if(ptr!=fd_head && ptr!=fd_tail)//ptr이 fd list의 중간에 있는 경우
    {
		prev_ptr->next=ptr->next;//ptr의 이전 node가 ptr의 다음 node를 가리키도록함
		free(ptr);
    }
	else if(ptr==fd_head)//ptr이 fd_list의 head인 경우
	{
		fd_head=ptr->next;//fd list의 head가 ptr의 다음 node를 가리키도록 함
		free(ptr);
	}
	else if(ptr==fd_tail)//ptr이 fd의 tail인 경우
	{
		fd_tail=prev_ptr;//fd_list의 tail이 ptr의 이전 node를 가리키도록함;
		fd_tail->next=NULL;
		free(ptr);
	}
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
		//이 사이 thread시에 synchornizae//read
		sprintf(temp, "%d %d %d	", ptr->stock_id, ptr->left_stock, ptr->stock_price);
		strcat(stocks, temp);
		//이 사이 thread시에 synchronize//read
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
		//이 사이 thread 시에 synchronization 필요
		if(ptr->left_stock>=stock_num)//잔여 수량이 남아 있는경우
		{
			ptr->left_stock-=stock_num;
			Rio_writen(fd, "[buy] success\n", strlen("[buy] success\n"));
		}
		else//잔여수량이 남아있지 않는 경우
		{
			Rio_writen(fd, "Not enough left stocks\n", strlen("Not enough left stocks\n"));
		}
		//이 사이 thread 시에 synchronization 필요
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
		//이 사이 thread 시에 synchronization 필요
		ptr->left_stock+=stock_num;
		Rio_writen(fd, "[sell] success\n", strlen("[sell] success\n"));
		//이 사이 thread 시에 synchronization 필요
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
		//이 사이 thread시에 synchornizae//read
		fprintf(fp, "%d %d %d\n", ptr->stock_id, ptr->left_stock, ptr->stock_price);
		//이 사이 thread시에 synchronize//read
		inorder_print(ptr->right, fp);
	}
}

void update_file()
{
	//file에 대한 synchronization 필요//
	FILE *fp=fopen("stock.txt", "w");
	inorder_print(root, fp);
	fclose(fp);
	//file에 대한 synchronization 필요//
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

int main(int argc, char **argv) 
{
	int listenfd, connfd;
	fd_set watch_set, pending_set;
    int fd_max;
    FD_ITEM *ptr;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
	char client_hostname[MAXLINE], client_port[MAXLINE];
	char command[MAXLINE]; 
	rio_t rio;
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}
	//load stock to memory
	Signal(SIGINT, sig_int_handler);
	load_stock_to_memory();
	FD_ZERO(&watch_set);

	listenfd = Open_listenfd(argv[1]);
	fd_max=listenfd;
	FD_SET(listenfd, &watch_set);
    fd_add(listenfd);
	while (1) {
		pending_set=watch_set;
		Select(fd_max+1, &pending_set, NULL, NULL, NULL);

		for(ptr=fd_head; ptr!=NULL;)//관리하고 있는 fd_list를 돌면서
		{
            int fd=ptr->fd;
            FD_ITEM* next_ptr=ptr->next;
			if(FD_ISSET(fd, &pending_set) && fd!=listenfd)//fd가 set 되어 있고, access 요청이 아닌 경우
			{
				int n;
				Rio_readinitb(&rio, fd);
				command[0]='\0';
				if((n = Rio_readlineb(&rio, command, MAXLINE)) != 0) {//명령이 들어온 경우
					printf("server received %d bytes\n", n);
					if(!strcmp(command, "exit\n"))
					{
						update_file();
						Rio_writen(fd, "exit\n", strlen("exit\n"));
						FD_CLR(fd, &watch_set);//connfd를 watch set에서 없애줌
                        fd_delete(fd);//fd list에서 지워줌
						Close(fd);

					}
					else if(!strcmp(command, "\n"))
					{
						Rio_writen(fd, "\n", strlen("\n"));
					}
					else
						execute_command(fd, command);
					//printf("%s", command);
					//Rio_writen(fd, command, n);
				}
				else
				{
					update_file();
					FD_CLR(fd, &watch_set);//connfd를 watch set에 없애줌
                    fd_delete(fd);//fd list에서 지워줌
					Close(fd);
					//아무것도 들어오지 않은경우->connection이 종료된 경우
				}
			}
			else if(FD_ISSET(fd, &pending_set)){//fd가 set 되어 있고, access 요청인 경우
				clientlen = sizeof(struct sockaddr_storage); 
				connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);//요청을 accept
				Getnameinfo((SA *) &clientaddr, clientlen,client_hostname, MAXLINE, client_port, MAXLINE, 0); 
				printf("Connected to (%s, %s)\n", client_hostname, client_port);
				FD_SET(connfd, &watch_set);//connfd를 watch_set에 set해줌
                fd_add(connfd);//coonfd를 fd list에 추가해줌
				if(fd_max<connfd)
					fd_max=connfd;
			}
            ptr=next_ptr;
		}
	}
}
/* $end echoserverimain */
