#include <stdlib.h>
#include<stdio.h>

void _start(void) {
	
	
	//printf("in start!!!!!!!!!\n");
	
	__volatile__ uint64_t argc = 0;
	__volatile__ uint64_t args;
    char** argv;

	__asm__(
	"movq %%rdi,%0;\n"
	"movq %%rsi,%1;\n"
	:"=g"(args),"=g"(argc)
	:
	);

	argv=(char **)args;
	//printf("argc=%d\n",argc);
	//printf("argc=%d\n",argc);
	
    main(argc, argv,NULL);
	//return;
	exit(0);
  // call main() and exit() here
}
