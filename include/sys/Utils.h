#include <sys/defs.h>



// Reference : http://wiki.osdev.org/Inline_Assembly/Examples#I.2FO_access

void PIC_remap(int offset1, int offset2);
static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

static inline void io_wait(void)
{
    /* TODO: This is probably fragile. */
    __asm__ ( "jmp 1f\n\t"
                   "1:jmp 2f\n\t"
                   "2:" );
}


void IRQ_set_mask(unsigned char IRQline);
 
void IRQ_clear_mask(unsigned char IRQline);

char getKeyBoardKey(int key);
void MakeKeyboardMapping();
char getchar();
int getCtrlKeyStatus();
void setCtrlKeyStatus(int _s);

static inline void sysOutLong(uint16_t port, uint32_t val)
{
    __asm__ ( "outl %0, %1" : : "a"(val), "Nd"(port) );
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

static inline uint32_t sysInLong(uint16_t port)
{
    uint32_t ret;
    __asm__ ( "inl %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}