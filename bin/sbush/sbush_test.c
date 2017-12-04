#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "Utilities.h"

void TESTEXECVPE2()
{
	pid_t pid=fork();
	 
	if(pid==0)
	{
		// yield();
		char *args[]={"/Tryme",NULL,NULL};
		execvpe("bin/ls",args,NULL);
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

int main_test(int argc, char *argv[], char *envp[]) {


	UpdateShellPrompt();

	while(1){

		char* input = TakeInput();
		
		//poll();

		if(input && input[0] != '\0'){
			 RedirectCommand(input);
		}
		else{
			 UpdateShellPrompt();
		}
		
		free(input);
		input = NULL;

	}

	return 0;
}