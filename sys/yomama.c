#include <sys/kprintf.h>
#include <sys/Utils.h>
#include<sys/phy_mem_manager.h>
#include<sys/paging.h>
#include <sys/virt_mem.h>
#include <sys/idt.h>
#include <sys/error.h>

extern uint64_t (*p[200])(gpr_t reg);
extern uint64_t RING0_MODE;
void sys_call()
{
	gpr_t reg;

	__asm__(
	"movq %%rax,%0;\n"
	"movq %%rbx,%1;\n"
	"movq %%rcx,%2;\n"
	"movq %%rdx,%3;\n"
	"movq %%rsi,%4;\n"
	"movq %%rdi,%5;\n"
	"movq %%rbp,%6;\n"
	"movq %%r8,%7;\n"
	"movq %%r9,%8;\n"
	"movq %%r10,%9;\n"
	"movq %%r11,%10;\n"
	"movq %%r12,%11;\n"
	"movq %%r13,%12;\n"
	"movq %%r14,%13;\n"
	"movq %%r15,%14;\n"
	:"=g"(reg.rax), "=g"(reg.rbx), "=g"(reg.rcx), "=g"(reg.rdx), "=g"(reg.rsi), "=g"(reg.rdi), "=g"(reg.rbp), "=g"(reg.r8), "=g"(reg.r9),"=g"(reg.r10), "=g"(reg.r11), "=g"(reg.r12), "=g"(reg.r13), "=g"(reg.r14), "=g"(reg.r15)
	);

	if((int)reg.rax < 0 || (int)reg.rax >= 200)
	{
		kprintf("Invalid syscall number or Not Mapped");
		return;
	}
	RING0_MODE = 1;

	uint64_t ret = p[(int)reg.rax](reg);
	reg.rax = ret;
	RING0_MODE = 0;

	__asm__(
	"movq %0, %%rax;\n"
	"movq %1, %%rbx;\n"
	"movq %2, %%rcx;\n"
	"movq %3, %%rdx;\n"
	"movq %4, %%rsi;\n"
	"movq %5, %%rdi;\n"
	"movq %6, %%rbp;\n"
	"movq %7,%%r8;\n"
	"movq %8, %%r9;\n"
	"movq %9, %%r10;\n"
	"movq %10, %%r11;\n"
	"movq %11, %%r12;\n"
	"movq %12, %%r13;\n"
	"movq %13, %%r14;\n"
	"movq %14, %%r15;\n"
	::"g"(reg.rax), "g"(reg.rbx), "g"(reg.rcx), "g"(reg.rdx), "g"(reg.rsi), "g"(reg.rdi), "g"(reg.rbp), "g"(reg.r8), "g"(reg.r9),"g"(reg.r10), "g"(reg.r11), "g"(reg.r12), "g"(reg.r13), "g"(reg.r14), "g"(reg.r15)
	);

	return;
}


void yomama(void)
{
	kprintf("YoMAMA");
}

int time=0;
int frac_time=0;

void timemama(void)
{
	frac_time=frac_time + 50;

	if (frac_time==1000)
	{
		time=time+frac_time;
		frac_time=0;

		//int ms=time%1000;
		int s=time/1000;

		flushtime(s);
	}
}

int ctr=0;

extern PCB *active;
void pagemama(void)
{
	//ctr ++;
	

	uint64_t addr;

	__asm__(
	"movq %%cr2,%0;\n"
	:"=g"(addr)
	:
	);
	// kprintf("page fault %p\n", addr);

	if (ctr > 0)
	{
		kprintf("address: %p\n",addr);
	    while(1);
	}
    //kprintf("address: %p\n",addr);

	if (RING0_MODE == 1)
	{
		// kprintf("There is no process yet!! page fault for %p\n", addr);
		// Thinking it is still in kernel mode
		if(map_phyaddr(addr) == -1)
		{
			kprintf("\nERROR: Can not allocate physical page for %p\n", addr);
			while(1);
		}		
		return;
	}

	//Check for address in process vma list
	if (IsPageInVmaList(active, addr) == 1)
	{
		//Page fault is valid as it had requested for malloc earlier.
		//Allocate physical page
		if(map_phyaddr(addr)==-1)
		{
			kprintf("\nERROR: Can not allocate physical page for %p\n", addr);
			while(1);	
		}		
	}
	else
	{
		ThrowSegmentationFault(addr);	
	}


	// else kprintf("\npage allocated\n");
	
	
}

void protection_fault()
{
     uint64_t cr2;
     __asm__(
         "movq %%cr2, %0"
         :"=g"(cr2)
         :
         :"memory"
     );
     ThrowSecurityError(cr2);
}

void keymama(void)
{
	// IRQ_set_mask('1');
	char c = getchar();
	if (c != '#')
	{
		if (getCtrlKeyStatus())
		{
			setCtrlKeyStatus(0);
			flushLastKeyPress('^', c);
		}
		else
		{
			flushLastKeyPress(c, ' ');
		}
	}
	
	// IRQ_clear_mask('1');
}
