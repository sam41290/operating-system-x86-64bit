#include <sys/error.h>
#include <sys/kprintf.h>

void ThrowSegmentationFault(uint64_t addr){
	kprintf("Trying to access %p, Segmentation Fault! Going to kill!\n", addr);
	//TODO Add exit later

	while(1);

}

void ThrowSecurityError(uint64_t addr){
     kprintf("Exception: General Protection Fault : %p\n", addr);

     while(1);

}

void ThrowOutOfMemoryError(){
	kprintf("DANGER! Out of Memory!!!\n");
	while(1);	
}