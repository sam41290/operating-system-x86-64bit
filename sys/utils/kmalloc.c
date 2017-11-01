// Kernel Malloc
#include <sys/virtualmem.h>
#include <sys/kprintf.h>
#include <sys/phymem.h>
#include <sys/paging.h>
extern uint64_t virtualMemoryAvailable;

//TODO implement free


void* kmalloc (uint64_t size){

	int pagesReq = size/PAGE_SIZE;
	pagesReq = (size%PAGE_SIZE==0)? pagesReq: pagesReq+1;
	uint64_t returnAddress = virtualMemoryAvailable;  		//TODO: Error Handling

	for (int i = 0; i < pagesReq; ++i)
	{
		uint64_t phyadd = p_allocblock();
		uint64_t virtadd = virtualMemoryAvailable;
		map_virtual_to_physical2(virtadd, phyadd);
		virtualMemoryAvailable += PAGE_SIZE;		
	}

	return (void*)returnAddress;
}
