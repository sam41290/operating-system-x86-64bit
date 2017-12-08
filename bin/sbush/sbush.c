#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/defs.h>
#include <dirent.h>

char test[100];


void foolcompiler(int* a){
	return;
}

int dummysyscall(){

	unsigned long syscallnumber = 5;
	void *addr = NULL;
	uint64_t length = 12345;
	int prot = 1|2;//PROT_READ;
	int flags = 1|0x20;//MAP_SHARED;
    int fd = -1; int offset = 0;
    int ret=0;
	__asm__(
	"movq %0,%%rax;\n"
	"movq %1,%%rdi;\n"
	"movq %2,%%rsi;\n"
	"movq %3,%%rdx;\n"	
	"movq %4,%%r10;\n"	
	"movq %5,%%r9;;\n"
	"movq %6,%%r8;\n"
	"int $0x80;\n"
	:
	:"m"(syscallnumber),"m"((uint64_t)addr),"m"(length),"m"(prot),"m"(flags),"m"(fd),"m"(offset)
	);
    //printf("returned from syscall\n");

	__asm__(
	"movq %%rax, %0"
	:"=m"(ret)
	:
	);
	// printf("Returned From dummysyscall %d\n", ret);
	return ret;
}

int testidontknow(){

	unsigned long syscallnumber = 5;
		int ret = 0;

	// __asm__(
	// "movq %1,%%rax;\n"
	// "int $0x80;\n"
	// "movq %%rax, %0;\n"
	// :"=m" (ret)
	// :"m"(syscallnumber)
	// );	

	__asm__(
	"movq %0,%%rax;\n"
	"int $0x80;\n"
	:
	:"m"(syscallnumber)
	);	



	__asm__(
	"movq %%rax, %0;\n"
	:"=m" (ret)
	:
	);	

	return ret;
}

void TESTMALLOC(){

// {
// 	int* trymalloc = (int*)malloc(2048*sizeof(int));
// 	// printf("add malloc %p\n", trymalloc);	
// 	trymalloc[0] = 1;
// 	printf("malloc success %d\n", trymalloc[0]);
// 	trymalloc[2047] = 2;
// 	printf("malloc success %d\n", trymalloc[2047]);
//
//
// 	free(trymalloc);
//
//
// 	int* trymalloc2 = (int*)malloc(1000*sizeof(int));
// 	trymalloc2[0] = 1;
//
// 	int* trymalloc3 = (int*)malloc(1000*sizeof(int));
// 	trymalloc3[0] = 1;
//
// 	int* trymalloc4 = (int*)malloc(1000*sizeof(int));
// 	trymalloc4[0] = 1;
//
// 	free(trymalloc2);
// 	free(trymalloc3);
// 	free(trymalloc4);	
//
// }
//{
//	int* trymalloc = (int*)malloc(2048*sizeof(int));
//	// printf("add malloc %p\n", trymalloc);	
//	trymalloc[0] = 1;
//	puts("Working----------------------------------------");
//	printf("malloc success %d\n", trymalloc[0]);
//	trymalloc[2047] = 2;
//	printf("malloc success %d\n", trymalloc[2047]);
//
//
//	free(trymalloc);
//
//
//	int* trymalloc2 = (int*)malloc(1000*sizeof(int));
//	trymalloc2[0] = 1;
//
//	int* trymalloc3 = (int*)malloc(1000*sizeof(int));
//	trymalloc3[0] = 1;
//
//	int* trymalloc4 = (int*)malloc(1000*sizeof(int));
//	trymalloc4[0] = 1;
//
//	free(trymalloc2);
//	free(trymalloc3);
//	free(trymalloc4);	
//
//}

// {
// 	// 23557 blocks ~ 90+ MB
// 	int kill = 0;
// 	while(1){
// 		int* killme = (int*)malloc(1000*sizeof(int));
// 		killme[0] = 1;
// 		printf("Allocated %d %p %d\n", kill, killme, killme[0]);
// 		kill++;
// 	}
//
//}

// {
// 	//Allocated 545269 857f8000 545269
// 	int kill = 0;
// 	while(1){
// 		int* killme = (int*)malloc(1000*sizeof(int));
// 		killme[0] = kill;
// 		printf("Allocated %d %p %d\n", kill, killme, killme[0]);
// 		free(killme);
// 		kill++;
// 	}
// }

// {
	
// 	int kill = 0;
// 	while(1){
// 		int* killme = (int*)malloc(1000*sizeof(int));
// 		printf("Allocated %d %p\n", kill, killme);
// 		kill++;
// 	}

// }


// {
// 	//Page table for virtual address take space so not infinite. Need to reuse virtual address to make this work
// 	// Allocated 69093 8752b000
// 	int kill = 0;
// 	while(1){
// 		int* killme = (int*)malloc(8000*sizeof(int));
// 		killme[0] = 1;
// 		killme[1500] = 1;
// 		killme[2500] = 1;
// 		killme[3500] = 1;
// 		killme[4500] = 1;
// 		killme[5500] = 1;
// 		printf("Allocated %d %p\n", kill, killme);
// 		free(killme);
// 		kill++;
// 	}

// }

// {
// 		int kill = 0;
// 		int* killme = (int*)malloc(8000*sizeof(int));
// 		killme[0] = 1;
// 		killme[1500] = 1;		
// 		killme[2500] = 1;
// 		killme[3500] = 1;
// 		killme[4500] = 1;
// 		killme[5500] = 1;
// 		printf("Allocated %d %p\n", kill, killme);
// 		free(killme);
// 		kill++;
		
// }




	// trymalloc[0] = 2;
	// printf("result %d\n", trymalloc[0]);	

	// // trymalloc[1025] = 99;
	// // printf("result %d\n", trymalloc[1025]);	
	// free(trymalloc);

	// int* try2 = (int*)malloc(4096);
	// try2[0] = 1;

	// int* try3 = (int*)malloc(4096);
	// try3[0] = 1;

	// free(try2);	
}

