#include<sys/defs.h>

#ifndef _PAGING_H
#define _PAGING_H
uint64_t map_phyaddr(uint64_t vadd);
void unmap_phyaddr(uint64_t vadd);
void unmap_phyaddr_range(uint64_t vstartAddr, uint64_t vendAddr);
uint64_t map_userstack(uint64_t vadd);
void map_kernpt(uint64_t kernmem,uint64_t physbase,uint64_t physfree,uint64_t kernend);

uint64_t set_Cow(uint64_t vadd,uint64_t padd);
#endif