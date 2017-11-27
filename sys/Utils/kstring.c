#include <stdio.h>
#include <stdlib.h>
#include <sys/kmalloc.h>
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

char* mystrtok(char *str, const char delim, int* pos){

    int startFound = 0; int start = 0;
    int endFound = 0; int end = 0;
    int i = *pos;
    for (; str[i] != '\0'; ++i)
    {
        if (str[i] != delim && !startFound)
        {
            start = i; startFound = 1;
        }
        else if (str[i] == delim && startFound)
        {
            end = i-1; endFound = 1;
            break;
        }
    }
    if (startFound)
    {
        end = i-1; endFound = 1;
    }

    if (endFound)
    {
        char* token = kmalloc(sizeof(char)*end-start+5);
        int j = 0;
        for (int i = start; i <= end; ++i)
        {
            token[j++] = str[i];
        }
        token[j] = '\0';
        *pos = end+1;
        return token;
    }

    return NULL;

}


char** SplitString(char* str_copy, int* vecCount){

    char* str = kmalloc(sizeof(char)*1024);
    int j = 0;
    for (; str_copy[j] != '\0'; ++j)
    {
        str[j] = str_copy[j];
    }
    str[j] = '\0';

    char** vector = kmalloc(sizeof(char*)*20);       //TODO: Change this 2 to something good
    *vecCount = 0;

    // Returns first token 
    int i = 0;
    int pos = 0;
    char *token = mystrtok(str, ' ', &pos);
    vector[i++] = token;
    *vecCount = *vecCount+1;
    // printf("%s", token);

    // Keep printing tokens while one of the
    // delimiters present in str[].
    while (token != NULL)
    {
        // printf("%s\n", token);
        token = mystrtok(str, ' ', &pos);
        vector[i++] = token;
        *vecCount = *vecCount+1;
    }
    *vecCount = *vecCount-1;
    // printf("%d", *vecCount);
    // for (int i = 0; i < *vecCount; ++i)
    // {
    //     printf("%s ", vector[i] );
    // }
    return vector;
}
