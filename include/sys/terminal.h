#ifndef _TERMINAL_H_
#define _TERMINAL_H_
#include <sys/defs.h>

void init_terminal();

struct terminal{
	char stdin[1024];
	int stdinlength;
	uint64_t (*read)(struct terminal* self, uint64_t addr);
	uint64_t (*write)(struct terminal* self, uint64_t addr, uint64_t count);
	void (*tunnel)(struct terminal* self);
};

#endif