#include <stdio.h>

int gets(char* buff){

	long fd=0;
	long count=1;
	unsigned long syscallnumber = 0;
	int read_count;

	__asm__(
		"movq %1, %%rax;\n"
		"movq %2, %%rdi;\n"
		"movq %3, %%rsi;\n"
		"movq %4, %%rdx;\n"
		"int $0x80;\n"
		"movq %%rax, %0;\n"
		: "=m" (read_count)
		: "m" (syscallnumber), "m" (fd), "m" ((unsigned long)buff), "m" (count)
		: "rax","rdi", "rsi", "rdx"
	);

	//Return value doesnt work maybe need to deep copy use the parameter buff in app
	return read_count;
}
