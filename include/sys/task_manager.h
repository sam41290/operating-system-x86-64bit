#include<sys/idt.h>
#include <sys/vfs.h>
#ifndef _PROC_H
#define _PROC_H


#define MAX_FD 10
struct vma_struct{
	uint64_t vstart;
	uint64_t vend;
	struct vma_struct* nextvma;
};

typedef struct vma_struct vma;


struct mm{
	vma* vma_list;
};

typedef struct mm mm_struct;

struct pcb_t
{
	uint64_t sno;
	uint64_t pid;
	uint64_t ppid;
	uint64_t entry_point;
	uint64_t u_stack;
	uint64_t u_stackbase;
	uint64_t k_stackbase;
	uint64_t k_stack;
	uint64_t state;
	uint64_t cr3;
	__volatile__ int waitstate;
	__volatile__ int waitingfor;
	__volatile__ uint64_t signalling_child;
	__volatile__ int sigchild_state;
	mm_struct mmstruct;
	file_object* fd[MAX_FD]; 
	uint64_t heap_top;
	struct pcb_t *next;
};

typedef struct pcb_t PCB;

uint64_t switch_to(gpr_t *);
void kernel_switch_to();
void init_proc();
void test();

void syscall_init();
void create_kstack(PCB *proc);
void copy_kstack(PCB *proc);
void change_ptable(uint64_t addr);
void init_stack(PCB *proc);
PCB *get_nextproc();
int check_cow(uint64_t addr);
void cow(uint64_t addr);
void copy_parent_stack();
uint64_t mappageTable();
void copy_vma(PCB *proc);
void create_new_process(int proc_index);
void context1();

void context2();
#endif
