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
//#include <input.h>

static void syscall_handler (struct intr_frame *);
tid_t exec(const char* cmd, struct intr_frame *f);
int wait(tid_t tid);
int fibonacci(int n);
int max_of_four_int(int a, int b, int c, int d);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void exit(int stat){
	thread_current() -> exit_status = stat;
	//list_remove(&thr->child_elem);
	printf("%s: exit(%d)\n", thread_name(), stat);
	thread_exit();
}

void check(const void *vaddr){
	if(!is_user_vaddr(vaddr))
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
	uint8_t *read_buf = (uint8_t*) buffer;
	int i = 0;
	if(fd == 0){
		for(i = 0; i < (int)size; i++){
			read_buf[i] = input_getc();
			//if(((char*)buffer)[i] == '\0')
			//break;
		}
		return i;
	}
	return -1;
}

int write(int fd,const void *buffer, unsigned size){
	if(fd == 1){
		putbuf((char *)buffer, size);
		return size;
	}
	return -1;
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
  else if(call_num == SYS_CREATE){
  }
  else if(call_num == SYS_REMOVE){
  }
  else if(call_num == SYS_OPEN){
  }
  else if(call_num == SYS_FILESIZE){
  	check(f->esp + 4);
  }
  else if(call_num == SYS_READ){
  	check(f->esp + 4);
	check(f->esp + 8);
	check(f->esp + 12);

	read((int)*(uint32_t *)(f->esp + 4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*((uint32_t *)(f->esp + 12)));
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
  //printf ("system call!\n");
  //thread_exit ();
}
