#include <sys/kprintf.h>
#include <sys/kmalloc.h>


void KMALLOC_TEST(){


  uint64_t* intmem = kmalloc(200*sizeof(int));
  for (int i = 0; i < 200; ++i)
  {
    intmem[i] = i;
  }

  for (int i = 0; i < 200; ++i)
  {
    kprintf("%d ", intmem[i]);
  }

  kprintf("\n");

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