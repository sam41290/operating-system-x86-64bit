#include <sys/kprintf.h>
#include <sys/Utils.h>
#include<sys/phy_mem_manager.h>
#include<sys/paging.h>
#include <sys/virt_mem.h>
#include <sys/idt.h>
extern uint64_t (*p[200])(gpr_t *reg);

uint64_t sys_call(gpr_t *reg)
{
	if((int)reg->rax < 0 || (int)reg->rax >= 200)
	{
		kprintf("Invalid syscall number or Not Mapped");
		return 0;
	}
	//kprintf("\nhere\n");
	uint64_t ret = p[(int)reg->rax](reg);
	reg->rax = ret;

	return ret;
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
	
	//kprintf("page fault\n");

	uint64_t addr;

	__asm__(
	"movq %%cr2,%0;\n"
	:"=g"(addr)
	:
	);

	if (ctr > 0)
	{
		kprintf("address: %p\n",addr);
	    while(1);
	}
    //kprintf("address: %p\n",addr);

	if (active == NULL)
	{
		// kprintf("There is no process yet!! page fault for %p\n", addr);
		// Thinking it is still in kernel mode
		if(map_phyaddr(addr) == -1)
		{
			kprintf("\ncan not allocate physical page\n");
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
			kprintf("\ncan not allocate physical page\n");
			while(1);
		}		
	}
	else
	{
		kprintf("Trying to access %p, Segmentation Fault! Going to kill!\n", addr);
		while(1);
	}


	// else kprintf("\npage allocated\n");
	
	
}

void protection_fault()
{
     kprintf("Exception: General Protection Fault\n");
     uint64_t cr2;
     __asm__(
         "movq %%cr2, %0"
         :"=g"(cr2)
         :
         :"memory"
     );
	 kprintf("address: %p\n",cr2);
     while(1);
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
