#include<sys/kprintf.h>
#include <sys/Utils.h>
#include <sys/idt.h>
#include<sys/paging.h>

#define MAX_KERN 0xFFFFFFFFFFFFFFFF

uint64_t kern_VA;

vma* vma_recycle_list = NULL;

void recyclevmastruct(vma* nodeToRemove){
	nodeToRemove->nextvma = NULL;

	if (vma_recycle_list == NULL)
	{
		vma_recycle_list = nodeToRemove;
		#ifdef DEBUG_MALLOC
		kprintf("Added vma to recycle\n");
		#endif
		return;
	}

	vma* last = vma_recycle_list;
	while(last->nextvma != NULL)
	{
		last = last->nextvma;
	}
	last->nextvma = nodeToRemove;
	#ifdef DEBUG_MALLOC
	kprintf("Added vma to recycle\n");
	#endif
}

vma* getvma_from_recylebin(){
	if (vma_recycle_list == NULL)
	{
		return NULL;
	}

	vma* freevma = vma_recycle_list;
	vma_recycle_list = vma_recycle_list->nextvma;

	#ifdef DEBUG_MALLOC
	kprintf("Reused vma\n");
	#endif

	return freevma;
}

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
		while(1);
		return NULL;
	}
	uint64_t ret=kern_VA + 1;
	kern_VA+=byte_count;
	return (void *)ret;
}


vma* alloc_vma(uint64_t startAdd, uint64_t endAddr){

	vma* new_vma = getvma_from_recylebin();
	if (new_vma == NULL)
	{
		new_vma = (vma*)kmalloc(sizeof(vma));
	}

	new_vma->vstart = startAdd;
	new_vma->vend = endAddr;
	new_vma->nextvma = NULL;
	// kprintf("Inside anon_vma %p %p\n",new_vma->vstart, new_vma->vend);
	return new_vma;
}


void walkthrough_vma_list(PCB* task){
	vma* last_vma = task->mmstruct.vma_list;
	while(last_vma != NULL)
	{
		kprintf("walk vma [%p - %p]\n", last_vma->vstart, last_vma->vend);
		last_vma = last_vma->nextvma;
	}
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



int remove_from_vma_list(PCB* task, uint64_t startAddr, uint64_t endAddr){
	
	if (task->mmstruct.vma_list == NULL)
	{
		return 0;
	}

	// kprintf("vma [%p - %p]\n", task->mmstruct.vma_list->vstart, task->mmstruct.vma_list->vend);
	//Remove headnode in a linked list
	if (task->mmstruct.vma_list->vstart <= startAddr && task->mmstruct.vma_list->vend >= endAddr)
	{
		vma* nodeToRemove = task->mmstruct.vma_list;
		task->mmstruct.vma_list = task->mmstruct.vma_list->nextvma;
		//TODO Either maintain a freelist or free this physical page of vma
		recyclevmastruct(nodeToRemove);

		//Free Physical page
		unmap_phyaddr_range(task->mmstruct.vma_list->vstart, task->mmstruct.vma_list->vend);
		return 1;
	}

	vma* current_vma = task->mmstruct.vma_list;
	vma* prev_vma = task->mmstruct.vma_list;
	while(current_vma != NULL)
	{	
		// kprintf("vma [%p - %p]\n", current_vma->vstart, current_vma->vend);
		//TODO maybe remove a part of vma later.
		//Currently removing whole vma 
		if (current_vma->vstart <= startAddr && current_vma->vend >= endAddr)
		{
			prev_vma->nextvma = current_vma->nextvma;
			//TODO Either maintain a freelist or free this physical page of vma
			recyclevmastruct(current_vma);

			//Free Physical page
			unmap_phyaddr_range(current_vma->vstart, current_vma->vend);
			return 1;
		}
		prev_vma = current_vma;
		current_vma = current_vma->nextvma;
	}

	kprintf("VMA Not Found \n");
	return 0;
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