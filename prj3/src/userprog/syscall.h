#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
void exit(int stat);
void check(const void *vaddr);
int write(int fd,const void *buffer, unsigned size);
void halt(void);
//tid_t exec(const char* cmd, struct intr_frame* f);
//int wait(tid_t tid);
int read(int fd, void* buffer, unsigned size);
int max_of_four_int(int a, int b, int c, int d);
int fibonacci(int n);

//proj2
//bool create(const char* file, unsigned initial_size);
//bool remove(const char* file);
#endif /* userprog/syscall.h */
