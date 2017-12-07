#ifndef __PHY_MEM_MANAGER__
#define __PHY_MEM_MANAGER__

#define PAGE_SIZE 0x1000

uint64_t freelist(uint64_t base, uint64_t length,uint64_t kernmem,uint64_t physbase,uint64_t physfree);
uint64_t get_page();
void free_page();
void printtotalFreeBlocks();
int get_reference_count(uint64_t paddr);
void increment_reference_count(uint64_t paddr);
void decrement_reference_count(uint64_t paddr);

#endif