#include <sys/kprintf.h>
#include <sys/Utils.h>
#include<sys/phy_mem_manager.h>
#include<sys/task_manager.h>
#include<sys/paging.h>
#include <sys/virt_mem.h>
#include <sys/idt.h>
#include <sys/error.h>
#include <sys/terminal.h>

extern uint64_t (*p[200])(gpr_t *reg);
extern uint64_t RING_0_MODE;

struct terminal terminal_for_keyboard;

void divide_by_zero()
{
	kprintf("Error:Divide by zero exception\n");
	return;
}


void sys_call(gpr_t *reg)
{
	RING_0_MODE=1;
	uint64_t syscallnum=reg->rax;
	if(syscallnum < 0 || syscallnum >= 200)
	{
		kprintf("Invalid syscall number or Not Mapped");
		return;
	}
	//kprintf("\nhere %d\n",syscallnum);
	uint64_t syscall_ret = p[(int)reg->rax](reg);
	reg->rax = syscall_ret;
	RING_0_MODE=0;
	//kprintf("\nhere %d\n",syscallnum);
	//while(1);
	return;
}


void yomama(void)
{
	kprintf("YoMAMA");
}

int time=0;
int frac_time=0;
int time_in_sec=0;

void timemama(void)
{
	frac_time=frac_time + 50;

	if (frac_time==1000)
	{
		time=time+frac_time;
		frac_time=0;

		//int ms=time%1000;
		time_in_sec=time/1000;

		flushtime(time_in_sec);
	}
}

int ctr=0;

extern PCB *active;
void pagemama(registers_t reg)
{
	//ctr ++;
	

	uint64_t addr;

	__asm__(
	"movq %%cr2,%0;\n"
	:"=g"(addr)
	:
	);
	#ifdef DEBUG_MALLOC
	kprintf("page fault %p\n", addr);
	#endif
	// kprintf("page fault %p\n", addr);


	if (ctr > 0)
	{
		kprintf("address: %p\n",addr);
	    while(1);
	}
    // kprintf("address: %p\n",addr);

	if (RING_0_MODE == 1)
	{
		// kprintf("RING_0_MODE\n");
		// kprintf("There is no process yet!! page fault for %p\n", addr);
		// Thinking it is still in kernel mode
		addr = (addr >> 12 << 12);	//Page Align for newly allocated page
		// kprintf("I am here\n");
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
		//check for cow
		
		int chk=0;
		if(reg.err_code==7)
		{
			// kprintf("Why am i entering cow\n");
			//kprintf("error code: %p\n",reg.err_code);
			//kprintf("cow called %p\n",addr);
			chk=check_cow(addr);
			if(chk==1)
				cow(addr);
		}
		else
		{
			//Page fault is valid as it had requested for malloc earlier.
			//Allocate physical page
			addr = (addr >> 12 << 12);	//Page Align for newly allocated page
			if(map_phyaddr(addr)==-1)
			{
				kprintf("\nERROR: Can not allocate phy &sical page for %p\n", addr);
				while(1);	
			}
		}
	}
	else
	{
		kprintf("error code: %p\n",reg.err_code);
		ThrowSegmentationFault(addr);
		
	}


	// kprintf("\npage allocated\n");
	
	
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

void WP2Keyboard(){

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

void keymama(void)
{
	terminal_for_keyboard.tunnel(&terminal_for_keyboard);
}
