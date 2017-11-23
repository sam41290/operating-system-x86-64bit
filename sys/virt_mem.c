#include<sys/kprintf.h>
#include <sys/Utils.h>
#include <sys/idt.h>
//#include<sys/tarfs.h>

#define MAX_KERN 0xFFFFFFFFFFFFFFFF

uint64_t kern_VA;


uint32_t file_map(uint64_t vadd, uint64_t source_add,uint64_t fsize,uint64_t msize)
{
	if(fsize < 1)
		return(-1);
	char *start=NULL;
	char *source=NULL;
	start=(char *)vadd;
	source=(char *)source_add;
	//kprintf("test: %p\n",start + msize);
	for(int i=0;i <msize;i++)
	{
		/*fsize can be <= msize. when msize > fsize, the binary file contains .bss section(section for uninitialized variables).
		this section needs to be set to 0*/
		if(i>fsize)
		{
			*start++ = 0;
			//start++;
		}
		else
		{
			//kprintf("source=%s\n",source);
			*start++ = *source++;
			//start++;
			//source++;
		}
	}
		//kprintf("test: %p %p\n",start,source);
		

	return 0;
}

void *kmalloc(uint64_t byte_count)
{
	//kprintf("kern VA from kmalloc:%p\n",kern_VA);
	if(MAX_KERN < (kern_VA + byte_count) )
	{
		kprintf("Kernel Virtual address overshoot!!");
		return NULL;
	}
	uint64_t ret=kern_VA + 1;
	kern_VA+=byte_count;
	return (void *)ret;
}

vma* alloc_vma(uint64_t startAdd, uint64_t endAddr){

	vma* new_vma = (vma*)kmalloc(sizeof(vma));
	new_vma->vstart = startAdd;
	new_vma->vend = endAddr;
	new_vma->nextvma = NULL;
	return new_vma;
}

void append_to_vma_list(PCB* task, vma* vma_to_insert){
	
	if (task->mmstruct.vma_list == NULL)
	{
		task->mmstruct.vma_list = vma_to_insert;
		return;
	}

	vma* last_vma = task->mmstruct.vma_list;
	while(last_vma->nextvma != NULL)
	{
		last_vma = last_vma->nextvma;
	}
	last_vma->nextvma = vma_to_insert;
}

int IsPageInVmaList(PCB* task, uint64_t vaddr){
	
	for (vma* itr = task->mmstruct.vma_list; itr != NULL;)
	{
		if (vaddr >= itr->vstart && vaddr <= itr->vend)
		{
			return 1;
		}
		itr = itr->nextvma;
	}

	return 0;
}