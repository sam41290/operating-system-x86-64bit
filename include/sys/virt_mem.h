
//Virtual Memory
#ifndef _VIRTUALMEM_
#define _VIRTUALMEM_

#include <stdio.h>
#include <sys/paging.h>
#include <sys/phymem.h>
#include <sys/kprintf.h>

uint32_t file_map(uint64_t vadd, uint64_t source_add,uint64_t fsize,uint64_t msize);
vma* alloc_vma(uint64_t startAdd, uint64_t endAddr);
void append_to_vma_list(PCB* task, vma* vma_to_insert);
int IsPageInVmaList(PCB* task, uint64_t vaddr);

#endif