//Virtual Memory
#ifndef _VIRTUALMEM_
#define _VIRTUALMEM_

#include <stdio.h>
#include <sys/paging.h>
#include <sys/phymem.h>
#include <sys/kprintf.h>

uint64_t virtualMemoryAvailable = KERNBASE;

#endif