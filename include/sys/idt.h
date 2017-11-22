

//Reference: http://wiki.osdev.org/Interrupt_Descriptor_Table


/*-----------------------------------------------------------------------------------------------

	1. Structure of idtr
	2. Structure of IDT
	3. init_idt() declaration [ Function to initialize IDT ]


-------------------------------------------------------------------------------------------------*/

#include <sys/defs.h>


#define IDT_IST           (0x0000)  /*** code segment descriptor ***/
#define IDT_RESERVED      (0x0000)  /*** data segment descriptor ***/
#define IDT_TYPE          (0x0E00)  /*** conforming ***/
#define IDT_ZERO          (0x0000)  /*** conforming ***/
#define IDT_DPL0          (0x0000)  /*** descriptor privilege level 0 ***/
#define IDT_DPL1          (0x2000)  /*** descriptor privilege level 1 ***/
#define IDT_DPL2          (0x4000)  /*** descriptor privilege level 2 ***/
#define IDT_DPL3          (0x6000)  /*** descriptor privilege level 3 ***/
#define IDT_P             (0x8000)  /*** present ***/

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
     unsigned char err_code;    // Interrupt number and error code (if applicable)
     //uint64_t ds;                  // Data segment selector
     //uint64_t rip, cs, eflags, usersp; // Pushed by the processor automatically.
} registers_t;

void init_idt();



