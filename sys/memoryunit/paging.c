#include <stdio.h>
#include <sys/paging.h>
#include <sys/phymem.h>
#include <sys/kprintf.h>

static uint64_t* gpml4_t = NULL; 
extern char *video;
extern char *videostart;
extern uint64_t virtualMemoryAvailable;

uint64_t get_pml4_index(uint64_t virtAddress){
	return ((virtAddress << 16) >> (9 + 9 + 9 + 12 + 16));
}
uint64_t get_directoryptr_index(uint64_t virtAddress){
	return ((virtAddress << (16 +9)) >> (9 + 9 + 12 + 16 + 9));	
}
uint64_t get_directory_index(uint64_t virtAddress){
	return ((virtAddress << (16 + 9 + 9)) >> (9 + 12 + 16 + 9 + 9));	
}
uint64_t get_pagetable_index(uint64_t virtAddress){
	return ((virtAddress << (16 + 9 + 9 + 9)) >> (12 + 16 + 9 + 9 + 9));
}

void map_virtual_to_physical(uint64_t vadd, uint64_t padd){
	uint64_t p1_index = get_pml4_index(vadd);
	uint64_t p2_index = get_directoryptr_index(vadd);
	uint64_t p3_index = get_directory_index(vadd);
	uint64_t p4_index = get_pagetable_index(vadd);
	
	// kprintf("p1=%d\n",p1_index);
	// kprintf("p2=%d\n",p2_index);
	// kprintf("p3=%d\n",p3_index);
	// kprintf("p4=%d\n",p4_index);


	uint64_t* pdpe_t, *pd_t, *pte_t;

	uint64_t pml4_e = *(uint64_t*)(gpml4_t + p1_index);		
	// kprintf("pml4_e %p ", pml4_e);

	if (pml4_e & 0x1)
	{
		pml4_e = pml4_e & 0xFFFFFFFFFFFFFFF8;
		pdpe_t = (uint64_t*)(pml4_e + KERNBASE);
		// kprintf("pdpe_t %p ", pdpe_t);
		uint64_t pdpe_e = (pdpe_t[p2_index]);
		// kprintf("pdpe_e %p ", pdpe_e);
		if (pdpe_e & 0x1)
		{
			pdpe_e = pdpe_e & 0xFFFFFFFFFFFFFFF8;
			pd_t = (uint64_t*)(pdpe_e + KERNBASE);
			uint64_t pd_e = *(uint64_t*)(pd_t + p3_index);
			// kprintf("pd_e %p ", pd_e);
			if (pd_e & 0x1)
			{
				pd_e = pd_e & 0xFFFFFFFFFFFFFFF8;
				pte_t = (uint64_t*)(pd_e + KERNBASE);
				pte_t[p4_index] = padd | 7;
			}
			else
			{
				// kprintf("Create 1 new tables\n");
				pte_t = (uint64_t*)p_allocblock();	
				pd_t[p3_index] = ((uint64_t)pte_t) | 7;
				pte_t += KERNBASE;								
			}
		}
		else
		{
			// kprintf("Create 2 new tables\n");
			pd_t = (uint64_t*)p_allocblock();
			pte_t = (uint64_t*)p_allocblock();		
			pdpe_t[p2_index] = ((uint64_t)pd_t) | 7;
			pd_t += KERNBASE;
			pd_t[p3_index] = ((uint64_t)pte_t) | 7;
			pte_t += KERNBASE;	
			// kprintf("pd_t : %p pte_t : %p\n", pd_t, pte_t);					
		}
	}
	else
	{
		// kprintf("Create 3 new tables\n");
		pdpe_t = (uint64_t*)p_allocblock();
		pd_t = (uint64_t*)p_allocblock();
		pte_t = (uint64_t*)p_allocblock();		
		gpml4_t[p1_index] = ((uint64_t)pdpe_t) | 7;
		pdpe_t += KERNBASE;
		pdpe_t[p2_index] = ((uint64_t)pd_t) | 7;
		pd_t += KERNBASE;
		pd_t[p3_index] = ((uint64_t)pte_t) | 7;
		pte_t += KERNBASE;		
	}

	pte_t[p4_index] = padd | 7;
	// kprintf("index of pte %p", pte_t+p4_index);
}

