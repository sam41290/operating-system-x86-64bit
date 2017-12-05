#include<stdio.h>
#include<stdlib.h>

#define WAITING_TO_LIVE 1
#define WAITING_TO_DIE 2

#define NEW_PROC 0
#define RUNNING 1
#define ZOMBIE 2
#define DEAD 5

struct proc_lst
{
	uint64_t pid;
	uint64_t ppid;
	uint64_t state;
	uint64_t waitstate;
	char name[1024];
};

int main(int argc, char *args[],char *envp[])
{
	unsigned long syscallnumber = 54;
	
	int num;
	
	__asm__(
	"movq %1, %%rax;\n"
	"int $0x80;\n"
	"movq %%rax,%0;\n"
	: "=m"(num)
	: "m" (syscallnumber)
	: "rax"
	);
	
	struct proc_lst *plist=(struct proc_lst *)malloc(sizeof(struct proc_lst) *num);
	
	syscallnumber = 55;
	
	uint64_t addr=(uint64_t)plist;
	
	__asm__(
	"movq %0, %%rax;\n"
	"movq %1, %%rdi;\n"
	"int $0x80;\n"
	: 
	: "m" (syscallnumber),"m"(addr)
	: "rax","rdi"
	);
	
	printf("pid    ppid      state       name\n");
	printf("----------------------------------\n");
	
	char *states[]={"ACTIVE","WAITING","ZOMBIE"};
	
	for(int i=0;i<num;i++)
	{
		if(plist[i].waitstate==1 || plist[i].waitstate==2)
			printf("%d    %d      %s       %s\n",plist[i].pid,plist[i].ppid,states[1],plist[i].name);
		else if(plist[i].state==ZOMBIE)
			printf("%d    %d      %s       %s\n",plist[i].pid,plist[i].ppid,states[2],plist[i].name);
		else
			printf("%d    %d      %s       %s\n",plist[i].pid,plist[i].ppid,states[0],plist[i].name);

	}
	
	return 0;
}