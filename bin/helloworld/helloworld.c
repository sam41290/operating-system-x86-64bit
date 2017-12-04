#include<stdio.h>
int main(int argc, char *argv[], char *envp[])
{
printf("helllo world!!!!!!exec successfull\n");

printf("no of arguements: %d\n",argc);
for(int i=0;i<argc;i++)
	printf("first args: %s\n",argv[i]);

if(argv[2][0]=='2')
	//while(1);
	printf("hola!!\n");

return 0;
}
