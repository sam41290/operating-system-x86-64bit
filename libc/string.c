#include <stdio.h>
#include <stdlib.h>

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
/*
ssize_t getline(char **lineptr, size_t *n, FILE *stream){
	return 0;
}*/