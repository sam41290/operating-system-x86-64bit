#include<sys/kprintf.h>
#include <sys/phy_mem_manager.h>
#include <sys/virt_mem.h>
#include <sys/idt.h>

uint64_t (*p[200])(gpr_t reg);
extern PCB *active;


uint64_t dead(gpr_t reg){

	//Get the address from the register to free
	kprintf("R %d, %d, %d, %d, %d, %d %d\n", reg.rax, reg.rdi,reg.rsi, reg.rdx, reg.r10, reg.r9, reg.r8);
	kprintf("Dead called\n");
	return 12653712;
}

uint64_t k_mmap(gpr_t reg){

	//TODO: Get values from registers
	void *addr = (void*)reg.rdi;
	uint64_t length = reg.rsi;
	int prot = reg.rdx;
	int flags = reg.r10;
    int fd = reg.r9; 
    int offset = reg.r8;

	kprintf("addr %d length %d prot %d, flags %p, fd %d, offset %d", addr, length, prot, flags, fd, offset);

	//call mmap
	//Change this later
	uint64_t nextAvailHeapMem = active->heap_top;
	active->heap_top += PAGE_SIZE;

	vma* anon_vma = alloc_vma(nextAvailHeapMem, nextAvailHeapMem+PAGE_SIZE);
	append_to_vma_list(active, anon_vma);

	kprintf("malloc %p\n", nextAvailHeapMem);
	return nextAvailHeapMem;
}

uint64_t k_munmap(gpr_t reg){

	//Get the address from the register to free

	uint64_t addrToFree = (uint64_t)reg.rdi;
	// uint64_t length = reg.rsi;
	kprintf("Requested %p to free \n", addrToFree);


	//TODO remove entry from vma
	//Remove entry from pagetable and clean page
	unmap_phyaddr(addrToFree);

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

