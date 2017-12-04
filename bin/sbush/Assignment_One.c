// #define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utilities.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char* TakeInput(){

        char *line=malloc(sizeof(char)*1024);
//    size_t size = 0;
        if (gets(line) == 0) {
        puts("No line\n");
//puts(line);
    } else {
//puts(line);
//puts("I reached here");
        //Check for invalid string
        if (!ValidString(line))
        {
                puts("Invalid String\n");
                return NULL;
        }

    }

   // line[strlen(line)-1] = '\0';              //Make the new line char to end char

        return line;
}




void UpdateShellPrompt()
{
	char* currDir = GetCurrentDir();
	if (currDir != NULL)
	{
		SHELL(currDir);
		SHELL(" sbush >" );
	}
}

int __Execute(char** vector, int vecCount){

	char* cmd = vector[0];
	char* myArgs[4];

	LOG("%s %d\n", cmd, vecCount);
	for (int i = 0; i < vecCount; ++i)
	{
		myArgs[i] = vector[i];
		// LOG("%s,", myArgs[i]);
	}	
	myArgs[vecCount] = NULL;
       pid_t pid;
	if ((pid=fork()) == 0)
	{
		LOG("Going to execute execvp %s %s\n", cmd, myArgs[0]);
		
		execvp(cmd, myArgs);	

		/* If execvp returns, it must have failed. */

		CONSOLE("Unknown command:%s\n",cmd);
		exit(0);	
	}
	else
	{
		waitpid(pid,0,0);
		LOG("Child %d killed\n", pid);
		return 0;
	}
	return 0;
}

int __ExecuteVE(char** vector, int vecCount){

        char* cmd = vector[0];
        char* myArgs[4];

        LOG("%s %d\n", cmd, vecCount);
        for (int i = 0; i < vecCount; ++i)
        {
                myArgs[i] = vector[i];
                // LOG("%s,", myArgs[i]);
        }       
        myArgs[vecCount] = NULL;
       pid_t pid;
        if ((pid=fork()) == 0)
        {
                LOG("Going to execute execvp %s %s\n", cmd, myArgs[0]);
                execve(cmd, myArgs, NULL);    

                /* If execvp returns, it must have failed. */

                CONSOLE("Unknown command:%s\n",cmd);
                exit(0);        
        }
else{
         waitpid(pid,0,0);
        LOG("Child %d killed\n", pid);
        return 0;
        }
        return 0;
}

void poll(){
	LOG("Polling\n");

	BackgroundChild* child = getHead();
	while(child != NULL){
		if(waitpid(child->pid, NULL, WNOHANG) > 0)
		{
			LOG("------------------I(%d) AM KILLED-----------------\n", child->pid);
			RemoveBackgroundChildProcess(child->pid);
			return;
		}
		child = child->next;
	}

}

int __ExecuteAsync(char** vector, int vecCount)
{
	char* cmd = vector[0];
	char* myArgs[4];

	LOG("%s ", cmd);
	for (int i = 0; i < 4; ++i)
	{	
		if (i >= vecCount-1)
		{
			myArgs[i] = NULL;
			break;
		}
		myArgs[i] = vector[i];
		// LOG("%s,", myArgs[i]);
	}	

	pid_t childy = fork();
	AddNewBackgroundChildProcess(childy);
	if (childy == 0)
	{
		// setpgid(0,0);
		// LOG("Going to execute execvp %s %s\n", cmd, myArgs[1]);
		execvp(cmd, myArgs);	

		/* If execvp returns, it must have failed. */

		CONSOLE("Unknown command\n");
		exit(0);	
	}

	// pid_t child_pid = waitpid(-1, NULL, WNOHANG| WUNTRACED);
	LOG("I am %d and y child %d\n", getpid(), childy);
	// LOG("Killed %d\n", child_pid);
	// while(waitpid(child_pid, NULL, WNOHANG| WUNTRACED) != child_pid){
	// 	LOG("Running %d\n", child_pid);
	// }
	poll();

	// LOG("Killed %d\n", child_pid);
	return 0;
}





