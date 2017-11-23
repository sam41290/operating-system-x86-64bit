#include <stdarg.h>
#include <sys/defs.h>
#include <stdio.h>
#include <stdlib.h>

void *malloc(uint64_t size){

	unsigned long syscallnumber = 1;
	void *addr = NULL;
	uint64_t length = size;
	int prot = 1|2;//PROT_READ;
	int flags = 1|0x20;//MAP_SHARED;
    int fd = -1; int offset = 0;
    void* ret = NULL;
  
	__asm__(
	"movq %1,%%rax;\n"
	"movq %2,%%rdi;\n"
	"movq %3,%%rsi;\n"
	"movq %4,%%rdx;\n"	
	"movq %5,%%r10;\n"	
	"movq %6,%%r9;\n"
	"movq %7,%%r8;\n"
	"int $0x80;\n"
	"movq %%rax, %0"
	:"=m"(ret)
	:"m"(syscallnumber),"m"((uint64_t)addr),"m"(length),"m"(prot),"m"(flags),"m"(fd),"m"(offset)
	);

	return ret;
}

void free(void *ptr)
{

	uint64_t addr=(uint64_t)ptr;

	uint64_t len=sizeof(*ptr);

	unsigned long syscallnumber = 2;

	  __asm__(
	                "movq %0, %%rax;\n"
	                "movq %1, %%rdi;\n"
	                "movq %2, %%rsi;\n"
					"int $0x80;\n"
	                : 
	                : "m" (syscallnumber), "m" ((uint64_t)addr), "m" (len)
	                : "rax","rdi", "rsi"
	        );

}