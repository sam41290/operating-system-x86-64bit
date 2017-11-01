#ifndef _PAGING_H_
#define _PAGING_H_

#include <stdio.h>
#include <sys/defs.h>

uint64_t get_pml4_index(uint64_t virtAddress);
uint64_t get_directoryptr_index(uint64_t virtAddress);
uint64_t get_directory_index(uint64_t virtAddress);
uint64_t get_pagetable_index(uint64_t virtAddress);
void init_paging(uint64_t vadd, uint64_t physbase);

#endif