void map_virtual_to_physical2(uint64_t vadd, uint64_t padd){
	// uint64_t p1_index = get_pml4_index(vadd);
	uint64_t p1_index = vadd << 16 >> 55 << 3;			//Take 39-47 bits and multiply 8 (it will go as offset to self reference)
	uint64_t p2_index = vadd << 16 >> 46 << 3;			//Take 30-38 bits and multiply 8 (it will go as offset to self reference)
	uint64_t p3_index = vadd << 16 >> 37 << 3;			//Take 21-29 bits and multiply 8 (it will go as offset to self reference)
	uint64_t p4_index = vadd << 16 >> 28 << 3;			//Take 12-20 bits and multiply 8 (it will go as offset to self reference)
	// uint64_t p2_index = get_directoryptr_index(vadd);
	// uint64_t p3_index = get_directory_index(vadd);
	// uint64_t p4_index = get_pagetable_index(vadd);
	
	// kprintf("p1=%d\n",p1_index);
	// kprintf("p2=%d\n",p2_index);
	// kprintf("p3=%d\n",p3_index);
	// kprintf("p4=%d\n",p4_index);

	uint64_t* pml4_e = (uint64_t*)(p1_index | 0xFFFFFF7FBFDFE000);
	kprintf("pml4_e %p", *pml4_e);

	uint64_t* pdpe_e = (uint64_t*)(p2_index | 0xFFFFFF7FBFC00000);
	uint64_t* pd_e = (uint64_t*)(p3_index | 0xFFFFFF7F80000000);
	uint64_t* pt_e = (uint64_t*)(p4_index | 0xFFFFFF0000000000);
	// kprintf("address pdpe_e %p\n", pdpe_e);

	if (*pml4_e & 0x1)
	{
		kprintf("pdpe_e %p\n", *pdpe_e);
		if (*pdpe_e & 0x1)
		{
			kprintf("pd_e %p\n", *pd_e);
			if (*pd_e & 0x1)
			{
				kprintf("pt_e %p\n", *pt_e);
				if (*pt_e & 0x1)
				{
					kprintf("This location is already Mapped\n");
				}
				else
				{
					*pt_e = padd | 7;
				}
			}
			else
			{
				// kprintf("Creating pte_table\n");
				*pd_e = p_allocblock() | 7;
				*pt_e = padd | 7;
			}
		}
		else
		{
			// kprintf("Create 2 new tables\n");
			*pdpe_e = p_allocblock() | 7;
			*pd_e = p_allocblock() | 7;
			*pt_e = padd | 7;		
		}
	}
	else
	{
		// kprintf("Create 3 new tables\n");
		*pml4_e = p_allocblock() | 7;
		*pdpe_e = p_allocblock() | 7;
		*pd_e = p_allocblock() | 7;
		*pt_e = padd | 7;		
	}
}

void init_paging(uint64_t vadd, uint64_t physbase){
	if (gpml4_t == NULL)
	{
		//Allocate a table for global pml4 table
		gpml4_t = (uint64_t*)(KERNBASE+p_allocblock());
		gpml4_t[510] = (((uint64_t)gpml4_t) - KERNBASE) | 7;

		kprintf("gpml4_t %p", (((uint64_t)gpml4_t) - KERNBASE));		
	}

	uint64_t kernstart = physbase;
	uint64_t kernend = get_physfreebase();// + 4*PAGE_SIZE;

	for (int phys = kernstart; phys < kernend; )
	{
		map_virtual_to_physical(vadd, phys);
		vadd += PAGE_SIZE;
		phys += PAGE_SIZE;
	}

	uint64_t cr3 = ((uint64_t)gpml4_t - KERNBASE);
	kprintf("cr3 %p\n",cr3);
	
	//Map Video Card
	map_virtual_to_physical(0xFFFFFFFF800B8000, 0xB8000);

	//change cr3
	__asm__(
	"movq %0,%%rax;\n"
	"movq %%rax,%%cr3;\n"
	:
	:"g"(cr3)
	);

	virtualMemoryAvailable += 6*PAGE_SIZE;
	// map_virtual_to_physical2(0xFFFFFFFF800B9000, 0xB8000);
	// map_virtual_to_physical2(0xFFFFFFFF800B9000, 0xB8000);

	// uint64_t phys = p_allocblock();
	// uint64_t* start = (uint64_t*)(KERNBASE + p_allocblock());
	// kprintf("virtual %p\n", (uint64_t)start);
	// map_virtual_to_physical2((uint64_t)start, phys);

}
