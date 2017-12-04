#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Utilities.h"

struct envVar{
	char name[1024];
	char value[10240];
};
	char* readFilePath = "/etc/environment";


static int singleton = 0;
struct envVar** allVariables = NULL;
int envVar = 0;
struct envVar** getAllEnvironmentVaiables(int* av){
    puts("Need to implement getAllEnvironmentVaiables\n");
    return NULL;

	if (singleton == 0)
	{
		singleton = 1;
		allVariables = malloc(sizeof(struct envVar*)*100);
	}
	else
	{
		*av = envVar;
		return allVariables;
	}
	
	File* fp = fopen(readFilePath,"r");
	if (fp == NULL)
	{
		LOGG("cant open file");
	}
	char c;
	c = fgetc(fp);

	*av = 0;
	while(c != '\0' && c!= EOF && c!= '\n'){
		struct envVar* obj = malloc(sizeof(struct envVar));
	
		int n = 0;
		while(c != '='){
			obj->name[n++] = c;
			// LOG("%c", c);
			c = fgetc(fp);
		}
		obj->name[n] = '\0';

		c = fgetc(fp); int v = 0;
		while(c != '\n'){
			if (c == '"')
			{
				//Ignore it;
				c = fgetc(fp);
				continue;
			}
			obj->value[v++] = c;
			// LOG("%c", c);
			c = fgetc(fp);
		}
		obj->value[v] = '\0';
	
		// LOG("%s - %s", obj->name, obj->value);
		allVariables[(*av)++] = obj; 
		c = fgetc(fp);
	}

	fclose(fp);
	envVar = *av;
	return allVariables;
}

void print(struct envVar** allVariables, int count){

	for (int i = 0; i < count; ++i)
	{
		puts(allVariables[i]->name);
		puts(allVariables[i]->value);

	}
}

// void toiletflush(struct envVar** allVariables, int count){
// 	FILE* fp = fopen(writeFilePath,"w");

// 	for (int i = 0; i < count; ++i)
// 	{
// 		for (int j = 0; allVariables[i]->name[j] != '\0'; ++j)
// 		{
// 			fputc(allVariables[i]->name[j],fp);
// 		}
// 		fputc('=',fp);
// 		for (int j = 0; allVariables[i]->value[j] != '\0'; ++j)
// 		{
// 			fputc(allVariables[i]->value[j],fp);
// 		}		
// 		fputc(';',fp);
// 	}
// 	fclose(fp);
// }

int setenv(const char *name, const char *value, int overwrite){
        puts("Need to implement setenv\n");
        return 0;

	int count;
	struct envVar** allVariables = getAllEnvironmentVaiables(&count);
	// LOGG("Inside setenv");putchar(48+count);
	for (int i = 0; i < count; ++i)
	{
		if (strncmp(name, allVariables[i]->name, strlen(allVariables[i]->name)) == 0)
		{	
			LOGG("Setenv match");
			int j = 0;
			for (; value[j] != '\0'; ++j)
			{
				allVariables[i]->value[j] = value[j]; 
			}
			allVariables[i]->value[j] = '\0';
			//toiletflush(allVariables, count);
			return 1;
		}		
	}

	struct envVar* var = malloc(sizeof(struct envVar));
	int i;
	for (i = 0; name[i] != '\0'; ++i)
	{
		var->name[i] = name[i];
	}
	var->name[i]='\0';
	for (i = 0; value[i] != '\0'; ++i)
	{
		var->value[i] = value[i];
	}
	var->value[i]='\0';
	allVariables[count++] = var; 
	envVar=count;

	//Think to flush it to file! I dont think its required
	//toiletflush(allVariables, count);

	return 0;
}


char* getenv(const char *name){
        puts("Need to implement getenv\n");
        return NULL;

	LOGG("getenv");
	int count;
	struct envVar** allVariables = getAllEnvironmentVaiables(&count);
	// puts("Getenv");putchar(48+count);
		// print(allVariables, count);

	for (int i = 0; i < count; ++i)
	{
		if (strncmp(name, allVariables[i]->name, strlen(allVariables[i]->name)) == 0)
		{
		//	puts("Returning.................");
		//	puts(allVariables[i]->value);
			return allVariables[i]->value;
		}		
	}

	LOGG("Return null");
	return NULL;
}

