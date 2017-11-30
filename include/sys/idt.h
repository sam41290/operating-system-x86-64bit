

//Reference: http://wiki.osdev.org/Interrupt_Descriptor_Table


/*-----------------------------------------------------------------------------------------------

	1. Structure of idtr
	2. Structure of IDT
	3. init_idt() declaration [ Function to initialize IDT ]


-------------------------------------------------------------------------------------------------*/

#include <sys/defs.h>

#ifndef _IDT_H
#define _IDT_H
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

typedef struct registers
{
     uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rbp, rdi, rsi, rdx, rcx, rbx, rax; // Pushed by pushq i.e. all general purpose registers
     uint64_t err_code;    // Interrupt number and error code (if applicable)
} registers_t;

typedef struct gpr
{
     uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rbp, rdi, rsi, rdx, rcx, rbx, rax; // Pushed by pushq i.e. all general purpose registers
     uint64_t rip, cs, eflags, usersp,ss; // Pushed by the processor automatically.
} gpr_t;

void init_idt();

#endif



