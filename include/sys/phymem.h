//Physical memory management

#ifndef _PHYMEM_H_
#define _PHYMEM_H_

#include <stdio.h>
#include <sys/defs.h>

#define PAGE_SIZE 0x1000
#define KERNBASE 0xFFFFFFFF80000000

struct freelist {
int next;
}__attribute__((packed));

void flushTLB();
uint64_t p_allocblock();
void p_freeblock(int blockindex);
void p_init(uint64_t startPhysFree, uint64_t endPhysFree, uint64_t physfree);
uint64_t get_physfreebase();


#endif