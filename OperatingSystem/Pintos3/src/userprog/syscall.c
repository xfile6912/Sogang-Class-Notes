#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "filesys/filesys.h"
#include "threads/synch.h"
struct semaphore wrt;
struct semaphore mutex;
int readcount;
static void syscall_handler (struct intr_frame *);
//halt, exit, exec, wait, read, write
//fibonacci, max_of_four_int
void checkPointer(void *vaddr)//to protecting invalid memory accesses from system call.
{
	struct thread *cur=thread_current();
	if(!is_user_vaddr(vaddr))//user 영역이 아니라면 잘못된 접근
	{
		exit(-1);
	}
	if(pagedir_get_page(cur->pagedir,vaddr)==NULL)
	{
		exit(-1);
	}

}
void checkFile(const char* file)
{
	if(file==NULL)
		exit(-1);
}
void checkFd(int fd)
{
	struct thread *cur=thread_current();
	if(cur->files[fd]==NULL)
		exit(-1);
}
void halt(void)
{
	//	printf("halt\n");
	shutdown_power_off();
}
void closeAll()//해당 thread가 연 모든 파일을 close해주는 함수
{
	struct thread * cur= thread_current();
	int i;
	for(i=2; i<cur->fd; i++)
	{
		if(cur->files[i]==NULL)
			continue;
		close(i);
	}

}
void waitAll()//모든 자식 프로세스들을 process wait해주는 함수
{
	struct thread * cur= thread_current();
	struct list *childList=&cur->childList;
	struct thread *children;
	struct list_elem *elePtr;
	for(elePtr=list_begin(childList); elePtr!=list_end(childList); elePtr=list_next(elePtr))
	{
		children=list_entry(elePtr, struct thread, current);
		process_wait(children->tid);
	}//부모는 종료될때 모든 자식프로세스들이 다 죽을 때까지 기다려야함.*/
}
void exit(int status)
{
	//	printf("exit\n");
	struct thread * cur= thread_current();
	int i;
	closeAll();//thread cur가 연 모든 파일을 close해줌.
	waitAll();//부모는 자식 프로세스가 다 끝날때까지 기다려야함.
	printf("%s: exit(%d)\n", cur->name, status);
	cur->exitStatus=status;//exitStatus를 저장해줌.
	//	cur->exitFlag=1;//부모에게 끝난 것을 처리해줘도 된다는 신호보냄.
	thread_exit();
}
tid_t exec(const char *file)
{
	//	printf("exec\n");
	return process_execute(file);
}
int wait (tid_t tid)
{
	//	printf("wait\n");
	return process_wait(tid);
}
int read(int fd, void *buffer, unsigned size)
{
	int i;
	//	printf("read\n");
	int value;
	sema_down(&mutex);
	readcount++;
	if(readcount==1)
		sema_down(&wrt);
	sema_up(&mutex);
	////////////file read/////////////
	if(fd==0)
	{
		for(i=0; i<size; i++)
		{
			((char*)buffer)[i]=input_getc();
			if(((char*)buffer)[i]=='\0')break;
		}
		if(i!=size)
		{
			value=-1;
		}
		else
		{
			value=size;
		}
	}
	else
	{
		struct thread *cur=thread_current();
		value= file_read(cur->files[fd], buffer, size);
	}
	///////////file read///////////////
	sema_down(&mutex);
	readcount--;
	if(readcount==0)
		sema_up(&wrt);
	sema_up(&mutex);
	return value;

}
int write(int fd, const void *buffer, unsigned size)
{
	//	printf("write\n");
	int value;
	sema_down(&wrt);
	//////////////write/////////////
	if(fd==1)
	{
		//		input_putc(fd);
		putbuf(buffer, size);
		value=size;
	}
	else
	{
		struct thread *cur=thread_current();
		value=file_write(cur->files[fd], buffer, size);
	}
	/////////////write////////////////
	sema_up(&wrt);
	return value;
}
int fibonacci(int n)
{
	//	printf("fibonacci\n");
	int i;
	int fn_1=1;
	int fn_2=1;
	int fn=0;
	//	printf("%d\n", n);
	if(n==1 || n==2)
		fn=1;
	else
	{
		for(i=3; i<=n; i++)
			/* Open executable file. */
		{
			fn=fn_1+fn_2;
			fn_2=fn_1;
			fn_1=fn;
		}
	}
	return fn;
}
int max_of_four_int(int a, int b, int c, int d)
{
	//	printf("max_of_four_int\n");
	int maxValue=a;
	if(b>maxValue)
		maxValue=b;
	if(c>maxValue)
		maxValue=c;
	if(d>maxValue)
		maxValue=d;
	return maxValue;
}

