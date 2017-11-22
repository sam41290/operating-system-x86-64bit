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
<<<<<<< HEAD
#include<sys/phy_mem_manager.h>
#include<sys/virt_mem.h>
#include<sys/task_manager.h>
=======
#include <sys/paging.h>
#include <sys/phymem.h>
#include <sys/kmalloc.h>
#include <sys/process.h>
#include <test.h>
>>>>>>> 97ea6f72c88866cf48d49640c51dd28c54385334
//#include <sys/scanPCI.h>
#include <sys/paging.h>

#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
<<<<<<< HEAD
extern char *video;
extern char *videostart;
extern uint64_t kern_VA;
extern uint64_t kernbase;
extern uint64_t upml4;
extern uint64_t ustacktop;
extern uint64_t u_heapboundary;
=======
>>>>>>> 97ea6f72c88866cf48d49640c51dd28c54385334


void start(uint32_t *modulep, void *physbase, void *physfree)
{
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;

<<<<<<< HEAD


uint64_t kernend=(uint64_t)physfree + 4096 * 280;

 for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      kprintf("Available Physical Memory [%p-%p %p]\n", smap->base, smap->base + smap->length,smap->length);
      //uint64_t pagenum=smap->length/4096;
    freelist((uint64_t)smap->base, (uint64_t)smap->length,(uint64_t)&kernmem,(uint64_t)physbase,(uint64_t)physfree);
    }
  }
  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

map_kernpt((uint64_t)&kernmem,(uint64_t)physbase,(uint64_t)physfree,(uint64_t)kernend);


syscall_init();


init_proc();


  //checkAllBuses();

  //while(1){
=======
    struct smap_t* max_smap = NULL;max_smap->length=0;
    for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
      if (smap->type == 1 /* memory */ && smap->length != 0) {
        kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
        //uint64_t pagenum=smap->length/4096;

        //Currently using the biggest chunk as free memory
        if (max_smap->length < smap->length)
        {
            max_smap->base = smap->base;
            max_smap->length = smap->length;
        }
    }
  }



  kprintf("max_smap in [%p:%p]\n", max_smap->base+0x300000, max_smap->length);

  p_init(max_smap->base + 0x250000 , max_smap->length + max_smap->length, (uint64_t)physfree);

  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

  init_paging((uint64_t)&kernmem, (uint64_t)physbase);

  __asm__ __volatile__("movq %0, %%rsp" : :"a"(&initial_stack[INITIAL_STACK_SIZE]));

  //Uncomment this
  create_idle_task();
  test_schedule();
  schedule_idle_task();

  // kprintf("Display Enabled\n");

  // KMALLOC_TEST();
  while(1){
>>>>>>> 97ea6f72c88866cf48d49640c51dd28c54385334
    //Dont return from start
  //}
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
  
  //  init_gdt();
  //  init_idt();
  //  PIC_remap((int)0x20,(int)0x28);				//In protected mode, we need to set the master PIC's offset to 0x20 and the slave's to 0x28
  //  init_pit();									          // Reference : http://wiki.osdev.org/PIC
  //  MakeKeyboardMapping();
  // outb(0x21,0xfc);                      //Disable all interrupts in master PIC except IRQ0 and IRQ1
  // outb(0xa1,0xff);                      //Disable all interrupts in slave PIC
  //  __asm__("sti;");
 


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
