#include <stdlib.h>

void _start(void) {

	int argc = 1;
    char* argv[10];
    char* envp[10];
    main(argc, argv, envp);
	//return;
	exit(0);
  // call main() and exit() here
}
