#include <sys/idt.h>
//Reference : http://wiki.osdev.org/IDT_problems#Problems_with_IDTs, /Project/sys/gdt.c

//Function pointers
void isr_wrapper();
void timer_fptr();
void key_fptr();

// void (*fun_ptr)()=&isr_wrapper;
void (*keyboard)()=&key_fptr;
void (*timer)()=&timer_fptr;

void init_idt()
{


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



	idtr.size=sizeof(IDT);
	idtr.addr=(uint64_t)IDT;

	__asm__(
	"lidt %0;"
	:
	: "m"(idtr)
	);

}