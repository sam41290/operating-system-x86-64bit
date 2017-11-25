#include<sys/kprintf.h>
#include <sys/phy_mem_manager.h>
#include <sys/virt_mem.h>
#include <sys/task_manager.h>
#include <sys/idt.h>
#include <sys/error.h>
#include<sys/kmalloc.h>
#include<sys/debug.h>
#include<sys/gdt.h>

uint64_t (*p[200])(gpr_t *reg);

extern PCB *active;
extern PCB proc_Q[101];
extern PCB all_pro[100];
extern uint64_t proc_descriptor[100];
extern int proc_start;
extern int proc_end;
extern uint64_t global_pid;

uint64_t dead(gpr_t *reg){

	//Get the address from the register to free
	kprintf("R %d, %d, %d, %d, %d, %d %d\n", reg->rax, reg->rdi,reg->rsi, reg->rdx, reg->r10, reg->r9, reg->r8);
	kprintf("Dead called\n");
	return 12653712;
}


uint64_t k_mmap(gpr_t *reg){

	// Get values from registers
	//TODO Use these variables
	uint64_t length = reg->rsi;
	
    #ifdef DEBUG_MALLOC
	void *addr = (void*)reg->rdi;
	int prot = reg->rdx;
	int flags = reg->r10;
    int fd = reg->r9; 
    int offset = reg->r8;
	kprintf("addr %d length %d prot %d, flags %p, fd %d, offset %d\n", addr, length, prot, flags, fd, offset);
	#endif
	//call mmap
	//Change this later make some freelist of vaddr to reuse vaddr
	uint64_t nextAvailHeapMem = active->heap_top;

	//Point to next page after length
	uint64_t endAddr = active->heap_top + length - 1;
	uint64_t nextPageAfterEndAddr = (((endAddr>>12)+1)<<12); 
	active->heap_top = nextPageAfterEndAddr;

	vma* anon_vma = alloc_vma(nextAvailHeapMem, endAddr);

	append_to_vma_list(active, anon_vma);
	// walkthrough_vma_list(active);

	// while(1);

	#ifdef DEBUG_MALLOC
	kprintf("anon_vma %p %p active->heap_top %p\n", anon_vma->vstart, anon_vma->vend, active->heap_top);
	kprintf("malloc %p\n", nextAvailHeapMem);
	#endif

	#ifdef DEBUG_MALLOC
	printtotalFreeBlocks();
	#endif

	return nextAvailHeapMem;
}


uint64_t k_munmap(gpr_t *reg){

	//Get the address from the register to free

	uint64_t addrToFree = (uint64_t)reg->rdi;
	uint64_t length = (uint64_t)reg->rsi;
	#ifdef DEBUG_MALLOC
	kprintf("Requested %p to free length %d\n", addrToFree, length);
	#endif

	// Remove entry from vma
	if(remove_from_vma_list(active, addrToFree, addrToFree+length) == 0)
	{
		ThrowSegmentationFault(addrToFree);
	}
	//TODO remove entry from vma
	//Remove entry from pagetable and clean page
	#ifdef DEBUG_MALLOC
	printtotalFreeBlocks();
	#endif

	return 0;
}

uint64_t syscall_print(gpr_t *reg)
{
	//kprintf("execute syscall %p\n",p[0]);
	uint64_t printval = reg->rbx;
	// __asm__(
	// "movq %%rbx,%0;\n"
	// :"=g"(printval)
	// );
	//kprintf("123::%p\n",printval);
	char *print=(char *)printval;
	//kprintf("123::%p\n",printval);
	//kprintf("user stack: %p re-entry point: %p rax:\n",reg->usersp,reg->rip);	
	kprintf("%s",print);

	return 0;
}

uint64_t syscall_switch(gpr_t *reg)
{
	//kprintf("\nhere\n");

	
	(proc_Q + proc_end)->sno         =active->sno;        
	(proc_Q + proc_end)->pid         =active->pid;        
	(proc_Q + proc_end)->ppid        =active->ppid ;      
	(proc_Q + proc_end)->entry_point =active->entry_point;
	(proc_Q + proc_end)->u_stackbase =active->u_stackbase;
	(proc_Q + proc_end)->k_stackbase =active->k_stackbase;
	(proc_Q + proc_end)->cr3         =active->cr3 ;       
	(proc_Q + proc_end)->mmstruct    =active->mmstruct;   
	(proc_Q + proc_end)->heap_top    =active->heap_top;   
	(proc_Q + proc_end)->next        = proc_Q + proc_end + 1;     
	
	(proc_Q + proc_end)->k_stack=(uint64_t)reg;
	(proc_Q + proc_end)->u_stack=reg->usersp;
	(proc_Q + proc_end)->state=1;
	
	proc_end=proc_end + 1;
	PCB *p_to=get_nextproc();
	active=p_to;
	//kprintf("old process user stack: %p\n",reg);
	//gpr_t *new_rg=(gpr_t *)active->k_stack;
	//kprintf("new process user stack: %p k_stack%p\n",new_rg->usersp,active->k_stack);
	
	//kprintf("new process cr3: %p\n",active->cr3);
	
	//kprintf("switching pid: %d\n",active->pid);
	//while(1);
	set_tss_rsp((void *)active->k_stack);
	
	change_ptable(active->cr3);
	
	
	//kprintf("new process user stack content %p",
	
	if(active->state==0)
	{
		
		
		__asm__(
		"pushq $0x23;\n"
		"pushq %0;\n"
		"pushf;\n"
		"pushq $0x1B;\n"
		"pushq %1;\n"
		"iretq;\n"
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
		"iretq;\n"
		:
		:"g"(active->k_stack)
		);
		return 0;
		
	}
	return 0;
}

