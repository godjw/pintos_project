#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <stdlib.h>
//#include "../lib/user/syscall.h'"
#include "../userprog/syscall.h"
#include "syscall.h"
//int fibo(int n);
//int max_of_four_int(a,b,c,d);

int main(int argc, char *argv[]){
	int a, b, c, d;
	int max, fibo;
	
	a = atoi(argv[1]);
	b = atoi(argv[2]);
	c = atoi(argv[3]);
	d = atoi(argv[4]);

	fibo = fibonacci(a);
	max = max_of_four_int(a, b, c, d);
	printf("%d %d\n", fibo, max);
	
	return EXIT_SUCCESS;
}
