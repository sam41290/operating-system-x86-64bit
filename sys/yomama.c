#include <sys/kprintf.h>
#include <sys/Utils.h>






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