void ExecutePipe(command* cmd1, command* cmd2){

	    int fd[2];
        if(pipe(fd) == -1) {
          LOG("Pipe failed");
          exit(1);
        }

        if(fork() == 0)            //first fork
        {
        	//LOG("Child1 : %d\n", getpid());
            close(1);  //closing stdout
            dup2(fd[1],1);         //replacing stdout with pipe write 
            close(fd[0]);       //closing pipe read
            close(fd[1]);
		    // LOG("%s \n", cmd1->cmd);
		    // puts(cmd1->cmd);
            execvp(cmd1->cmd, cmd1->args);
            // LOG("execvp of cmd1 failed");
            exit(1);
        }

		// LOG("parent1 pid : %d\n", getpid());
        if(fork() == 0)            //creating 2nd child
        {
        	//LOG("Child2 : %d\n", getpid());
            close(0);  			 //closing stdin
            dup2(fd[0],0);         //replacing stdin with pipe read
            close(fd[1]);       //closing pipe write
            close(fd[0]);

		    // LOG("%s %s\n", cmd2->cmd, cmd2->args[1]);
		    // puts(cmd2->cmd);
            execvp(cmd2->cmd, cmd2->args);
            // LOG("execvp of cmd2 failed");
            exit(1);
        }

        // LOG("parent2 pid : %d\n", getpid());
        close(fd[0]);
        close(fd[1]);
        wait(0);
        wait(0);
        // pid_t child = wait(0);
        // pid_t child2 = wait(0);
        // LOG("killed %d\n", child);
        // LOG("killed %d\n", child2);
}


int ExecuteBinary(char** vector, int vecCount){

	char* cmd;
	// char* myArgs[4] ;


	cmd = vector[0];
	// LOG("%s %d\n", cmd, vecCount);
	LOGG(cmd);
	// putchar(48+vecCount);
	// shputs("vec count:");putchar(48+vecCount);

	if(0 == strncmp(vector[vecCount-1],"&", 1))
	{
		return __ExecuteAsync(vector, vecCount);
	}
	else
	{
		return __Execute(vector, vecCount);
	}
}

int ExecuteLS(){
	pid_t pid=fork();
	 
	if(pid==0)
	{
		//yield();
		char* myArgs[1];
		myArgs[0] = "Hi";
		myArgs[1] = NULL;

		execvpe("bin/ls",myArgs,NULL);
	 //execvpe: variable passing and path passing test pending
	}
	if(pid > 0)
	{
		int status;
		wait(&status);
	}	

	return 0;
}


int ExecuteCAT(){
	pid_t pid=fork();
	 
	if(pid==0)
	{
		//yield();
		execvpe("bin/cat",NULL,NULL);
	 //execvpe: variable passing and path passing test pending
	}
	if(pid > 0)
	{
		int status;
		wait(&status);
	}

	return 1;
}

int ExecuteBuiltIn(char** vector, int vecCount){

	char* cmd;
	// char* myArgs[4] ;


	cmd = vector[0];
	LOG("%s ", cmd);
	// for (int i = 0; i < vecCount; ++i)
	// {
	// 	myArgs[i] = vector[i];
	// 	// LOG("%s,", myArgs[i]);
	// }	

	LOGG("cmd");
	if (0 == strncmp(cmd, "cd", 2))
	{
		return ChangeDirectory(vector[1]);				//TODO Clean it
	}
	else if (0 == strncmp(cmd, "ls", 2))
	{
		return MyLS(GetCurrentDir());				//TODO Clean it
	}
	else if (0 == strncmp(cmd, "cat", 3))
	{
		return ExecuteCAT();				//TODO Clean it
	}
	else if (0 == strncmp(cmd, "export", 6))
    {
    		LOGG("Export");
            return ExportEnvVar(vector[1]);                              //TODO Clean it
    }
	else if (0 == strncmp(cmd, "exit", 4))
	{
		SHELL("Shutting Down!\n");
		exit(1);
	}
	else if (CheckIfPipeCommand(vector, vecCount))
	{
		LOG("WHY AM AI COMING HERE\n");
		command* cmd1 = malloc(sizeof(command)); command* cmd2 = malloc(sizeof(command));
		ParsePipeCommand(cmd1, cmd2, vector, vecCount);
		ExecutePipe(cmd1, cmd2);
		return 0;
	}
	else
	{
		return -1;
	}

	LOGG("Not match");
	return -1;

}





int RedirectCommand(char* input){

	LOGG(input);
	//Expand all path variables and environment variables in the input

	LOGG("Rediresct");
	//Extract command
	int vecCount;
	char** vector = SplitString(getargs(input), &vecCount);
    // putchar(vecCount+48);
   	// LOGG(vector[0]);
    // LOGG(vector[1]);
	LOGG("Redirect2");
	//Execute command
	// int ret = Execute(vector, vecCount-1);
	LOGG("FULL COMMAND :");
	LOGG(input);

	int ret = Execute(input, vector, vecCount);
	if( ret == -1){
		SHELL("Failed!\n");
	}
	

	UpdateShellPrompt();

	return ret;
}
