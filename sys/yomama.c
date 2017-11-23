#include <sys/kprintf.h>
#include <sys/Utils.h>
#include<sys/phy_mem_manager.h>
#include<sys/paging.h>
#include<sys/idt.h>

extern int (*p[1])(void);

gpr_t sys_call(gpr_t reg)
{
	uint64_t syscallnum;
	
	
	__asm__(
	"movq %%rax,%0;\n"
	//"movq %%rcx,%1;\n"
	:"=g"(syscallnum)
	);
	uint64_t ret=0;
	if(syscallnum==0)
		ret=p[syscallnum]();
	reg.rax=ret;
	return reg;
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
	if(map_phyaddr(addr)==-1)
	{
		kprintf("\ncan not allocate physical page\n");
		while(1);
	}
	//else kprintf("\npage allocated\n");
	
	
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
