#include <stdio.h>
#include <stdlib.h>

void foolcompiler(int* a){
	return;
}

void dummysyscall(){

	unsigned long syscallnumber = 1;
	void *addr = NULL;
	uint64_t length = 12345;
	int prot = 1|2;//PROT_READ;
	int flags = 1|0x20;//MAP_SHARED;
    int fd = -1; int offset = 0;
    int ret=0;
	__asm__(
	"movq %0,%%rax;\n"
	"movq %1,%%rdi;\n"
	"movq %2,%%rsi;\n"
	"movq %3,%%rdx;\n"	
	"movq %4,%%r10;\n"	
	"movq %5,%%r9;;\n"
	"movq %6,%%r8;\n"
	"int $0x80;\n"
	:
	:"m"(syscallnumber),"m"((uint64_t)addr),"m"(length),"m"(prot),"m"(flags),"m"(fd),"m"(offset)
	);
    //printf("returned from syscall\n");

	__asm__(
	"movq %%rax, %0"
	:"=m"(ret)
	:
	);
	printf("Returned From dummysyscall %d\n", ret);
	return;
}
int main(int argc, char *argv[], char *envp[]) {

	int a=10;
	// printf("sbush> %d\n",a);
	a=a+1;

	a=printf("sbush> %d\n",a);
	//a=a+1;

	printf("hello world!!! %d\n",a);

	 //dummysyscall();

	 int* trymalloc = (int*)malloc(12345);
	 printf("add malloc %p\n", trymalloc);	
	 trymalloc[0] = 1;
	 trymalloc[1] = 2;
	 trymalloc[2] = 3;
	 printf("result %d\n", trymalloc[0]);	
	 printf("result %d\n", trymalloc[1]);	
	 printf("result %d\n", trymalloc[2]);	
     
	 // trymalloc[1025] = 99;
	 // printf("result %d\n", trymalloc[1025]);	
	 free(trymalloc);
     
	 int* try2 = (int*)malloc(4096);
	 try2[0] = 1;
     
	 int* try3 = (int*)malloc(4096);
	 try3[0] = 1;
     
	 free(try2);

	while(1);
	return 0;
}
