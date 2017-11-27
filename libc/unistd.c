#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int chdir(char* in_path){

	unsigned long syscallnumber = 80;
	int ret = -1;

	__asm__(
		"movq %1, %%rax;\n"
		"movq %2, %%rdi;\n"
		"syscall;\n"
		"movq %%rax, %0;\n"
		: "=m" (ret)
		: "m" (syscallnumber), "m" (in_path)
		: "rax","rdi"
	);

	return ret;
}



// int chdir(char *path)
// {


// char *buff=path;

// unsigned long syscallnumber = 80;
// int read_count;

//   // write character to stdout

// __asm__(
// "movq %1, %%rax;\n"
// "movq %2, %%rdi;\n"
// "syscall;\n"
// "movq %%rax, %0;\n"
// : "=m" (read_count)
// : "m" (syscallnumber), "m" (buff)
// : "rax","rdi"
// );


// return read_count;

// }


char* getcwd(char *buf, unsigned long size){		//TODO change int size to size_t

	unsigned long syscallnumber = 79;
	char* ret = (char* )malloc(size);


	__asm__(
		"movq %1, %%rax;\n"
		"movq %2, %%rdi;\n"
		"movq %3, %%rsi;\n"
		"syscall;\n"
		"movq %%rax, %0;\n"
		: "=m" (ret)
		: "m" (syscallnumber), "m" (buf), "m" (size)
		: "rax","rdi", "rsi"
	);
	buf=ret;
	return buf;
}


int execve(const char *filename, char *const argv[], char *const envp[]){

	// shputs("execve:");shputs(filename);shputs(argv[0]);

	unsigned long syscallnumber = 59;
	int ret;

	__asm__(
		"movq %1, %%rax;\n"
		"movq %2, %%rdi;\n"
		"movq %3, %%rsi;\n"
		"movq %4, %%rdx;\n"
		"syscall;\n"
		"movq %%rax, %0;\n"
		: "=m" (ret)
		: "m" (syscallnumber), "m" (filename), "m" (argv), "m" (envp)
		: "rax","rdi", "rsi", "rdx"
	);

	return ret;
}

pid_t fork(void)
{
	unsigned long syscallnumber = 57;
	pid_t ret;

	__asm__(
		"movq %1, %%rax;\n"
		"int $0x80;\n"
		"movq %%rax, %0;\n"
		: "=m" (ret)
		: "m" (syscallnumber)
	);
	//printf("fork syscall complete %d\n",ret);
	return ret;
}

void yield(void)
{
	unsigned long syscallnumber = 58;
	//pid_t ret;

	__asm__(
		"movq %0, %%rax;\n"
		"int $0x80;\n"
		//"movq %%rax, %0;\n"
		: 
		: "m" (syscallnumber)
	);
	//printf("yield syscall complete %d\n");
	return ;
}

pid_t getpid(void){

	unsigned long syscallnumber = 39;
	pid_t ret;

	__asm__(
		"movq %1, %%rax;\n"
		"syscall;\n"
		"movq %%rax, %0;\n"
		: "=m" (ret)
		: "m" (syscallnumber)
		: "rax"
	);

	return ret;
}

pid_t wait(int *status){

	unsigned long syscallnumber = 61;
	
	int pid=-1;
	
	pid_t ret;

	__asm__(
		"movq %1, %%rax;\n"
		"movq %2, %%rdi;\n"
		"movq %3, %%rsi;\n"
		"int $0x80;\n"
		"movq %%rax, %0;\n"
		: "=m" (ret)
		: "m" (syscallnumber),"m"(pid),"m"(status)
		: "rax","rdi","rsi"
	);
	
	//printf("waited on %d\n",ret);

	return ret;	
}

int waitpid(int pid, int *status){
	
	//printf("calling wait: %p\n",status);
	
	unsigned long syscallnumber = 61;
	pid_t ret;

	__asm__(
		"movq %1, %%rax;\n"
		"movq %2, %%rdi;\n"
		"movq %3, %%rsi;\n"
		"int $0x80;\n"
		"movq %%rax, %0;\n"
		: "=m" (ret)
		: "m" (syscallnumber), "m" (pid), "m" (status)
		: "rax", "rdi", "rsi"
	);

	return ret;	

}

int pipe(int* pipefd){
	unsigned long syscallnumber = 22;
	int ret;

	__asm__(
		"movq %1, %%rax;\n"
		"movq %2, %%rdi;\n"
		"syscall;\n"
		"movq %%rax, %0;\n"
		: "=m" (ret)
		: "m" (syscallnumber), "m" (pipefd)
		: "rax", "rdi"
	);

	return ret;		
}

int dup2(int oldfd, int newfd){
	unsigned long syscallnumber = 33;
	int ret;

	__asm__(
		"movq %1, %%rax;\n"
		"movq %2, %%rdi;\n"
		"movq %3, %%rsi;\n"
		"syscall;\n"
		"movq %%rax, %0;\n"
		: "=m" (ret)
		: "m" (syscallnumber), "m" (oldfd), "m" (newfd)
		: "rax", "rdi", "rsi"
	);

	return ret;		
}

/*
int execvp(const char *file, char* argv[])
{
        // puts("Hello");
    char *path;
    path=getenv("PATH");
    // LOGG("execvp");LOGG( path);
    // puts(path);
    // int cmdind=0;
    char dir[1024];
    int k=0;
    int len=strlen(path);
//Go to each path defined in env and search the command

    for (int j = 0;j<=len; ++j)
    {
            if(path[j]!=':' && path[j]!='\0')
            {
                    dir[k++] = path[j];
                    continue;
            }
            else
            {
        //printf("a");
                    dir[k++]='/';
                    for(int l=0;file[l]!='\0';l++)
                    {
                            dir[k++]=file[l];
                    }
                    dir[k]='\0';
                    k=0;

                    //Run execve
                    // LOG("%s\n", dir);

                    argv[0] = dir;
                    execve(dir, argv, NULL);
                    //puts("Failed");

            }
    }
    return -1;
}
*/
