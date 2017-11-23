#include<sys/kprintf.h>
#include<sys/idt.h>

int (*p[1])();

int syscall_print()
{
	//kprintf("execute syscall %p\n",p[0]);
	uint64_t printval;
	__asm__(
	"movq %%rbx,%0;\n"
	:"=g"(printval)
	);
	
	char *print=(char *)printval;
	//kprintf("123::%p\n",printval);
	//kprintf("user stack: %p re-entry point: %p rax:\n",reg.usersp,reg.rip);	
	kprintf("%s",print);
	
	return 1;
}



void syscall_init()
{
	//kprintf("iiiiiioooo\n");
	p[0]=syscall_print;
}

