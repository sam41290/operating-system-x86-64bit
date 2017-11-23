#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/time.h>
#include <sys/Utils.h>
#include <sys/idt.h>
#include<sys/phy_mem_manager.h>

extern char *video;
extern char *videostart;
extern uint64_t kern_VA;
extern uint64_t kernbase;
extern uint64_t upml4;
extern uint64_t ustacktop;
extern uint64_t u_heapboundary;


void map_kernpt(uint64_t kernmem,uint64_t physbase,uint64_t physfree,uint64_t kernend)
{
	uint64_t *page_directory1=(uint64_t *)((uint64_t)kernmem - (uint64_t)physbase
                                                + (uint64_t)physfree + 4096 * 260);
	uint64_t *page_directory2=(uint64_t *)(uint64_t)(page_directory1 + 4096);
	
	uint64_t *page_directory3=(uint64_t *)(uint64_t)(page_directory2 + 4096);
	
	uint64_t *kernelpage=(uint64_t *)(uint64_t)(page_directory3 + 4096);
	
	
											
	uint64_t kernstart=(uint64_t)physbase;
	
	uint64_t vadd=(uint64_t)kernmem;
	
	uint64_t p1_index = (vadd << 16) >> (9 + 9 + 9 + 12 + 16);
	uint64_t p2_index = (vadd << (16 +9)) >> (9 + 9 + 12 + 16 + 9);
	uint64_t p3_index = (vadd << (16 + 9 + 9)) >> (9 + 12 + 16 + 9 + 9);
	uint64_t p4_index = (vadd << (16 + 9 + 9 + 9)) >> (12 + 16 + 9 + 9 + 9);
	
	for(int i=0;i<512;i++)
	{
		page_directory1[i]=0x0;
		page_directory2[i]=0x0;
		page_directory3[i]=0x0;
		kernelpage[i]=0x0;
	}
	
	page_directory1[p1_index]=(((uint64_t)page_directory2) - ((uint64_t)kernmem - (uint64_t)physbase)) | 7;
	page_directory2[p2_index]=(((uint64_t)page_directory3) - ((uint64_t)kernmem - (uint64_t)physbase)) | 7;
	page_directory3[p3_index]=(((uint64_t)kernelpage) - ((uint64_t)kernmem - (uint64_t)physbase)) | 7;
	int j=0;
	
	kprintf("PML4: %p\n",page_directory1[511]);
	
	while (kernstart <= kernend)
	{
		kernelpage[p4_index + j]=kernstart | 3;
		j++;
		kernstart=kernstart + 4096;
	}
	
	page_directory1[510]=(((uint64_t)page_directory1) - ((uint64_t)kernmem - (uint64_t)physbase)) | 3;
	
	
	
	uint64_t cr3 = (((uint64_t)page_directory1) - ((uint64_t)kernmem - (uint64_t)physbase));
	
	char *cur_disp=getcurrdisp();
	
	kernelpage[p4_index + j]=((uint64_t)cur_disp & 0xFFFFFFFFFF000) | 7;
	
	
	__asm__(
	"movq %0,%%rax;\n"
	"movq %%rax,%%cr3;\n"
	:
	:"g"(cr3)
	);
	(video)=(char *)(((uint64_t)kernmem + 4096 * j) | ((uint64_t)video & 0xfff));
	
	(videostart)=(char *)((uint64_t)kernmem + 4096 * j);
	
	kern_VA=(uint64_t)kernmem + 4096 * (j+2);
	
	kernbase=(uint64_t)kernmem ;
	upml4=kernbase - 4096;
	ustacktop=upml4 -(4096 * 4096);
	
	return;
}



