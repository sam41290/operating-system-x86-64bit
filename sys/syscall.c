#include<sys/kprintf.h>
#include <sys/phy_mem_manager.h>
#include <sys/virt_mem.h>
#include <sys/task_manager.h>
#include <sys/idt.h>
#include <sys/error.h>
#include<sys/kmalloc.h>
#include<sys/debug.h>
#include<sys/gdt.h>
#include <sys/terminal.h>
#include <sys/kmalloc.h>

uint64_t (*p[200])(gpr_t *reg);

extern PCB *active;
extern uint64_t proc_Q[101];
extern PCB all_pro[100];
extern uint64_t proc_descriptor[100];
extern int proc_start;
extern int proc_end;
extern uint64_t global_pid;
extern struct terminal terminal_for_keyboard;



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

uint64_t temporary_printf(gpr_t *reg)
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

	if(active!=NULL && active->waitstate!=1)
	{
		proc_Q[proc_end]=(uint64_t)active->sno;    
		active->k_stack=(uint64_t)reg;
		active->u_stack=reg->usersp;
		active->state=1;
		
		proc_end=(proc_end + 1)%101;
	}
	if(active->waitstate==1)
	{
		uint64_t kstack;
		
		__asm__(
		"movq %%rsp,%0;\n"
		:"=g"(kstack)
		:
		);
		active->k_stack=(uint64_t)kstack;
	}
	PCB *p_to=get_nextproc();
	active=p_to;
	set_tss_rsp((void *)active->k_stack);
	
	change_ptable(active->cr3);
		
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
		if(active->waitstate==1)
		{
			//kprintf("I am waiting on!!!!!\n");
			__asm__(
			"movq %0,%%rsp;\n"
			:
			:"g"(active->k_stack)
			);
			//while(1);
			return 0;
		}
	
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

uint64_t syscall_exit(gpr_t *reg)
{
	//kprintf("in exit\n");
	all_pro[active->sno].state=0;
	proc_descriptor[active->sno]=0;
	all_pro[active->sno].cr3=0;
	vma *last_vma=(active->mmstruct).vma_list;
	while(last_vma != NULL)
	{
		remove_from_vma_list(active,last_vma->vstart, last_vma->vend);
		last_vma = last_vma->nextvma;
	
	}
	//kprintf("exit status: %d\n",reg->rdi);
	uint64_t ppid=active->ppid;
	for(int i=0;i<100;i++)
	{
		if(all_pro[i].pid==ppid)
		{
			
			all_pro[i].sigchild_state=reg->rdi;
			all_pro[i].signalling_child=active->pid;
			break;
		}
	}
	
	active=NULL;
	
	//kprintf("calling switch\n");
	syscall_switch(reg);
	return 0;
}


void do_wait(gpr_t *reg)
{
	
	
	//kprintf("parent waiting:\n");
	proc_Q[proc_end]=(uint64_t)active->sno;
	
	active->u_stack=reg->usersp;
	active->state=1;
		
	proc_end=(proc_end + 1)%101;
	
	syscall_switch(reg);
	//kprintf("aahhaa\n");
	
}

uint64_t syscall_wait(gpr_t *reg)
{
	//kprintf("wait called\n");
	active->waitstate=1;
	active->waitingfor=reg->rdi;
	//int ctr=0;
	while(1)
	{
		do_wait(reg);
		if(active->sigchild_state==0 && (active->waitingfor==-1 || active->waitingfor==active->signalling_child))
		{
			active->waitstate=0;
			active->sigchild_state=5;
			break;
		}
		
		//ctr++;
		//if(ctr >= 4)
		//while(1);
	}
	//kprintf("wait complete\n");
	active->k_stack=(uint64_t)reg;
	active->u_stack=reg->usersp;
	set_tss_rsp((void *)active->k_stack);
	//while(1);
	reg->rax=active->signalling_child;
	//kprintf("rax value: %d\n",reg->rax);
	*((uint64_t *)reg->rdi)=active->sigchild_state;
	return active->signalling_child;
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
	create_new_process(i);
	child=&all_pro[i];
	child->state=1;
	child->ppid=active->pid;
	child->entry_point=active->entry_point;
	child->heap_top =active->heap_top;
	
	(child->mmstruct).vma_list=NULL;
	copy_vma(child);
	
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
	active->sigchild_state=child->state;
	proc_Q[proc_end]=(uint64_t)active->sno;
	proc_end=(proc_end + 1)%101;
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


uint64_t k_read(gpr_t *reg){
	// kprintf("Read request arrives\n");

	uint64_t fd = reg->rdi;
	if (fd == 0)
	{
		uint64_t buff = reg->rsi;
		return terminal_for_keyboard.read(&terminal_for_keyboard, (uint64_t)buff);
	}

	kprintf("We dont support file pointers other than stdin stdout!!\n");
	return 0;	
}

uint64_t k_write(gpr_t *reg){
	// kprintf("Read request arrives\n");
	uint64_t fd = reg->rdi;
	if (fd == 1)
	{
		uint64_t buff = reg->rsi;
		uint64_t count = reg->rdx;
		return terminal_for_keyboard.write(&terminal_for_keyboard, (uint64_t)buff, count);
	}

	kprintf("We dont support file pointers other than stdin stdout!!\n");
	return 0;
}

void syscall_init()
{
	//kprintf("iiiiiioooo\n");
	p[0] = k_read;
	p[1] = k_write;
	p[9] = k_mmap;
	p[11] = k_munmap;
	p[57]= syscall_fork;
	p[58] = syscall_switch;
	p[60] = syscall_exit;
	p[61]=syscall_wait;
	p[99] = temporary_printf;
}

