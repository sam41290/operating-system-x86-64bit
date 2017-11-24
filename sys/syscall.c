#include<sys/kprintf.h>
#include <sys/phy_mem_manager.h>
#include <sys/virt_mem.h>
#include <sys/idt.h>
#include <sys/error.h>
#include<sys/kmalloc.h>
#include<sys/debug.h>

uint64_t (*p[200])(gpr_t reg);
extern PCB *active;


uint64_t dead(gpr_t reg){

	//Get the address from the register to free
	kprintf("R %d, %d, %d, %d, %d, %d %d\n", reg.rax, reg.rdi,reg.rsi, reg.rdx, reg.r10, reg.r9, reg.r8);
	kprintf("Dead called\n");
	return 12653712;
}


uint64_t k_mmap(gpr_t reg){

	// Get values from registers
	//TODO Use these variables
	uint64_t length = reg.rsi;
	
    #ifdef DEBUG_MALLOC
	void *addr = (void*)reg.rdi;
	int prot = reg.rdx;
	int flags = reg.r10;
    int fd = reg.r9; 
    int offset = reg.r8;
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

	#ifdef DEBUG_MALLOC
	kprintf("anon_vma %p %p active->heap_top %p\n", anon_vma->vstart, anon_vma->vend, active->heap_top);
	kprintf("malloc %p\n", nextAvailHeapMem);
	#endif

	#ifdef DEBUG_MALLOC
	printtotalFreeBlocks();
	#endif

	return nextAvailHeapMem;
}

uint64_t k_munmap(gpr_t reg){

	//Get the address from the register to free

	uint64_t addrToFree = (uint64_t)reg.rdi;
	uint64_t length = (uint64_t)reg.rsi;
	#ifdef DEBUG_MALLOC
	kprintf("Requested %p to free length %d\n", addrToFree, length);
	#endif

	// Remove entry from vma
	if(remove_from_vma_list(active, addrToFree, addrToFree+length) == 0)
	{
		ThrowSegmentationFault(addrToFree);
	}
	//Remove entry from pagetable and clean page
	#ifdef DEBUG_MALLOC
	printtotalFreeBlocks();
	#endif

	return 0;
}

uint64_t syscall_print(gpr_t reg)
{
	//kprintf("execute syscall %p\n",p[0]);
	uint64_t printval = reg.rbx;
	// __asm__(
	// "movq %%rbx,%0;\n"
	// :"=g"(printval)
	// );
	//kprintf("123::%p\n",printval);
	char *print=(char *)printval;
	//kprintf("123::%p\n",printval);
	//kprintf("user stack: %p re-entry point: %p rax:\n",reg.usersp,reg.rip);	
	kprintf("%s",print);

	return 0;
}

void syscall_init()
{
	//kprintf("iiiiiioooo\n");
	p[0] = syscall_print;
	p[1] = k_mmap;
	p[2] = k_munmap;
}