void TESTCONTEXTSWITCH(){

	int a=5;
	 pid_t pid;
	 pid=fork();
	 if(pid > 0)
	 {
		 printf("I am parent %d\n",a);
		 yield();
		 printf("I am parent %d\n",a);
		 yield();
		 //printf("I am parent 1.0\n");
		 //yield();
		 //while(1);
		
	 }
	 if(pid==0)
	 {
		 printf("I am child %d\n",a);
		 a=a+1;
		 yield();
		 printf("I am child %d\n",a);
		 yield();
		 printf("I am child 1.1\n");
		 //while(1);
		 exit(0);
	 }
}

void TESTEXIT(){
	
	puts("testing exit\n");

	int a=5;
	pid_t pid;
	pid=fork();
	if(pid > 0)
	{
	 printf("I am parent %d\n",a);
	 yield();
	 printf("I am parent %d\n",a);
	 yield();
	 //printf("I am parent 1.0\n");
	 //yield();
	 //while(1);
	
	}
	if(pid==0)
	{
	 printf("I am child %d\n",a);
	 a=a+1;
	 yield();
	 printf("I am child %d\n",a);
	 yield();
	 printf("I am child 1.1\n");
	 exit(0);
	}
	printf("my child is dead %d\n",pid);
}

void TESTTERMINAL(){

	puts("Hi how are puts working fine! Enter some text \n");

	char* buff = (char*)malloc(1024*sizeof(char));
	gets(buff);

	puts(buff);

	puts("You entered ");
	puts(buff);
	puts("\n");
	free(buff);


	printf("Hi I am back to the good world\n");	
	// int* try = (int*)malloc(10*sizeof(int));
	// foolcompiler(try);	

	char* num = (char*)malloc(1024*sizeof(char));
	puts("Enter a number\n");
	gets(num);
	puts("You entered");puts(num);
	
}

void TESTVFS(){


	char* queryPath = "/";

    struct dirent *pDirent;
    DIR *pDir;


    pDir = opendir (queryPath);
    if (pDir == NULL) {
        puts ("Cannot open directory \n");
        return ;
    }

    while ((pDirent = readdir(pDir)) != NULL) {
        puts (pDirent->d_name);
        puts("\n");
    }
    // printf("readdir finished\n");
    closedir(pDir);
}

void TESTWAIT()
{
	printf("in wait\n");
	
	//while(1);
	int x=5;
	pid_t pid;
	pid=fork();
	if(pid > 0)
	{
		pid_t pid2=fork();
		if(pid2==0)
		{
			printf("I am child 2.0\n");
			yield();
			printf("I am child 2.1\n");
			yield();
			printf("I am child 2.2\n");
			yield();
			printf("I am child 2.3\n");
			yield();
			printf("I am child 2.4\n");
			sleep(5);
			printf("child 2 closing\n");
			exit(0);
		}
		int status;
		printf("I am parent..calling wait\n");
		pid_t cpid=waitpid(-1,&status, 0);
		printf("Child %d completed execution:status=%d\n",cpid,status);
		cpid=waitpid(-1,&status, 0);
		printf("Child %d completed execution:status=%d\n",cpid,status);
		printf("I am parent 1\n");
		yield();
		printf("I am parent 1.1\n");
		yield();
		printf("I am parent 1.2\n");
		//yield();
		//while(1);
	
	}
	if(pid==0)
	{
		printf("I am child 1\n");
		x=x+1;
		yield();
		printf("I am child 1.1\n");
		yield();
		printf("I am child 1.2\n");
		printf("child 1 closing\n");
		exit(0);
	}
	puts("my child is dead \n");
	//yield();
}


