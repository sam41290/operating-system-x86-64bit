#include <stdio.h>
#include <stdlib.h>

/*

void *malloc(size_t size){

	unsigned long syscallnumber = 9;
	void *addr = NULL;
	size_t length = size;
	int prot = 1|2;//PROT_READ;
	int flags = 1|0x20;//MAP_SHARED;
    int fd = -1; int offset = 0;
    void* ret = NULL;

  	__asm__(
		"movq %1, %%rax;\n"
		"movq %2, %%rdi;\n"
		"movq %3, %%rsi;\n"
		"movq %4, %%rdx;\n"
		"movq %5, %%r10;\n"
		"movq %6, %%r8;\n"
		"movq %7, %%r9;\n"
		"syscall;\n"
		"movq %%rax, %0;\n"
		: "=m" (ret)
		: "m" (syscallnumber), "m" (addr), "m" (length), "m" (prot), "m" (flags), "m" (fd), "m" (offset)
		: "rax","rdi", "rsi", "rdx", "%r10", "%r8", "%r9"
	);  

	return ret;
}
*/


void exit(int status){
	//printf("calling exit\n");
	unsigned long syscallnumber = 60;
	uint64_t exit_status=status;
	
	__asm__(
		"movq %0, %%rax;\n"
		"movq %1, %%rdi;\n"
		"int $0x80;\n"
		:
		:"g"(syscallnumber),"g"(exit_status)
	);
}