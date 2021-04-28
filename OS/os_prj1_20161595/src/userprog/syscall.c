#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
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

void halt(void)
{
//	printf("halt\n");
	shutdown_power_off();
}
void exit(int status)
{
//	printf("exit\n");
        struct thread * cur= thread_current();
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
	if(fd==0)
	{
		for(i=0; i<size; i++)
		{
			((char*)buffer)[i]=input_getc();
			if(((char*)buffer)[i]=='\0')break;
		}
		if(i!=size)
			return -1;
		return size;
	}
	return -1;

}
int write(int fd, const void *buffer, unsigned size)
{
	//	printf("write\n");
	if(fd==1)
	{
		//		input_putc(fd);
		putbuf(buffer, size);
		return size;
	}
	return -1;
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

	void
syscall_init (void) 
{
	intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

	static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	//printf ("system call!\n");
	uint32_t callNumber=*(uint32_t*)(f->esp);
	void *esp=f->esp;
	uint32_t returnValue=f->eax;
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
		returnValue=(uint32_t)exec(file);
	}
	if(callNumber==SYS_WAIT)
	{
		checkPointer(esp+4);
		uint32_t tid=*(uint32_t*)(esp+4);
		returnValue=(uint32_t)wait(tid);
	}
	if(callNumber==SYS_READ)
	{
		checkPointer(esp+4);
		int fd=*(int*)(esp+4);
		checkPointer(esp+8);
		void *buffer=(void*)(*(uint32_t*)(esp+8));//4byte로 받아와서 void로 바꾸어줌.
		checkPointer(esp+12);
		unsigned size=*(unsigned*)(esp+12);
		returnValue=(uint32_t)read(fd,buffer, size);
	}
	if(callNumber==SYS_WRITE)
	{
		// printf("%x\n", (int)esp);
		//hex_dump(esp, esp, 200, 1);
		checkPointer(esp+4);
		int fd=*(int*)(esp+4);
		checkPointer(esp+8);
		void *buffer=(void*)(*(uint32_t*)(esp+8));//4byte로 받아와서 void로 바꾸어줌.
		checkPointer(esp+12);
		unsigned size=*(unsigned*)(esp+12);
		returnValue=(uint32_t)write(fd, buffer, size);
	}
	if(callNumber==SYS_FIBONACCI)
	{
		checkPointer(esp+4);
		int n=*(int*)(esp+4);
		returnValue=(uint32_t)fibonacci(n);
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
		returnValue=(uint32_t)max_of_four_int(a,b,c,d);
	}
	
	f->eax=returnValue;

	//thread_exit ();
}
