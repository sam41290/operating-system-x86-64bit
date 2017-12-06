#include<stdio.h>
#include<stdlib.h>
#include<string.h>



uint64_t string_to_int(char *str)
{	
	uint64_t num=0;
	
	for(int i=0;str[i]!='\0';i++)
	{
		num=10 * num + (int)(str[i]-'0');
	}
	
	return num;
}

int main(int argc,char *args[],char *envp[])
{
	uint64_t pid;
	
	
	int i=1;
	
	if(strcmp(args[1],"-9")==0)
		i++;
	
	for(;i<argc;i++)
	{
		pid=string_to_int(args[i]);
		
		if(pid==1)
		{
			printf("Kill pid %d: permission denied\n",pid);
			continue;
		}
		
		unsigned long syscallnumber = 53;
			
		__asm__(
		"movq %0, %%rax;\n"
		"movq %1, %%rdi;\n"
		"int $0x80;\n"
		: 
		: "m" (syscallnumber),"m"(pid)
		: "rax","rdi"
		);
	}

	return 0;
}