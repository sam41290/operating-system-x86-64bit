#include <sys/idt.h>
//Reference : http://wiki.osdev.org/IDT_problems#Problems_with_IDTs, /Project/sys/gdt.c

//Function pointers
void isr_wrapper();
void timer_fptr();
void key_fptr();
void page_fault(void);
void page_fault2(void);
void _syscall();
void zero_excep();

// void (*fun_ptr)()=&isr_wrapper;
void (*keyboard)()=&key_fptr;
void (*timer)()=&timer_fptr;
void (*pagefault)()=&page_fault;
void (*pagefault2)()=&page_fault2;
void (*_sys_call)()=&_syscall;
void (*_zero_excep)()=&zero_excep;

void init_idt()
{
	
	{
        IDT[0].offset_1= (uint64_t)_zero_excep & (uint64_t)0xFFFF;
        IDT[0].offset_2= (uint64_t)(_zero_excep)>>16 & (uint64_t)0xFFFF;
        IDT[0].offset_3= (uint64_t)_zero_excep>>32 & (uint64_t)0xFFFFFFFF;
        IDT[0].ist=0;
        IDT[0].zero=0;
        IDT[0].type_attr=(uint8_t)0x8E;
        IDT[0].selector=8;
    }

	//PAGE FAULT
	{
        IDT[13].offset_1= (uint64_t)pagefault2 & (uint64_t)0xFFFF;
        IDT[13].offset_2= (uint64_t)(pagefault2)>>16 & (uint64_t)0xFFFF;
        IDT[13].offset_3= (uint64_t)pagefault2>>32 & (uint64_t)0xFFFFFFFF;
        IDT[13].ist=0;
        IDT[13].zero=0;
        IDT[13].type_attr=(uint8_t)0x8E;
        IDT[13].selector=8;
    }
	
	{
        IDT[14].offset_1= (uint64_t)pagefault & (uint64_t)0xFFFF;
        IDT[14].offset_2= (uint64_t)(pagefault)>>16 & (uint64_t)0xFFFF;
        IDT[14].offset_3= (uint64_t)pagefault>>32 & (uint64_t)0xFFFFFFFF;
        IDT[14].ist=0;
        IDT[14].zero=0;
        IDT[14].type_attr=(uint8_t)0x8E;
        IDT[14].selector=8;
    }


	//TIMER
	{
		IDT[32].offset_1= (uint64_t)timer & (uint64_t)0xFFFF;
		IDT[32].offset_2= (uint64_t)(timer)>>16 & (uint64_t)0xFFFF;
		IDT[32].offset_3= (uint64_t)timer>>32 & (uint64_t)0xFFFFFFFF;
		IDT[32].ist=0;
		IDT[32].zero=0;
		IDT[32].type_attr=(uint8_t)0x8E;
		IDT[32].selector=8;
	}

	// KEYBOARD
	{
		IDT[33].offset_1= (uint64_t)keyboard & (uint64_t)0xFFFF;
		IDT[33].offset_2= (uint64_t)(keyboard)>>16 & (uint64_t)0xFFFF;
		IDT[33].offset_3= (uint64_t)keyboard>>32 & (uint64_t)0xFFFFFFFF;
		IDT[33].ist=0;
		IDT[33].zero=0;
		IDT[33].type_attr=(uint8_t)0x8E;
		IDT[33].selector=8;  
	}

	// SYSCALL
	{
		IDT[0x80].offset_1= (uint64_t)_sys_call & (uint64_t)0xFFFF;
		IDT[0x80].offset_2= (uint64_t)(_sys_call)>>16 & (uint64_t)0xFFFF;
		IDT[0x80].offset_3= (uint64_t)_sys_call>>32 & (uint64_t)0xFFFFFFFF;
		IDT[0x80].ist=0;
		IDT[0x80].zero=0;
		IDT[0x80].type_attr=(uint8_t)0xEE;
		IDT[0x80].selector=8;  
	}
		
		
	idtr.size=sizeof(IDT);
	idtr.addr=(uint64_t)IDT;

	__asm__(
	"lidt %0;"
	:
	: "m"(idtr)
	);

}
