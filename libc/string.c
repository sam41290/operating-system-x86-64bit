#include <stdio.h>
#include <stdlib.h>
//Reference : https://filippo.io/linux-syscall-table/ + http://man7.org/linux/man-pages/man2/ + https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html

int strncmp(const char *s1, const char *s2, size_t n){
	
	for (int i = 0; i < n; ++i)
	{
		if (s1[i] == '\0' || s2[i] == '\0')
		{
			return -1;
		}
		if (s1[i] != s2[i])
		{
			return -1;
		}
	}
	return 0;
}

uint64_t strlen(const char* str){

	uint64_t length = 0;
	while(str[length] != '\0')
	{
		length++;
	}

	return length;
}
/*
ssize_t getline(char **lineptr, size_t *n, FILE *stream){
	return 0;
}*/
