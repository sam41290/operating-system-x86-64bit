#include <stdio.h>
#include<sys/kprintf.h>
#include <sys/tarfs.h>
#include <string.h>
#include<sys/virt_mem.h>
#include<sys/task_manager.h>

int strcmp(const char *str1,const char *str2)
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
	
	char *phead=(char *)(elfaddr + off);
	uint64_t head_count= (uint64_t)(*(elfhead->e_phnum)) + 10 * (uint64_t)(*((elfhead->e_phnum) + 1));
	uint64_t head_size=((uint64_t)(*(elfhead->e_phentsize)))+
						256*((uint64_t)(*((elfhead->e_phentsize)+1)));
	for(int i=0;i<head_count;i++)
	{
		//kprintf("phead:%p\n",phead);
		struct pheader *p=(struct pheader *)phead;
		uint64_t filesz,memsz,vaddr,offset,location;
		filesz=p->p_filesz;
		memsz=p->p_memsz;
		vaddr=p->p_vaddr;
		offset=p->p_offset;
		location=(uint64_t)(elfaddr + offset);
		//kprintf("start add:%p\n",vaddr);
		//kprintf("start location:%p\n",location);
		//kprintf("filesz:%d\n",filesz);
		//kprintf("memsz:%d\n",memsz);
		file_map(vaddr,location,filesz,memsz);
		//kprintf("here:%d\n",i);
		phead = phead + head_size;
	}
	//kprintf("here out:%d\n",123);
	//struct pheader *phead=()()
	
}


void scan_tarfs(PCB *proc)
{
	struct posix_header_ustar *header =  (struct posix_header_ustar*)(&_binary_tarfs_start);
    struct posix_header_ustar *end =  (struct posix_header_ustar*)(&_binary_tarfs_end);
	int size=0;
    while(header < end)
    {
		if(strcmp(header->name,"bin/sbush")==0 && header->size[0]!='\0')
		{
			size=oct_to_dec(header->size);
			kprintf("file found :%s size: %d||%s\n",header->name,size,header->size);
			read_elf((char *)(header + 1),proc);
			return;
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

}



