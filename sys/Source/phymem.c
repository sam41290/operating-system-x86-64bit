#include <sys/phymem.h>
#include <sys/kprintf.h>
#include <sys/kmalloc.h>
static struct freelist p_list[50000];
static int head_freelist = 0;
static uint64_t PHYSFREEBASE;
static int freeblockcount = 0;

uint64_t get_physfreebase(){
	return PHYSFREEBASE;
}

void p_init(uint64_t startPhysFree, uint64_t endPhysFree, uint64_t physfree){

	PHYSFREEBASE = startPhysFree;
	while(startPhysFree < endPhysFree){
		//TODO : memset all to 0
		memset((void*)startPhysFree, 0, PAGE_SIZE);
		p_list[freeblockcount].next = freeblockcount+1;
		startPhysFree += 0x1000;
		freeblockcount++;
	}

	// kprintf("FREEhead %p", PHYSFREEBASE + head_freelist->next*PAGE_SIZE);
}


uint64_t p_allocblock(){
	if (freeblockcount < 5)
	{
		//out of memory
		return (uint64_t)-1;
	}

	int freeblock = head_freelist;
	head_freelist = p_list[head_freelist].next;
	freeblockcount--;


	return (uint64_t)(PHYSFREEBASE+freeblock*PAGE_SIZE);
}

void p_freeblock(int blockindex){

	p_list[blockindex].next = head_freelist;
	head_freelist = blockindex;
	freeblockcount++;
	//TODO : memset all to 0
}

