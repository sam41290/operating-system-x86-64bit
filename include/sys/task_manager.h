
#ifndef _PROC_H
#define _PROC_H
struct pcb_t
{
uint64_t pid;
uint64_t ppid;
uint64_t entry_point;
uint64_t u_stack;
uint64_t k_stackbase;
uint64_t k_stack;
uint64_t state;
uint64_t cr3;
struct pcb_t *next;
};

typedef struct pcb_t PCB;

void switch_to();
void init_proc();
void test();

void syscall_init();

void context1();

void context2();
#endif