uint64_t syscall_fork(gpr_t *reg)
{
	copy_parent_stack();
		

	
	//kprintf("parent cr3:%p\n",parent_cr3);
	
	if((proc_end + 1) % 101==proc_start)
	{
		kprintf("child can not be queued\n");
		while(1);
	}
	PCB *child;
	int i;
	for(i=0;i<100;i++)
	{
		if(proc_descriptor[i]==0)
			break;
	}
	child=&all_pro[i];
	proc_descriptor[i]=1;
	child->cr3=mappageTable();  //active->cr3;//to be changed later
	child->pid=global_pid%100;
	global_pid++;
	child->next=(PCB *)(child + 1);
	child->state=1;
	child->sno=i;
	child->ppid=active->pid;
	child->entry_point=active->entry_point;
	child->heap_top =active->heap_top;
	(child->mmstruct).vma_list=NULL;
	
	
	
	//while(1);
	change_ptable(child->cr3);
	
	create_kstack(child);
	copy_kstack(child);
	
	
	gpr_t *parent_rg,*child_rg;
	parent_rg=(gpr_t *)reg;
	
	active->k_stack=(uint64_t)reg;
	active->u_stack=(uint64_t)(parent_rg->usersp);
	//kprintf("Inside forks 1234\n");
	
	init_stack(child);
	reg->rax=child->pid;
	
	//proc_Q[proc_end]=active[0];
	(proc_Q + proc_end)->sno         =active->sno;        
	(proc_Q + proc_end)->pid         =active->pid;        
	(proc_Q + proc_end)->ppid        =active->ppid;       
	(proc_Q + proc_end)->entry_point =active->entry_point;
	(proc_Q + proc_end)->u_stackbase =active->u_stackbase;
	(proc_Q + proc_end)->k_stackbase =active->k_stackbase;
	(proc_Q + proc_end)->cr3         =active->cr3 ;       
	(proc_Q + proc_end)->mmstruct    =active->mmstruct;   
	(proc_Q + proc_end)->heap_top    =active->heap_top; 
	(proc_Q + proc_end)->next        = proc_Q + proc_end + 1; 
	(proc_Q + proc_end)->k_stack	 =active->k_stack;
	(proc_Q + proc_end)->u_stack	 =active->u_stack;
	(proc_Q + proc_end)->state	 =1;
	proc_end=proc_end + 1;
	//kprintf("parent kstack:%p ustack:%p\n",(proc_Q + proc_start)->k_stack,(proc_Q + proc_start)->u_stack);
	//kprintf("parent sp val:%p sp:%p\n",*(uint64_t *)parent_rg->usersp,parent_rg->usersp);
	
	
	uint64_t stackdiff=active->k_stackbase-(uint64_t)reg;
	child_rg=(gpr_t *)(child->k_stackbase - stackdiff);
	
	//kprintf("parent sp val:%p %\n",*(uint64_t *)child_reg->usersp,child_reg->usersp);
	
	
	child_rg->usersp=child->u_stack;

	//kprintf("child kstack:%p ustack:%p\n",child->k_stack,child->u_stack);
	
	child_rg->rax=0;
	//kprintf("child cr3: %p parent cr3: %p\n",child->cr3,active->cr3);
	active=child;
	//kprintf("parent kstack:%p ustack:%p\n",(proc_Q + proc_start)->k_stack,(proc_Q + proc_start)->u_stack);
	set_tss_rsp((void *)active->k_stack);
	//kprintf("Inside forks 1234\n");
	
	
	return child->pid;
}

void syscall_init()
{
	//kprintf("iiiiiioooo\n");
	p[0] = syscall_print;
	p[1] = k_mmap;
	p[2] = k_munmap;
	p[57]= syscall_fork;
	p[58] = syscall_switch;
}

