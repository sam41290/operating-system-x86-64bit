#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/time.h>
#include <sys/Utils.h>
#include <sys/idt.h>
#include <sys/error.h>

//---------------------------------------------------------
struct freelist_v2 {
int next;
int ref_count;
}__attribute__((packed));

static int totalblockcount = 0;
static struct freelist_v2 p_list[50000];
static int head_freelist = 0;
static int lastfreeblock = 0;
static int currblock = 0;
#define PAGE_SIZE 0x1000
#define KERNBASE 0xFFFFFFFF80200000

void printtotalFreeBlocks(){
	kprintf("Total Free Blocks %d\n", totalblockcount);
}

void flushTLB(){

	__asm__(
	"movq %%cr3,%%rax;\n"
	"movq %%rax,%%cr3"
	::
	);
	
}

uint64_t FREEMEMORY;

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
		if ((base <= (physfree + (1024*1024))))
        {
			base=base + 4096;
			FREEMEMORY = base;
            continue;                               // Taking care of kernel in memory & Video memory (does not map them in free list)
        }

		//TODO : memset all to 0
		// memset((void*)startPhysFree, 0, PAGE_SIZE);
        if (lastfreeblock == 0)
        {
        	lastfreeblock = currblock;
  			head_freelist = lastfreeblock;
        }
        else
        {
			p_list[lastfreeblock].next = currblock;
			p_list[lastfreeblock].ref_count = 0;
			lastfreeblock = currblock;
			totalblockcount++;
        }
        currblock++;
        base = base + 4096;
    }

    kprintf("totalfreeblockcount %d\n", totalblockcount);
	return 0;	
}

uint64_t get_page()
{
	if (totalblockcount < 5)
	{
		//out of memory
		ThrowOutOfMemoryError();
		return (uint64_t)-1;	
	}

	if (head_freelist == 0)
	{
		// Dont want to change the handling of get_page. It assumes 0 for fail case
		head_freelist = p_list[head_freelist].next;
		totalblockcount--;
	}

	int freeblock = head_freelist;
	head_freelist = p_list[head_freelist].next;
	totalblockcount--;

	p_list[freeblock].ref_count++;	
	// kprintf("returning %p\n", freeblock*PAGE_SIZE);
	return (uint64_t)(FREEMEMORY + freeblock*PAGE_SIZE);
}

int get_reference_count(uint64_t paddr){
	int blockindex =  (paddr >> 12 << 12 >> 12) - (FREEMEMORY >> 12 << 12 >> 12);
	
	return p_list[blockindex].ref_count;	
}

void increment_reference_count(uint64_t paddr){
	int blockindex =  (paddr >> 12 << 12 >> 12) - (FREEMEMORY >> 12 << 12 >> 12);
	
	p_list[blockindex].ref_count++;		
}

void decrement_reference_count(uint64_t paddr){
	int blockindex =  (paddr >> 12 << 12 >> 12) - (FREEMEMORY >> 12 << 12 >> 12);
	
	if (p_list[blockindex].ref_count < 1)
	{
		kprintf("Tring to decrement the reference count of %p below 0! Some BUG!\n", paddr);
		return;
	}
	p_list[blockindex].ref_count--;			
}

void free_page(uint64_t paddr){
	int blockindex =  (paddr >> 12 << 12 >> 12) - (FREEMEMORY >> 12 << 12 >> 12);

	p_list[blockindex].next = head_freelist;
	head_freelist = blockindex;
	totalblockcount++;
	//TODO : memset all to 0

	flushTLB();
	#ifdef DEBUG_MALLOC
	kprintf("%p freed!\n", paddr);
	#endif
}

//---------------------------------------------------------------------------
//Not in use currently
// struct freelist {
// uint64_t base;
// struct freelist *next;
// }__attribute__((packed));


// struct freelist *memstart=NULL;

// struct freelist *memend=NULL;


// struct freelist *freebase;

// uint64_t free_page_count=0,last_free_ind;
// uint64_t freelist_v2(uint64_t base, uint64_t length,uint64_t kernmem,uint64_t physbase,uint64_t physfree)
// {
// 	if(base == 0)
// 	{
// 		return 0;
// 	}
// 	uint64_t last_addr= base + length;
// 	while (base < last_addr)
//     {
//         if(base % 4096 != 0)
//         {
//             uint64_t mod = base % 4096;
//             base=(base-mod) + 4096;
//             continue;
//         }
// 		if ((base >= physbase && base <= (physfree + (1024*1024))) || (base >= 0xB8000 && base <= 0xBC096))
//         {
// 			base=base + 4096;
//             continue;                               // Taking care of kernel in memory & Video memory (does not map them in free list)
//         }
//         if (memend==NULL)
//         {
// 			memstart=(struct freelist *)(kernmem - physbase + physfree + 4096);
//         	memstart->next=NULL;
//             memstart->base=base;
//             memstart->next=NULL;
//             memend=memstart;
// 			freebase=memstart;
// 			free_page_count++;
//         }
//         else
//         {
//             memend->next=(struct freelist *)(memend + 1);
//             (memend->next)->base=base;
//             (memend->next)->next=NULL;
//             memend=memend->next;
// 			free_page_count++;
//         }
//         base = base + 4096;
//     }
// 	last_free_ind=free_page_count - 1;
// 	kprintf("total pages %d last page ind %d\n",free_page_count,last_free_ind);
// 	kprintf("memstart: %p startpage: %p\n",memstart,memstart->base);
// 	return 0;	
// }

// uint64_t get_page_v1()
// {
// 	if(last_free_ind==10000)
// 		kprintf("last 10 pages\n");
// 	if(memstart!=memend)
// 	{
// 		uint64_t page=memstart->base;
// 		memstart=memstart->next;
// 		//kprintf("memstart: %p\n",memstart->base);
// 		last_free_ind--;
// 		return page;
// 	}
// 	else {
// 		kprintf("memstart: %p %p memend: %p %d\n",memstart,memstart->base,memend,last_free_ind);
// 		return 0;
// 	}
// }
