#include <signal.h>

#include <sys/defs.h>
#include <stdio.h>

int kill(pid_t pid, int sig)
{

	unsigned long syscallnumber = 53;

	if(pid==1)
	{
		// printf("Kill pid %d: permission denied\n",pid);
		puts("Permission denied");
		return 0;
	}
			
	__asm__(
	"movq %0, %%rax;\n"
	"movq %1, %%rdi;\n"
	"int $0x80;\n"
	: 
	: "m" (syscallnumber),"m"(pid)
	: "rax","rdi"
	);

	return 1;
}