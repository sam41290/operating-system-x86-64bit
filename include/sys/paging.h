<<<<<<< HEAD

#ifndef _PAGING_H
#define _PAGING_H
uint64_t map_phyaddr(uint64_t vadd);
void map_kernpt(uint64_t kernmem,uint64_t physbase,uint64_t physfree,uint64_t kernend);
=======
#ifndef _PAGING_H_
#define _PAGING_H_

#include <stdio.h>
#include <sys/defs.h>

uint64_t get_pml4_index(uint64_t virtAddress);
uint64_t get_directoryptr_index(uint64_t virtAddress);
uint64_t get_directory_index(uint64_t virtAddress);
uint64_t get_pagetable_index(uint64_t virtAddress);
void init_paging(uint64_t vadd, uint64_t physbase);
void map_virtual_to_physical_selfref(uint64_t vadd, uint64_t padd);
uint64_t get_CR3();
>>>>>>> 97ea6f72c88866cf48d49640c51dd28c54385334
#endif
