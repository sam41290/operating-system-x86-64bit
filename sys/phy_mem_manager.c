#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/time.h>
#include <sys/Utils.h>
#include <sys/idt.h>

struct freelist {
uint64_t base;
struct freelist *next;
}__attribute__((packed));


struct freelist *memstart=NULL;

struct freelist *memend=NULL;


struct freelist *freebase;

uint64_t free_page_count=0,last_free_ind;

uint64_t freelist(uint64_t base, uint64_t length,uint64_t kernmem,uint64_t physbase,uint64_t physfree)
{
	if(base == 0)
	{
		return 0;
	}
	uint64_t last_addr= base + length;
	while (base < last_addr)
    {
        if(base % 4096 != 0)
        {
            uint64_t mod = base % 4096;
            base=(base-mod) + 4096;
            continue;
        }
		if ((base >= physbase && base <= (physfree + (1024*1024))) || (base >= 0xB8000 && base <= 0xBC096))
        {
			base=base + 4096;
            continue;                               // Taking care of kernel in memory & Video memory (does not map them in free list)
        }
        if (memend==NULL)
        {
			memstart=(struct freelist *)(kernmem - physbase + physfree + 4096);
        	memstart->next=NULL;
            memstart->base=base;
            memstart->next=NULL;
            memend=memstart;
			freebase=memstart;
			free_page_count++;
        }
        else
        {
            memend->next=(struct freelist *)(memend + 1);
            (memend->next)->base=base;
            (memend->next)->next=NULL;
            memend=memend->next;
			free_page_count++;
        }
        base = base + 4096;
    }
	last_free_ind=free_page_count - 1;
	kprintf("total pages %d last page ind %d\n",free_page_count,last_free_ind);
	kprintf("memstart: %p startpage: %p\n",memstart,memstart->base);
	return 0;	
}

uint64_t get_page()
{
	if(last_free_ind==10000)
		kprintf("last 10 pages\n");
	if(memstart!=memend)
	{
		uint64_t page=memstart->base;
		memstart=memstart->next;
		//kprintf("memstart: %p\n",memstart->base);
		last_free_ind--;
		return page;
	}
	else {
		kprintf("memstart: %p %p memend: %p %d\n",memstart,memstart->base,memend,last_free_ind);
		return 0;
	}
}


