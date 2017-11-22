#include<sys/kprintf.h>

int (*p[1])(void);

int syscall_print(void)
{
	//kprintf("execute syscall %p\n",p[0]);
	uint64_t printval;
	__asm__(
	"movq %%rbx,%0;\n"
	:"=g"(printval)
	);
	//kprintf("123::%p\n",printval);
	char *print=(char *)printval;
	kprintf("%s",print);
	return 0;
}



void syscall_init()
{
	//kprintf("iiiiiioooo\n");
	p[0]=syscall_print;
}

