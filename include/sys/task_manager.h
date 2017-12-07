#include<sys/idt.h>
#include <sys/vfs.h>
#ifndef _PROC_H
#define _PROC_H

#define MAX_FD 20

#define PROC_SIZE 100
#define CHILD 0
#define NEW 1

#define NEW_PROC 0
#define RUNNING 1
#define ZOMBIE 2
#define DEAD 5

#define WAITING_TO_LIVE 1
#define WAITING_TO_DIE 2

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
	__volatile__ uint64_t pid;
	__volatile__ uint64_t ppid;
	__volatile__ int waitstate;
	__volatile__ int waitingfor;
	__volatile__ uint64_t signalling_child;
	__volatile__ int sigchild_state;
	mm_struct mmstruct;
	uint64_t* fd[MAX_FD]; 
	uint64_t heap_top;
	char *name;
	char *currentDir;
	uint64_t entry_point;
	uint64_t u_stack;
	uint64_t u_stackbase;
	uint64_t k_stackbase;
	uint64_t k_stack;
	uint64_t state;
	uint64_t cr3;
	struct pcb_t *next;
}__attribute__((packed));

typedef struct pcb_t PCB;


struct proc_lst
{
	uint64_t pid;
	uint64_t ppid;
	uint64_t state;
	uint64_t waitstate;
	char name[1024];
};

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
uint64_t mappageTable(uint64_t proc_index, int new);
void copy_vma(PCB *proc);
void create_new_process(int proc_index,int new);
void increment_childpg_ref(uint64_t vaddr_start,uint64_t vaddr_end);

void copy_cur_dir(PCB *proc);
void copy_name(PCB *proc,char *str);
void context1();

void context2();


#endif
