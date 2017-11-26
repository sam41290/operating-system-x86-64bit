
//Reference: http://wiki.osdev.org/Programmable_Interval_Timer


#include<sys/kprintf.h>


void init_pit()
{

unsigned int counter=59659;

unsigned int command=(unsigned int)0x34;    //command to initialize channel 0 in rate generator mode

__asm__(
"mov %0, %%al;\n"  		//storing 8 bit command on al
"outb %%al, $0x43;\n" 		//writing the command to 8bit command port
"mov %1,%%ax;\n"		//storing 16bit counter value
"outb %%al,$0x40;\n"		//writing 8 low bits onto the counter first
"mov %%ah, %%al;\n"
"outb %%al,$0x40;\n"		//writing 8 high bits onto the counter next
:
:"m"(command),"m"(counter)
);

}


void read_pit()
{
unsigned int counter;
unsigned int command=(unsigned int)0x0;		//command to initialize channel 0 for reading counter value
__asm__(
"mov %1,%%al;\n"				//storing the command in al
"outb %%al,$0x43;\n"				//writing the command onto the command register of PIT
"inb $0x40,%%al;\n"				//reading 8 low bits of counter value
"mov %%al,%%ah;\n"				
"inb $0x40,%%al;\n"				//reading 8 high bits of counter value
"rol $8,%%ax;\n"				//rearranging the low and high bits
"mov %%ax,%0;\n"		
:"=m"(counter)
:"m"(command)
);
kprintf("counter value:%d\n",counter);

}
