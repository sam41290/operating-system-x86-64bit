#ifndef _KMALLOC_H_
#define _KMALLOC_H_

void* kmalloc (uint64_t size);
void kfree(uint64_t vaddr, uint64_t byte_count);
void memset( void * ptr, int value, uint64_t num );
void memset64( void * ptr, int value, uint64_t num );
void memset8( void * ptr, int value, uint64_t num );
#endif