//private repo
#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/time.h>
#include <sys/Utils.h>
#include <sys/idt.h>
#include<sys/phy_mem_manager.h>
#include<sys/virt_mem.h>
#include<sys/task_manager.h>
//#include <sys/scanPCI.h>
#include <sys/paging.h>
#include <sys/terminal.h>
#include <sys/tarfs.h>
#include <sys/vfs.h>


#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
extern char *video;
extern char *videostart;
extern uint64_t kern_VA;
extern uint64_t kernbase;
extern uint64_t upml4;
extern uint64_t ustacktop;
extern uint64_t u_heapboundary;
uint64_t RING_0_MODE=1;
extern inode* root_inode;


void start(uint32_t *modulep, void *physbase, void *physfree)
{
	

	//physfree=physfree + 4096 * 10;
	
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;



uint64_t kernend=(uint64_t)physfree + 4096 * 10;

 for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      kprintf("Available Physical Memory [%p-%p %p]\n", smap->base, smap->base + smap->length,smap->length);
      // uint64_t pagenum=smap->length/4096;
    freelist((uint64_t)smap->base, (uint64_t)smap->length,(uint64_t)&kernmem,(uint64_t)physbase,(uint64_t)physfree);
    }
  }
  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

map_kernpt((uint64_t)&kernmem,(uint64_t)physbase,(uint64_t)physfree,(uint64_t)kernend);

syscall_init();


init_tarfs();

init_proc();



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
  
   init_terminal();
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