void TESTEXECVPE(int a)
{
	pid_t pid=fork();
	 
	if(pid==0)
	{
		yield();
		if(a==2)
		{
		char *args[]={"abcd","2",NULL};
		execvpe("bin/helloworld",args,NULL);}
		else
		{
		char *args[]={"abcd","1",NULL};
		execvpe("bin/helloworld",args,NULL);}
	 //execvpe: variable passing and path passing test pending
	}
	if(pid > 0)
	{
		int status;
		wait(&status);
	}
	printf("child execution complete\n");
	
	printf("execvpe: variable passing and path passing test pending\n");
}

void emti_test()
{
	int *str=malloc(sizeof(int)*2048);
	 
	 str[2047]=10;
	 
	 printf("%d",str[2047]);
	
}

void TESTFILE(){
	File* file = fopen("/Tryme/Piku.txt", "r");
	if (file == NULL)
	{
		printf("Fopen failed\n");
		return;
	}
	// printf("File Opened fd %d\n", file->fd);
	char* buff = (char*) malloc(4096*sizeof(char));
	int read_count = read(file->fd, buff, 9);
	for (int i = 0; i < read_count; ++i)
	{
		printf("%c", buff[i]);
	}

	fclose(file);
}

void TESTCHDIR(){

	chdir("Tryme");

	char* cwd = malloc(sizeof(char)*1024);
   	if (getcwd(cwd, 1024) != NULL){
   		printf("Current Dir%s\n", cwd);
   	}	

	char* queryPath = cwd;

    struct dirent *pDirent;
    DIR *pDir;


    pDir = opendir (queryPath);
    if (pDir == NULL) {
        puts ("Cannot open directory \n");
        return ;
    }

    while ((pDirent = readdir(pDir)) != NULL) {
        puts (pDirent->d_name);
        puts("\n");
    }
    // printf("readdir finished\n");
    closedir(pDir);


}

void test_ps()
{
	pid_t pid=fork();
	if(pid==0)
	{
		printf("I am the child!!!\n");
		exit(0);
	}
	pid_t pid2=fork();
	if(pid2==0)
	{
		execvpe("/bin/ps",NULL,NULL);
	}
	if(pid2>0)
	{
		int status;
		waitpid(pid2,&status,0);
		//execvpe(kill,pid,NULL);
	}
	pid_t pid3=fork();
	if(pid3==0)
	{
		
		char *myargs[]={"2",NULL};
		execvpe("/bin/kill",myargs,NULL);
	}
	pid_t pid4=fork();
	if(pid4==0)
	{
		execvpe("/bin/ps",NULL,NULL);
	}
	
}

struct try
{
	int a;
	int b;
}__attribute__((packed));

int ExecuteCAT2(char* path){
	pid_t pid=fork();
	 
	if(pid==0)
	{
		//yield();
		char* absPath = path;
		// printf("Abs Path %s\n", absPath);
		char *myArgs[]={absPath,NULL,NULL};

		execvpe("bin/cat",myArgs,NULL);
	 //execvpe: variable passing and path passing test pending
		free(absPath);
	}
	if(pid > 0)
	{
		// int status;
		// wait(&status);
	}

	return 0;
}

int ExecuteEcho2(char* data){
	pid_t pid=fork();
	 
	if(pid==0)
	{
		//yield();
		// printf("Abs Path %s\n", absPath);
		char *myArgs[]={data,NULL,NULL};

		execvpe("bin/echo",myArgs,NULL);
	 //execvpe: variable passing and path passing test pending
	}
	if(pid > 0)
	{
		int status;
		wait(&status);
	}

	return 0;
}

int ExecuteLS2(char* path){
	pid_t pid=fork();
	 
	if(pid==0)
	{
		//yield();
		// char* myArgs[1];
		// myArgs[0] = path;
		// myArgs[1] = NULL;

		 char *myArgs[]={path,NULL};
		 // printf("args addr %p %p\n",(uint64_t)myArgs,(uint64_t)myArgs[0]);

		  execvpe("bin/ls",myArgs,NULL);
		//TESTVFS();

	 //execvpe: variable passing and path passing test pending
	}
	if(pid > 0)
	{
		int status;
		wait(&status);
	}	

	return 0;
}
int main_test(int argc, char *argv[], char *envp[]) {

	puts("sbush> Hello World!!\n");
	
	for(int i=0;i<50;i++)
	{
		ExecuteLS2("/");
	}

	int *a=(int *)malloc(sizeof(int) * 5);
	
	a[0]=5;
	
	pid_t pid=fork();
	
	if(pid==0)
	{
		a[0]=a[0] + 1;
		printf("I am child %d\n",a[0]);
		exit(0);
	}
	if(pid > 0)
	{
		int status;
		wait(&status);
		printf("I am parent %d\n",a[0]);
	}
	//TESTWAIT();
	//TESTMALLOC();
	TESTFILE();
	while(1);
}
