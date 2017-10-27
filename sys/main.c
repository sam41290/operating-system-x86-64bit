//stealth
//private repo
#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/time.h>
#include <sys/Utils.h>
#include <sys/idt.h>
//#include <sys/scanPCI.h>
//#include <sys/paging.h>

#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
extern char *video;
extern char *videostart;

struct freelist {
uint64_t base;
struct freelist *next;
}__attribute__((packed));



void start(uint32_t *modulep, void *physbase, void *physfree)
{
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;


struct freelist *memstart=(struct freelist *)((uint64_t)&kernmem - (uint64_t)physbase 
						+ (uint64_t)physfree + 4096);

struct freelist *memend=NULL;
memstart->next=NULL;

 for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
      //uint64_t pagenum=smap->length/4096;



	uint64_t base=smap->base;
	uint64_t length=smap->length;
	uint64_t last_addr=base + length;
	while (base < last_addr)
	{
        	if(base % 4096 != 0)
		{
			uint64_t mod = base % 4096;
                	base=(base-mod) + 4096;
			continue;
		}
        	if (memend==NULL)
        	{
                	memstart->base=base;
			memstart->next=NULL;
                	memend=memstart;
        	}
        	else
        	{
			memend->next=(struct freelist *)(memend + 1);
                	(memend->next)->base=base;
			(memend->next)->next=NULL;
                	memend=memend->next;
        	}
        	base = base + 4096;
	}
    }
  }
  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

kprintf("number of pages %p\n",memend - memstart);
kprintf("page base: %p\n",memstart->base);
kprintf("page base: %p\n",memstart->next->base);
kprintf("page base: %p\n",(memend - 1)->base);
kprintf("page base: %p\n",memend->base);
//  struct freelist *tmp=memstart;
//  while (tmp != NULL)	{
//	kprintf("page base: %p\n",tmp->base);
//	tmp=tmp->next;
//	}

uint64_t *page_directory1=(uint64_t *)((uint64_t)&kernmem - (uint64_t)physbase
                                                + (uint64_t)physfree + 4096 * 256);
uint64_t *page_directory2=(uint64_t *)(uint64_t)(page_directory1 + 4096 * 9);

uint64_t *page_directory3=(uint64_t *)(uint64_t)(page_directory2 + 4096 * 9);

uint64_t *kernelpage=(uint64_t *)(uint64_t)(page_directory3 + 4096 * 9);
                                           
uint64_t kernstart=(uint64_t)physbase;
uint64_t kernend=((uint64_t)(kernelpage) + 4096 * 9) - ((uint64_t)&kernmem - (uint64_t)physbase);
kprintf("kernel start %p\n",kernstart);
kprintf("kernel end%p\n",kernend);

uint64_t vadd=(uint64_t)&kernmem;

uint64_t p1_index = (vadd << 16) >> (9 + 9 + 9 + 12 + 16);
uint64_t p2_index = (vadd << (16 +9)) >> (9 + 9 + 12 + 16 + 9);
uint64_t p3_index = (vadd << (16 + 9 + 9)) >> (9 + 12 + 16 + 9 + 9);
uint64_t p4_index = (vadd << (16 + 9 + 9 + 9)) >> (12 + 16 + 9 + 9 + 9);

kprintf("%p\n",p1_index);
kprintf("%p\n",p2_index);
kprintf("%p\n",p3_index);
kprintf("%p\n",p4_index);


page_directory1[p1_index]=(((uint64_t)page_directory2) - ((uint64_t)&kernmem - (uint64_t)physbase)) | 7;
page_directory2[p2_index]=(((uint64_t)page_directory3) - ((uint64_t)&kernmem - (uint64_t)physbase)) | 7;
page_directory3[p3_index]=(((uint64_t)kernelpage) - ((uint64_t)&kernmem - (uint64_t)physbase)) | 7;
int j=0;

while (kernstart <= kernend)
{
	kernelpage[p4_index + j]=kernstart | 3;
	j++;
	kernstart=kernstart + 4096;
}

page_directory1[510]=(((uint64_t)page_directory1) - ((uint64_t)&kernmem - (uint64_t)physbase)) | 7;




kprintf("%p\n",j);

kprintf("%p\n",kernelpage[0x1fc]);

uint64_t cr3 = (((uint64_t)page_directory1) - ((uint64_t)&kernmem - (uint64_t)physbase));

kprintf("cr3 %p\n",cr3);

char *cur_disp=getcurrdisp();

kernelpage[p4_index + j]=((uint64_t)cur_disp & 0xFFFFFFFFFF000) | 5;
kprintf("%p\n",kernelpage[0x1fc]);


kprintf("kernend: %p\n",kernend + ((uint64_t)&kernmem - (uint64_t)physbase));

__asm__(
"movq %0,%%rax;\n"
"movq %%rax,%%cr3;\n"
:
:"g"(cr3)
);
(video)=(char *)(0xffffffff803fc000 | ((uint64_t)video & 0xfff));

(videostart)=(char *)(0xffffffff803fc000);


//disp++;

//setcurrdisp(disp);

kprintf("Display Enabled\n");

//char a='a';
//kprintf("%c %p\n",a,&a);
  //checkAllBuses();

  while(1){
    //Dont return from start
  }
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp1, *temp2;

  for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
  __asm__ volatile (
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  
   init_gdt();
   init_idt();
   PIC_remap((int)0x20,(int)0x28);				//In protected mode, we need to set the master PIC's offset to 0x20 and the slave's to 0x28
   init_pit();									          // Reference : http://wiki.osdev.org/PIC
   MakeKeyboardMapping();
  // outb(0x21,0xfc);                      //Disable all interrupts in master PIC except IRQ0 and IRQ1
  // outb(0xa1,0xff);                      //Disable all interrupts in slave PIC
   __asm__("sti;");
 


  start(										
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1) __asm__ volatile ("hlt");
}
