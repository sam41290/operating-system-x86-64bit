#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "Utilities.h"




int main(int argc, char *argv[], char *envp[]) {


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