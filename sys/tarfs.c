#include <stdio.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/kstring.h>
#include <sys/virt_mem.h>
#include <sys/task_manager.h>

int strcmp2(const char *str1,const char *str2)
{

	int i=0;
	//kprintf("compare %s | %s\n",str1,str2);
	for(i=0;str1[i]!='\0' && str2[i]!='\0';i++)
	{

		if(str1[i] ==str2[i])
			continue;
		else
		if(str1[i] <str2[i])
			return -1;
		else if (str1[i] >str2[i])
			return 1;
	}
	//kprintf("i=%d %c\n",i,str2[i]);
	if((str1[i] =='\0') && (str2[i]=='\0'))
		return 0;
	else if ((str1[i] =='\0') && (str2[i]!='\0'))
		return -1;
	else if ((str1[i] !='\0') && (str2[i]=='\0'))
		return 1;

	return 2;

}

int power(int x, int y)
{
        if( y == 0)
        {
                return 1;
        }
        else if (y%2 == 0)
        {
                return power(x, y/2)*power(x, y/2);
        }
        else
        {
                return x*power(x, y/2)*power(x, y/2);
        }

}

int oct_to_dec(char *num)
{
	int i=0;
	int oct=0;
	int ctr=0;	
	while(num[i]!='\0')
	{
		if(num[i]=='0' && oct==0)
		{
			i++;
			continue;
		}
		//kprintf("%c\n",num[i]);
		oct=10*oct + ((int)num[i]-(int)'0');
		ctr++;
		//kprintf("%d\n",oct);
		i++;
	}
	//kprintf("oct=%d\n",oct);
	int dec=0,rem;
	i=0;
	while (oct!=0)
        {
                rem = oct%10;
                oct/=10;
                dec += rem*power(8,i);
                ++i;
        }

        return dec;
}



void read_elf(char *elfaddr,PCB *proc)
{
	struct elf_header *elfhead=(struct elf_header *)elfaddr;
	//kprintf("program entry point: %p\n",elfhead->e_entry);
	
	proc->entry_point=elfhead->e_entry;
	
	char *offset=elfhead->e_phoff;
	//int i=0;
	uint64_t off = 0;
    uint64_t base = 1;
    for(int i = 0;i < 8 ; i++)
    {
			//kprintf("%d ",(uint64_t)offset[i]);
            off+=base*((uint64_t)offset[i]);
            base*=8;
    }
	//kprintf("\nheader offset: %d",off );
	
    // kprintf("num header %d", elfhead->e_phnum[0]);
	char *phead=(char *)(elfaddr + off);
	uint64_t head_count= (uint64_t)(*(elfhead->e_phnum)) + 10 * (uint64_t)(*((elfhead->e_phnum) + 1));
	//kprintf("head count: %d\n",head_count);
	
	uint64_t head_size=((uint64_t)(*(elfhead->e_phentsize)))+
						256*((uint64_t)(*((elfhead->e_phentsize)+1)));

	//TODO Check for head count if it is correct.
	for(int i=0;i<head_count-1;i++)
	{
		//kprintf("phead:%p\n",phead);
		struct pheader *p=(struct pheader *)phead;
		uint64_t filesz,memsz,vaddr,offset,location;
		filesz=p->p_filesz;
		memsz=p->p_memsz;
		vaddr=p->p_vaddr;
		offset=p->p_offset;
		location=(uint64_t)(elfaddr + offset);

		// kprintf("ptype : %d %d %d %d \n", (uint64_t)p->p_type[0],(uint64_t)p->p_type[1],(uint64_t)p->p_type[2],(uint64_t)p->p_type[3]);
		//Create vma for the segments
		
		//kprintf("vaddr %p\n", vaddr);
		vma* seg_vma = alloc_vma(vaddr, vaddr+memsz);
		// kprintf("seg_vma [%d - %d]\n", seg_vma->vstart, seg_vma->vend);
		append_to_vma_list(proc, seg_vma);
    	
    	//It will alsways write the last segment(data segment)
    	proc->heap_top = (uint64_t)((((seg_vma->vend) >> 12) + 1) << 12);

	
		file_map(vaddr,location,filesz,memsz);
		phead = phead + head_size;
	}
	//kprintf("Heap top %p\n", proc->heap_top);
	//kprintf("here out:%d\n",123);
	//struct pheader *phead=()()

	// if (proc->mmstruct.vma_list == NULL)
	// {
	// 	kprintf("Empty vma list\n");
	// }

	// for (vma* it = proc->mmstruct.vma_list; it != NULL; )
	// {
	// 	kprintf("vma [%d - %d]\n", it->vstart, it->vend);
	// 	it = it->nextvma;
	// }	

}


int scan_tarfs(PCB *proc,char *fname)
{
	struct posix_header_ustar *header =  (struct posix_header_ustar*)(&_binary_tarfs_start);
    struct posix_header_ustar *end =  (struct posix_header_ustar*)(&_binary_tarfs_end);
	int size=0;
    while(header < end)
    {
		if(strcmp2(header->name,fname)==0 && header->size[0]!='\0')
		{
			size=oct_to_dec(header->size);
			//kprintf("file found :%s size: %d||%s\n",header->name,size,header->size);
			read_elf((char *)(header + 1),proc);
			return 1;
		}
		size=oct_to_dec(header->size);
		if((size % sizeof(struct posix_header_ustar))==0)
		{
			header= header + 1 + (int)(size/sizeof(struct posix_header_ustar));
		}
		else
		{
			header= header + 2 + (int)(size/sizeof(struct posix_header_ustar));
		}
	}
	return 0;

}


