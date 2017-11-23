#include<sys/defs.h>

#ifndef _PAGING_H
#define _PAGING_H
uint64_t map_phyaddr(uint64_t vadd);
void unmap_phyaddr(uint64_t vadd);

void map_kernpt(uint64_t kernmem,uint64_t physbase,uint64_t physfree,uint64_t kernend);

#endif