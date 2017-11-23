
#define PAGE_SIZE 0x1000

uint64_t freelist(uint64_t base, uint64_t length,uint64_t kernmem,uint64_t physbase,uint64_t physfree);
uint64_t get_page();
void free_page();