bool create(const char* file, unsigned initial_size)
{
	//create a newfile called file initially initial_size bytes in size
	//return true if successful else false
	return filesys_create(file, initial_size);
}
bool remove(const char *file)
{
	//deletes the file called file
	//return true if successful else false
	return filesys_remove(file);
}
int open(const char *file)
{
	//opens the file called file
	//return a nonnegative integer handled called a fd
	//return -1 if the file could not be opened
	sema_down(&mutex);
	readcount++;
	if(readcount==1)
		sema_down(&wrt);
	sema_up(&mutex);
	///////////////open////////////////
	struct file* openfile=filesys_open(file);//open file
	int value;
	if(openfile==NULL)
	{
		value=-1;
	}
	else
	{
		struct thread *cur=thread_current();
		cur->files[cur->fd]=openfile;//비어있는 fd번호에 openfile추가
		if(strcmp(file,cur->name)==0)//to prevent write excutable of running thread
		{
			file_deny_write(cur->files[cur->fd]);
		}
		value=cur->fd++;
	}
	////////////////open///////////////////
	sema_down(&mutex);
	readcount--;
	if(readcount==0)
		sema_up(&wrt);
	sema_up(&mutex);
	return value;
}
int filesize(int fd)
{
	//returns the size, in bytes, of the file open as fd;
	struct thread *cur=thread_current();
	return file_length(cur->files[fd]);
}
void seek(int fd, unsigned position)
{
	struct thread *cur=thread_current();
	file_seek(cur->files[fd], position);
}
unsigned tell(int fd)
{
	//returns the position of the next byte to be read or written in open file fd, expressed in bytes from the beginning of the file
	struct thread *cur=thread_current();
	return file_tell(cur->files[fd]);
}
void close(int fd)
{
	//close the file descripter fd. exiting or terminating a process implicitly closes all its open file descriptors, as if by calling this function for each one.
	struct thread *cur=thread_current();
	file_close(cur->files[fd]);
	cur->files[fd]=NULL;
}
	void
syscall_init (void) 
{
	sema_init(&wrt,1);
	sema_init(&mutex,1);
	readcount=0;
	intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

	static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	//printf ("system call!\n");
	uint32_t callNumber=*(uint32_t*)(f->esp);
	void *esp=f->esp;
	uint32_t *returnValue=&(f->eax);
	//printf("%d", SYS_WRITE);
	if(callNumber==SYS_HALT)
		halt();
	if(callNumber==SYS_EXIT)
	{
		//return 넣어주기.
		checkPointer(esp+4);
		int status=*(int*)(esp+4);
		exit(status);
	}
	if(callNumber==SYS_EXEC)
	{
		checkPointer(esp+4);
		char *file=(char *)(*(uint32_t*)(esp+4));//4byte로 받아와서 char *로 바꾸어줌.
		*returnValue=exec(file);
	}
	if(callNumber==SYS_WAIT)
	{
		checkPointer(esp+4);
		uint32_t tid=*(uint32_t*)(esp+4);
		*returnValue=wait(tid);
	}
	if(callNumber==SYS_READ)
	{
		checkPointer(esp+4);
		int fd=*(int*)(esp+4);
		checkPointer(esp+8);
		void *buffer=(void*)(*(uint32_t*)(esp+8));//4byte로 받아와서 void로 바꾸어줌.
		checkPointer(buffer);
		checkPointer(esp+12);
		unsigned size=*(unsigned*)(esp+12);
		*returnValue=read(fd,buffer, size);
	}
	if(callNumber==SYS_WRITE)
	{
		checkPointer(esp+4);
		int fd=*(int*)(esp+4);
		checkPointer(esp+8);
		void *buffer=(void*)(*(uint32_t*)(esp+8));//4byte로 받아와서 void로 바꾸어줌.
		checkPointer(buffer);
		checkPointer(esp+12);
		unsigned size=*(unsigned*)(esp+12);
		*returnValue=write(fd, buffer, size);
	}
	if(callNumber==SYS_FIBONACCI)
	{
		checkPointer(esp+4);
		int n=*(int*)(esp+4);
		*returnValue=fibonacci(n);
	}
	if(callNumber==SYS_MAX_OF_FOUR_INT)
	{
		checkPointer(esp+4);
		int a=*(int*)(esp+4);
		checkPointer(esp+8);
		int b=*(int*)(esp+8);
		checkPointer(esp+12);
		int c=*(int*)(esp+12);
		checkPointer(esp+16);
		int d=*(int*)(esp+16);
		*returnValue=max_of_four_int(a,b,c,d);
	}
	if(callNumber==SYS_CREATE)
	{
		checkPointer(esp+4);
		char *file=*(char**)(esp+4);
		checkFile(file);
		checkPointer(file);
		checkPointer(esp+8);
		unsigned initial_size=*(unsigned*)(esp+8);
		*returnValue=create(file, initial_size);
	}
	if(callNumber==SYS_REMOVE)
	{
		checkPointer(esp+4);
		char *file=*(char**)(esp+4);
		checkFile(file);
		checkPointer(file);
		*returnValue=remove(file);
	}
	if(callNumber==SYS_OPEN)
	{
		checkPointer(esp+4);
		char *file=*(char**)(esp+4);
		checkFile(file);
		checkPointer(file);
		//printf("%s\n", file);
		//hex_dump(esp, esp, 100, 1);
		*returnValue=open(file);
	}
	if(callNumber==SYS_CLOSE)
	{
		checkPointer(esp+4);
		int fd=*(int*)(esp+4);
		checkFd(fd);
		close(fd);
	}
	if(callNumber==SYS_FILESIZE)
	{
		checkPointer(esp+4);
		int fd=*(int*)(esp+4);
		checkFd(fd);
		*returnValue=filesize(fd);
	}
	if(callNumber==SYS_SEEK)
	{
		checkPointer(esp+4);
		int fd=*(int*)(esp+4);
		checkFd(fd);
		checkPointer(esp+8);
		unsigned position=*(unsigned*)(esp+8);
		seek(fd, position);
	}
	if(callNumber==SYS_TELL)
	{
		checkPointer(esp+4);
		int fd=*(int*)(esp+4);
		checkFd(fd);
		*returnValue=tell(fd);
	}

	//thread_exit ();
}
