#include <stdio.h>
//Reference : https://filippo.io/linux-syscall-table/ + http://man7.org/linux/man-pages/man2/ + https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html

int puts(const char *s)
{
  for( ; *s; ++s) {
  	putchar(*s);
  }
  return 0;
}
