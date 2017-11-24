#include<sys/virt_mem.h>
#include<sys/task_manager.h>
#include<sys/phy_mem_manager.h>
#include<sys/gdt.h>
#include<sys/idt.h>
#include<sys/paging.h>
#include<sys/tarfs.h>
#include<sys/kmalloc.h>
#include<sys/defs.h>
#define PROC_SIZE 100


extern uint64_t RING_0_MODE;
int proc_start=0;
int proc_end=0;

PCB *active = NULL;

PCB proc_Q[101];

PCB all_pro[100];

uint64_t kernbase;

uint64_t upml4;

uint64_t ustacktop;

uint64_t u_heapboundary;

uint64_t global_pid=0;

extern int ctr;

extern uint64_t kern_VA;

extern uint8_t initial_stack[4096];

uint64_t proc_descriptor[100];

uint64_t kstackbase;


PCB *get_nextproc()
{
	if(proc_start==proc_end)
	{
		kprintf("0 process in the queue\n");
		return NULL;
	}
	PCB *temp=proc_Q + proc_start;
	//kprintf("from get next new kstack:%p new:%p\n",(temp)->k_stack,(temp)->u_stack);
	proc_start=proc_start + 1;
	return temp;
}

void init_stack(PCB *proc)
{
	int i;
	if(proc->pid==0)
	{
		uint64_t top=(ustacktop -(4096)) & 0xFFFFFFFFFFFFF000;
		//kprintf("\nfrom init top pid=0: %p\n",top);
		for(i=0;i<=4096;i++)
		{
			*((uint64_t *)(top - i))=0;
		}
		proc->u_stack=top;
		proc->u_stackbase=top;
	}
	else
	{	//kprintf("\nfrom init active->u_stackbase: %p %p\n",active->u_stack,*((uint64_t *)(active->u_stackbase-1)));
		uint64_t  top=(uint64_t )active->u_stack;
		
		top=(top -(4096)) & 0xFFFFFFFFFFFFF000;
		proc->u_stackbase=top;
		uint64_t parent_stackbase=(uint64_t )active->u_stackbase;
		//kprintf("\nfrom init top: %p\n",top);
		for(i=0;i<=4096;i++)
		{
			if(parent_stackbase>=active->u_stack)
			{
				*((uint8_t *)(top))=*((uint8_t *)parent_stackbase);
				proc->u_stack=top;
			}
			else
				*((uint8_t *)(top))=0;
			top--;
			parent_stackbase--;
		}
		//proc->u_stack;
		//kprintf("\nfrom init base: %p %p\n",proc->u_stack,*((uint64_t *)proc->u_stack));
		
	}
	
	return;
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

void kernel_switch_to()
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
	(proc_Q + proc_end)->pid=active->pid;
	
	(proc_Q + proc_end)->k_stack=active->k_stack;
	
	uint64_t user_stack;

	__asm__(
	"movq %%rsp,%0;\n"
	:"=g"(user_stack)
	:
	);
	(proc_Q + proc_end)->u_stack=user_stack;
	
	(proc_Q + proc_end)->entry_point=active->entry_point;
	//proc_end->next=(PCB *)kmalloc(sizeof(PCB));
	(proc_Q + proc_end)->state=1;
	proc_end=proc_end + 1;
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

void init_kstack()
{
	//kprintf("\nhere\n");
	kstackbase=(uint64_t)((kern_VA + 4096) & 0xFFFFFFFFFFFFF000);
	kern_VA = kern_VA + (4096 * 101);
	//kprintf("\nhere\n");
	//while(1);
}

void create_kstack(PCB *proc)
{
	int i;
	//kstackdescriptor[proc->pid]=0;
	uint64_t top=kstackbase + (4096 * proc->sno);
	//kprintf("\nfrom init top: %p\n",top);
	for(i=0;i<=4096;i++)
	{
		*((uint8_t *)(top + i))=0;
		top++;
	}
	
	//kprintf("\nfrom init top: %p\n",top);
	proc->k_stack=top;
	proc->k_stackbase=top;
}

void copy_kstack(PCB *proc)
{
	uint64_t kernel_stack=0;
	
	__asm__(
	"movq %%rsp,%0;\n"
	:"=g"(kernel_stack)
	:
	);
	
	uint64_t k_base;
	
	if(proc->pid==0)
		k_base=(uint64_t)&initial_stack[4096];
	else
		k_base=active->k_stackbase;
	//kprintf("kernel_stack:%p initial_stack:%p\n",kernel_stack,k_base);
	while (k_base>=kernel_stack)
	{
		*((uint8_t *)(proc->k_stack))=*((uint8_t *)k_base);
		k_base--;
		proc->k_stack--;
	}
	proc->k_stack++;
	//kprintf("new k stack:%p\n",proc->k_stack);
	//while(1);
	__asm__(
	 "movq %0,%%rsp;\n"
	 :
	 :"g"(proc->k_stack)
	 );
	//while(1);
}
/*
PCB* create_new_process(){
	PCB* new_task = (PCB *)kmalloc(sizeof(PCB));
	new_task->mmstruct.vma_list = NULL;
	//initialize other PCB members if needed
	return new_task;
}*/



void init_proc()
{
	for(int i=0;i<100;i++)
	{
		proc_descriptor[i]=0;
	}
	init_kstack();
	int proc_index=0;
	(all_pro + proc_index)->sno=proc_index;
	((all_pro + proc_index)->mmstruct).vma_list = NULL;
	(all_pro + proc_index)->cr3=mappageTable();
	if((all_pro + proc_index)->cr3==0)
	{
		kprintf("\npage table creation failed\n");
		return;
	}
	//kprintf("test: %p\n",proc_end->cr3);
	//while(1);
	change_ptable((all_pro + proc_index)->cr3);
	
	
	(all_pro + proc_index)->pid=global_pid;
	global_pid++;
	(all_pro + proc_index)->next = (all_pro + proc_index + 1);
	(all_pro + proc_index)->state=0;
	scan_tarfs(all_pro + proc_index);
	//proc_end->entry_point=(uint64_t)test;
	
	
	init_stack(all_pro + proc_index);
	create_kstack(all_pro + proc_index);
	copy_kstack(all_pro + proc_index);
	
	
	//while(1);
	proc_descriptor[proc_index]=1;
	proc_Q[proc_end]=all_pro[proc_index];
	proc_end=proc_end + 1;
	active=(all_pro + proc_index);
	proc_start=proc_start + 1;
	active->state=1;

	kprintf("user stack: %p\n",active->u_stack);
	
	set_tss_rsp((void *)active->k_stack);
	
	RING_0_MODE=0;
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
