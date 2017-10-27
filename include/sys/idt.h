

//Reference: http://wiki.osdev.org/Interrupt_Descriptor_Table


/*-----------------------------------------------------------------------------------------------

	1. Structure of idtr
	2. Structure of IDT
	3. init_idt() declaration [ Function to initialize IDT ]


-------------------------------------------------------------------------------------------------*/

#include <sys/defs.h>


struct idtr_t {
  uint16_t size;
  uint64_t addr;
}__attribute__((packed));

struct idtr_t idtr;

struct IDTDescr {
uint16_t offset_1;
uint16_t selector;
uint8_t ist;
uint8_t type_attr;
uint16_t offset_2;
uint32_t offset_3;
uint32_t zero;
}__attribute__((packed));

struct IDTDescr IDT[256];

void init_idt();
