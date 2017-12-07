#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/defs.h>
#include <stdio.h>
#include <stdlib.h>

//Reference : https://filippo.io/linux-syscall-table/ + http://man7.org/linux/man-pages/man2/ + https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html


void free(void *ptr)
{

	uint64_t addr=(uint64_t)ptr;
	uint64_t len=sizeof(*ptr);
	unsigned long syscallnumber = 11;

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

//Reference : https://filippo.io/linux-syscall-table/ + http://man7.org/linux/man-pages/man2/mmap.2.html + https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
void *malloc(uint64_t size){

	unsigned long syscallnumber = 9;
	void *addr = NULL;
	uint64_t length = size;
	uint64_t prot = 1|2;//PROT_READ;
	uint64_t flags = 1|0x20;//MAP_SHARED;
    uint64_t fd = -1; 
    uint64_t offset = 0;
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