#include<stdlib.h>
#include<stdio.h>

uint64_t strlen(const char *str);


int strcmp(const char *str1,const char *str2);
int strncmp(const char *s1, const char *s2, size_t n);
char** SplitString(char* str_copy, int* vecCount);

//kfree the return pointer after use
char* mystrtok(char *str, const char delim, int* pos);
