.globl   key_fptr
.globl   isr_wrapper
.globl   timer_fptr
.align   4
 
isr_wrapper:
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rbp
    push %rsi
    push %rdi
    call yomama
    pop %rax
    pop %rbx
    pop %rcx
    pop %rdx
    pop %rbp
    pop %rsi
    pop %rdi
    iretq

timer_fptr:
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rbp
    push %rsi
    push %rdi
    call timemama
    mov $0x20, %al
    outb %al, $0x20
    pop %rax
    pop %rbx
    pop %rcx
    pop %rdx
    pop %rbp
    pop %rsi
    pop %rdi
    iretq


key_fptr:
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rbp
    push %rsi
    push %rdi
    call keymama
    mov $0x20, %al
    outb %al, $0x20
    pop %rax
    pop %rbx
    pop %rcx
    pop %rdx
    pop %rbp
    pop %rsi
    pop %rdi
    iretq