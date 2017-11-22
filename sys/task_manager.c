#include<sys/kprintf.h>
#include<sys/virt_mem.h>
#include<sys/task_manager.h>
#include<sys/phy_mem_manager.h>
#include<sys/gdt.h>
#include<sys/paging.h>
#include<sys/tarfs.h>
#define PROC_SIZE 10

PCB *proc_start;
PCB *proc_end;

PCB *active;

uint64_t kernbase;

uint64_t upml4;

uint64_t ustacktop;

uint64_t u_heapboundary;

uint64_t global_pid=0;

extern int ctr;


void _x86_64_asm_ltr(uint64_t tss_idx);

PCB *get_nextproc()
{
	if(proc_start==proc_end)
	{
		kprintf("0 process in the queue\n");
		return NULL;
	}
	PCB *temp=proc_start;
	proc_start=proc_start->next;
	return temp;
}

uint64_t init_stack()
{
	int i;
	uint64_t top=(ustacktop -(4096)) & 0xFFFFFFFFFFFFF000;
	//kprintf("\nfrom init top: %p\n",top);
	for(i=0;i<=4096;i++)
	{
		*((uint64_t *)(top - i))=0;
	}
	
	return top;
}


uint64_t mappageTable()
{
	
	uint64_t *v_pml4=(uint64_t *)(upml4-(4096 * global_pid));
	//kprintf("\nv_pml4:%p\n",v_pml4);
	
	
	
	uint64_t p_pml4=map_phyaddr((uint64_t)v_pml4);
	//kprintf("\np_pml4:%p\n",p_pml4);
	uint64_t base=0xFFFF000000000000;

	base=(((base >> 48)<<9)|0x1FE)<<39;
	base=(((base >> 39)<<9)|0x1FE)<<30;
	base=(((base >> 30)<<9)|0x1FE)<<21;
	base=(((base >> 21)<<9)|0x1FE)<<12;

	uint64_t *p1=(uint64_t *)base;
	
	for(int i=0;i<512;i++)
	{
		v_pml4[i]=0x0;
	}
	v_pml4[511]=p1[511];
	v_pml4[510]=(p_pml4 & 0xFFFFFFFFFFFFF000) | 7;
	//kprintf("\nvpml4 511: %p\n",v_pml4[511]);
	//kprintf("old 511: %p\n",(p1[511] & 0xFFFFFFFFFFFFF000) | 7);
	//while(1);
	return p_pml4;
}

void switch_to()
{
	__asm__(
	"pushq %%rax;\n"
	"pushq %%rbx;\n"
	"pushq %%rcx;\n"
	"pushq %%rdx;\n"
	"pushq %%rsi;\n"
	"pushq %%rdi;\n"
	"pushq %%rbp;\n"
	"pushq %%r8;\n"
	"pushq %%r9;\n"
	"pushq %%r10;\n"
	"pushq %%r11;\n"
	"pushq %%r12;\n"
	"pushq %%r13;\n"
	"pushq %%r14;\n"
	"pushq %%r15;\n"
	:
	:
	);
	proc_end->pid=active->pid;
	
	proc_end->k_stack=active->k_stack;
	
	uint64_t user_stack;

	__asm__(
	"movq %%rsp,%0;\n"
	:"=g"(user_stack)
	:
	);
	proc_end->u_stack=user_stack;
	
	proc_end->entry_point=active->entry_point;
	proc_end->next=(PCB *)kmalloc(sizeof(PCB));
	proc_end->state=1;
	proc_end=proc_end->next;
	PCB *p_to=get_nextproc();
	active=p_to;
	if(p_to->state==0)
	{
		
		__asm__(
		"movq %0,%%rsp;\n"
		"pushq %1;\n"
		"retq;\n"
		:
		:"g"(p_to->u_stack),"g"(p_to->entry_point)
		);
	}
	else
	{
		__asm__(
		"movq %0,%%rsp;\n"
		"popq %%r15;\n"
		"popq %%r14;\n"
		"popq %%r13;\n"
		"popq %%r12;\n"
		"popq %%r11;\n"
		"popq %%r10;\n"
		"popq %%r9;\n"
		"popq %%r8;\n"
		"popq %%rbp;\n"
		"popq %%rdi;\n"
		"popq %%rsi;\n"
		"popq %%rdx;\n"
		"popq %%rcx;\n"
		"popq %%rbx;\n"
		"popq %%rax;\n"
		//"retq;\n"
		:
		:"g"(p_to->u_stack)
		);
		return;
		
	}
}

void change_ptable(uint64_t addr)
{
	uint64_t pml4=addr;
	__asm__(
	"movq %0,%%rax;\n"
	"movq %%rax,%%cr3;\n"
	:
	:"g"(pml4)
	);
	
	//while(1);
	return;
}

void init_proc()
{
	proc_start=(PCB *)kmalloc(sizeof(PCB));

	proc_end=proc_start;
	
	proc_end->cr3=mappageTable();
	if(proc_end->cr3==0)
	{
		kprintf("\npage table creation failed\n");
		return;
	}
	//kprintf("test: %p\n",proc_end->cr3);
	//while(1);
	change_ptable(proc_end->cr3);
	//kprintf("here\n");
	
	
	proc_end->pid=global_pid;
	global_pid++;
	proc_end->next=(PCB *)kmalloc(sizeof(PCB));
	proc_end->state=0;
	
	
	scan_tarfs(proc_end);
	proc_end->u_stack=init_stack();
	
	uint64_t kernel_stack=0;
	
	__asm__(
	"movq %%rsp,%0;\n"
	:"=g"(kernel_stack)
	:
	);
	
	proc_end->k_stack=kernel_stack;
	proc_end=proc_end->next;

	active=proc_start;
	proc_start=proc_start->next;
	active->state=1;

	kprintf("user stack: %p\n",active->u_stack);
	set_tss_rsp((void *)active->k_stack);
	
	
     __asm__( 
     "pushq $0x23;\n"
     "pushq %0;\n"
     "pushf;\n"
     "pushq $0x1B;\n"
     "pushq %1;\n"
	 "iretq;\n"
	 :
	 :"g"(active->u_stack),"g"(active->entry_point)
	 );
	 
     
	/*
	__asm__(
	"movq %0,%%rsp;\n"
	"pushq %1;\n"
	"retq;\n"
	:
	:"g"(active->u_stack),"g"(active->entry_point)
	);
	*/
	
	return;
}