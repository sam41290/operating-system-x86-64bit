#include <stdio.h>

int puts(const char *s)
{
  for( ; *s; ++s) {
  	putchar(*s);
  }
  return 0;
}
