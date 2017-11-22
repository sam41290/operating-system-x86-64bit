#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/defs.h>



struct task_struct{
	char kstack[4096];
	// uint8_t kstack[4096]__attribute__((aligned(16)));
	uint64_t pid;
	uint64_t rsp;
	uint64_t rip;
	uint64_t cr3;
	enum { RUNNING, SLEEPING, ZOMBIE } state;
	int exit_status;
	struct task_struct* next;
};

typedef struct task_struct task_struct;
task_struct* create_new_task();
task_struct* get_ready_task();
void add_to_task_list(task_struct* new_task);
void schedule(task_struct* me, task_struct* next);
void create_idle_task();
void schedule_idle_task();
void yield();
void schedule2(task_struct* me, task_struct* next);
#endif