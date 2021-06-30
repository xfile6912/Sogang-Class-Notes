#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "threads/thread.h"

void syscall_init (void);

void checkPointer(void *vaddr);
void halt(void);
void exit(int status);
tid_t exec(const char *file);
int wait (tid_t tid);
int read(int fd, void *buffer, unsigned size);
int write(int fd, const void *buffer, unsigned size);
int fibonacci(int n);
int max_of_four_int(int a, int b, int c, int d);
#endif /* userprog/syscall.h */