uint64_t map_phyaddr(uint64_t vadd)
{
	uint64_t p1_index = (vadd << 16) >> (9 + 9 + 9 + 12 + 16);
    uint64_t p2_index = (vadd << (16 +9)) >> (9 + 9 + 12 + 16 + 9);
    uint64_t p3_index = (vadd << (16 + 9 + 9)) >> (9 + 12 + 16 + 9 + 9);
    uint64_t p4_index = (vadd << (16 + 9 + 9 + 9)) >> (12 + 16 + 9 + 9 + 9);
	//if(man_mapping==1){
	//kprintf("p1 index %p\n",p1_index);
	//kprintf("p2 index %p\n",p2_index);
	//kprintf("p3 index %p\n",p3_index);
	//kprintf("p4 index %p\n",p4_index);}

	uint64_t *p1=NULL,*p2=NULL,*p3=NULL,*p4=NULL,*padd=NULL;

	uint64_t base=0xFFFF000000000000;

	base=(((base >> 48)<<9)|0x1FE)<<39;
	base=(((base >> 39)<<9)|0x1FE)<<30;
	base=(((base >> 30)<<9)|0x1FE)<<21;
	base=(((base >> 21)<<9)|0x1FE)<<12;

	p1=(uint64_t *)base;
	p2=(uint64_t *)p1[p1_index];
	
//	kprintf("p2 %p\n",p2);
	
	int newpg=0;
	if(!p2)
	{
		newpg=1;
		p2=(uint64_t *)get_page();
		if(p2==0)
			return -1;
		p1[p1_index]=((uint64_t)p2 & 0xFFFFFFFFFFFFF000) | 7;
	}

	base=(((base >> 48)<<9)|0x1FE)<<39;
    base=(((base >> 39)<<9)|0x1FE)<<30;
    base=(((base >> 30)<<9)|0x1FE)<<21;
    base=(((base >> 21)<<9)|p1_index)<<12;
	
	p2=(uint64_t *)base;

	if(newpg==1)
	{
		for(int i=0;i<512;i++)
			p2[i]=0x0;
		newpg=0;
			
	}
	
	
	p3=(uint64_t *)p2[p2_index];
	
//	kprintf("p3 %p\n",p3);

	if(!p3)
    {
		newpg=1;
        p3=(uint64_t *)get_page();
		if(p3==0)
			return -1;
        p2[p2_index]=((uint64_t)p3 & 0xFFFFFFFFFFFFF000) | 7;
    }

	base=(((base >> 48)<<9)|0x1FE)<<39;
    base=(((base >> 39)<<9)|0x1FE)<<30;
    base=(((base >> 30)<<9)|p1_index)<<21;
    base=(((base >> 21)<<9)|p2_index)<<12;
    
    p3=(uint64_t *)base;
	
	if(newpg==1)
	{
		for(int i=0;i<512;i++)
			p3[i]=0x0;
		newpg=0;
			
	}
	
    p4=(uint64_t *)p3[p3_index];

//	kprintf("p4 %p\n",p4);

    if(!p4)
    {
		newpg=1;
        p4=(uint64_t *)get_page();
		if(p4==0)
		return -1;
        p3[p3_index]=((uint64_t)p4 & 0xFFFFFFFFFFFFF000) | 7;
    }	

	base=(((base >> 48)<<9)|0x1FE)<<39;
    base=(((base >> 39)<<9)|p1_index)<<30;
    base=(((base >> 30)<<9)|p2_index)<<21;
    base=(((base >> 21)<<9)|p3_index)<<12;
    
    p4=(uint64_t *)base;
	
	if(newpg==1)
	{
		for(int i=0;i<512;i++)
			p4[i]=0x0;
		newpg=0;
			
	}
	
    padd=(uint64_t *)p4[p4_index];

	//if(man_mapping==1)
	//kprintf("padd %p\n",padd);
	//kprintf("padd %p\n",((uint64_t)padd & 0x1));

    if(((uint64_t)padd & 0x1)==0)
    {
		padd=(uint64_t *)get_page();
		if(padd==0)
			return -1;
		p4[p4_index]=((uint64_t)padd & 0xFFFFFFFFFFFFF000) | 7;

    }

	return (uint64_t)padd;
}
