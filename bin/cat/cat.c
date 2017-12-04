#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>

int main(int argc, char *argv[], char *envp[]) 
{

	if(argc > 2)
	{
		puts("Invalid number of arguements");
		return -1;
	}
	else if(argc==2)
	{

		File *fp=fopen(argv[1],"r");
		if(fp==NULL)
		{
			puts("file doesn't exist");
			return -1;
		}
		char ch=fgetc(fp);
		while(ch!=EOF)
		{
			putchar(ch);
			ch=fgetc(fp);
		}
	}
	else
	{
		char *str=(char*)malloc(sizeof(char)*1024);
		//size_t t=0;
			while(1)
			{
				gets(str);
				if(strcmp(str,"^C")==0)
					break;
				puts(str);

			}

	}

	return 0;

}

