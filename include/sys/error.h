#include <sys/defs.h>

void ThrowSegmentationFault(uint64_t addr);
void ThrowSecurityError(uint64_t addr);
void ThrowOutOfMemoryError();