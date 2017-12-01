#include <stdlib.h>

void _start(void) {
	
	uint64_t argc = 0;
	uint64_t args;
    char** argv;

	__asm__(
	"movq %%rdi,%0;\n"
	"movq %%rsi,%1;\n"
	:"=g"(args),"=g"(argc)
	:
	);

	argv=(char **)args;
	
    main(argc, argv,NULL);
	//return;
	exit(0);
  // call main() and exit() here
}
