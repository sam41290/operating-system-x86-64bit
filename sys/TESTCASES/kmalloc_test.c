#if 0 

#include <sys/kprintf.h>
#include <sys/kmalloc.h>
extern uint64_t virtualMemoryAvailable;

void KMALLOC_TEST(){

  kprintf("Available virtual mem %p\n", virtualMemoryAvailable);
  kprintf("size of int %d\n", sizeof(int));
  uint64_t* intmem = kmalloc(4096*2);
  for (int i = 0; i < 512+512; ++i)
  {
    intmem[i] = i;
  }

  for (int i = 0; i < 512+512; ++i)
  {
    kprintf("%d ", intmem[i]);
  }

  kprintf("Available virtual mem %p\n", virtualMemoryAvailable);

  uint64_t* charmem = kmalloc(26*sizeof(char));
  for (int i = 0; i < 26; ++i)
  {
  	charmem[i] = 'a' + i;
  }

  for (int i = 0; i < 26; ++i)
  {
  	kprintf("%c ", charmem[i]);
  }

}

#endif