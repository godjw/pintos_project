#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include <stdlib.h>
//#include <syscall.h>
#include <string.h>
#include "threads/interrupt.h"
#include "devices/shutdown.h"
#include "threads/vaddr.h"
#include "exception.h"
#include "process.h"
#include "devices/input.h"
#include "filesys/filesys.h"
#include "filesys/off_t.h"
#include "threads/synch.h"
#include "filesys/file.h"

static void syscall_handler (struct intr_frame *);
tid_t exec(const char* cmd, struct intr_frame *f);
int wait(tid_t tid);
int fibonacci(int n);
int max_of_four_int(int a, int b, int c, int d);

//proj2

bool create(const char* file, unsigned initial_size);
bool remove(const char* file);
int open(const char* file);
void close(int fd);
int filesize(int fd);
void seek(int fd, unsigned pos);
unsigned tell(int fd);

struct semaphore mutex, wrt;
int readcnt;

void
syscall_init (void) 
{
  sema_init(&mutex, 1);
  sema_init(&wrt, 1);
  readcnt = 0;
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void exit(int stat){
	thread_current() -> exit_status = stat;
	//list_remove(&thr->child_elem);
	for(int i = 3; i < 131; i++){
		if(thread_current()->files[i] != NULL)
			close(i);
	}
	printf("%s: exit(%d)\n", thread_name(), stat);
	thread_exit();
}

void check(const void *vaddr){
	if(!is_user_vaddr(vaddr))
		exit(-1);
	 if(vaddr == NULL)
		exit(-1);
}

void halt(void){
	shutdown_power_off();
}

tid_t exec(const char* cmd, struct intr_frame *f){
	int pid = process_execute(cmd);
	f->eax = pid;
	return pid;
}

int wait(tid_t tid){
	return process_wait(tid);
}

int read(int fd, void* buffer, unsigned size){
  	//uint8_t *read_buf = (uint8_t*) buffer;
	int i = 0;
	struct thread* thr = thread_current();
	
	check(buffer);
	sema_down(&mutex);
	readcnt++;
	if(readcnt == 1)
		sema_down(&wrt);
	sema_up(&mutex);
		
	//check(buffer);
	if(fd == 0){
		for(i = 0; i < (int)size; i++){
			if(input_getc() == '\0')
				break;
		}
		
	}
	else if(fd > 2){
		/*if(thread_current()->files[fd] == NULL)
			exit(-1);*/
		i = (int)file_read(thr->files[fd], buffer, (off_t)size);
	}
	else
		i = -1;
	
	sema_down(&mutex);
	readcnt--;
	if(readcnt == 0)
		sema_up(&wrt);
	sema_up(&mutex);

	return i;
}

int write(int fd,const void *buffer, unsigned size){
	struct thread* thr = thread_current();
	int i = 0;
	check(buffer);
	sema_down(&wrt);
	if(fd == 1){
		putbuf((char *)buffer, size);
		i = size;
	}
	else if(fd > 2){
		if(thread_current()->files[fd] == NULL){
			sema_up(&wrt);
			exit(-1);
		}
		i = (int)file_write(thr->files[fd], buffer, (off_t)size);
	}
	else 
		i = -1;
	sema_up(&wrt);

	return i;
}

int fibonacci(int n){
	int sum;
	int num1, num2;

	num1 = 1;
	num2 = 1;
	sum = 0;

	if(n==1)
		return 1;
	else if(n==2)
		return 1;
	else{
		for(int i = 2; i < n; i++){
			sum = num1 + num2;
			num1 = num2;
			num2 = sum;		
		}
		return sum;
	}
}

int max_of_four_int(int a, int b, int c, int d){
	int max;
	max = a;

	if(max < b)
		max = b;
	if(max < c)
		max = c;
	if(max < d)
		max = d;
	return max;
}
/*--------------------------------------------

 				proejct 2

--------------------------------------------*/
bool create(const char *file, unsigned initial_size){
	check(file);
	return filesys_create(file, (off_t)initial_size);
}

bool remove(const char *file){
	check(file);
	return filesys_remove(file);
}

int open(const char* file){
	struct thread* thr = thread_current();
	struct file* fp;
	int i;
	
	check(file);
	fp = filesys_open(file);
	if(fp == NULL)
		return -1;
	if(!strcmp(thread_name(), file))
		file_deny_write(fp);
	
	for(i = 3; i < 131; i++){
		if(thr->files[i] == NULL){
			thr->files[i] = fp;
			return i;
		}
	}
	
	return -1;
}

int filesize(int fd){
	return file_length(thread_current()->files[fd]);
}

void seek(int fd, unsigned pos){
	file_seek(thread_current()->files[fd], pos);
}

unsigned tell(int fd){
	return file_tell(thread_current()->files[fd]);
}

void close(int fd){
	struct file* fp;
	if(thread_current()->files[fd] == NULL)
		exit(-1);
	fp = thread_current()->files[fd];
	thread_current()->files[fd] = NULL;
	file_close(fp);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int call_num;
  call_num = *(uint32_t *)(f -> esp);
  //printf("syscall %d\n", call_num);
  //hex_dump(f->esp, f->esp, 100, 1);

  if(call_num == SYS_HALT){
	//printf("SYS HALT\n");
	halt();
  }

  else if(call_num == SYS_EXIT){
	//printf("SYS EXIT\n");

	check(f->esp + 4);
	exit(*(uint32_t *)(f->esp + 4));
	f->eax = *(uint32_t *)(f->esp + 4);
  }

  else if(call_num == SYS_EXEC){
  	check(f->esp + 4);
	f->eax = exec((const char*)*(uint32_t *)(f->esp + 4), f);
  }
  else if(call_num == SYS_WAIT){
  	check(f->esp + 4);
	f->eax = wait((tid_t)*(uint32_t *)(f->esp + 4));
  }
  else if(call_num == SYS_READ){
  	check(f->esp + 4);
	check(f->esp + 8);
	check(f->esp + 12);

	f->eax = read((int)*(uint32_t *)(f->esp + 4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*((uint32_t *)(f->esp + 12)));
  }

  else if(call_num == SYS_WRITE){
	//printf("SYS WRITE\n");
	check(f->esp + 4);
	check(f->esp + 8);
	check(f->esp + 12);
	f->eax = write((int)*(uint32_t *)(f->esp + 4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*((uint32_t *)(f->esp + 12)));
  }
  else if(call_num == FIBONACCI){
	f-> eax = fibonacci((tid_t)*(uint32_t *)(f->esp + 4));
  }
  else if(call_num == MAX_OF_FOUR_INT){
	check(f->esp + 4);
	check(f->esp + 8);
	check(f->esp + 12);
	check(f->esp + 16);
	f->eax = max_of_four_int((int)*(uint32_t *)(f->esp + 4), (int)*(uint32_t *)(f->esp + 8), (int)*(uint32_t *)(f->esp + 12), (int)*(uint32_t *)(f->esp + 16));
  }

  //proj2
  else if(call_num == SYS_CREATE){
  	check(f->esp + 4);
  	check(f->esp + 8);
  	f->eax = create((const char *)*(uint32_t *)(f->esp + 4), (unsigned)*(uint32_t *)(f->esp + 8));
	}
  else if(call_num == SYS_REMOVE){
	check(f->esp + 4);
	f->eax = remove((const char *)*(uint32_t *)(f->esp + 4));
  }
  else if(call_num == SYS_OPEN){
	check(f->esp + 4);
	f->eax = open((const char *)*(uint32_t *)(f->esp + 4));
  }
  else if(call_num == SYS_FILESIZE){
	check(f->esp + 4);
	f->eax = filesize((int)*(uint32_t *)(f->esp + 4));
  }
  else if(call_num == SYS_SEEK){
	check(f->esp + 4);
	check(f->esp + 8);
	seek((int)*(uint32_t *)(f->esp +4), (unsigned)*(uint32_t *)(f->esp + 8));
  }
  
  else if(call_num == SYS_TELL){
	check(f->esp + 4);
	f->eax = tell((int)*(uint32_t *)(f->esp + 4));
  }

  else if(call_num == SYS_CLOSE){
	check(f->esp + 4);
	close((int)*(uint32_t *)(f->esp + 4));
  }

  //printf ("system call!\n");
  //thread_exit ();
}
