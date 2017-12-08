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
		SHELL("\n");
		SHELLCOLOR(currDir);
		SHELLCOLOR(" sbush> " );
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
	return 1;
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

int ExecuteLS(char* path){
	pid_t pid=fork();
	 
	if(pid==0)
	{
		//yield();
		// char* myArgs[1];
		// myArgs[0] = path;
		// myArgs[1] = NULL;

		char *myArgs[]={path,NULL,NULL};

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

//Free the returned char* after use
char* GetAbsolutePath(char* path){
	char* absolutePath = (char*)malloc(1024*sizeof(char));
	if (path[0] == '/')
	{
		for (int i = 0; path[i] != '\0'; ++i)
		{
			absolutePath[i] = path[i];
		}
	}
	else
	{
		char* currDir = GetCurrentDir();
		// printf("currDir %s\n", currDir);
		int i = 0;
		for (; currDir[i] != '\0'; ++i)
		{
			absolutePath[i] = currDir[i];		
		}	
		for (int j = 0; path[j] != '\0'; ++j)
		{
			absolutePath[i++] = path[j];
		}
		absolutePath[i] = '\0';
	}
	return absolutePath;
}

int ExecuteCAT(char* path){
	pid_t pid=fork();
	 
	if(pid==0)
	{
		//yield();
		char* absPath = GetAbsolutePath(path);
		// printf("Abs Path %s\n", absPath);
		char *myArgs[]={absPath,NULL,NULL};

		execvpe("/bin/cat",myArgs,NULL);
	 //execvpe: variable passing and path passing test pending
		free(absPath);
	}
	if(pid > 0)
	{
		int status;
		wait(&status);
	}

	return 0;
}

int ExecuteEcho(char* data){
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

int ExecutePS(){
	pid_t pid=fork();
	 
	if(pid==0)
	{
		//yield();
		// printf("Abs Path %s\n", absPath);
		//char *myArgs[]={data,NULL,NULL};

		execvpe("bin/ps",NULL,NULL);
	 //execvpe: variable passing and path passing test pending
	}
	if(pid > 0)
	{
		int status;
		wait(&status);
	}

	return 0;
}


int ExecuteKill(char* data){
	pid_t pid=fork();
	 
	if(pid==0)
	{
		//yield();
		// printf("Abs Path %s\n", absPath);
		char *myArgs[]={data,NULL,NULL};

		execvpe("bin/kill",myArgs,NULL);
	 //execvpe: variable passing and path passing test pending
	}
	if(pid > 0)
	{
		int status;
		wait(&status);
	}

	return 0;
}

int ExecuteBuiltIn(char** vector, int vecCount){
	// printf("Inside Builtin\n");
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
		// printf("Inside Change Dir\n");
		return ChangeDirectory(vector[1]);				//TODO Clean it
	}
	else if (0 == strncmp(cmd, "ls", 2))
	{
		// return MyLS(GetCurrentDir());				//TODO Clean it
		return ExecuteLS(GetCurrentDir());
	}
	else if (0 == strncmp(cmd, "cat", 3))
	{		
		return ExecuteCAT(vector[1]);				//TODO Clean it
	}
	else if (0 == strncmp(cmd, "echo", 4))
	{		
		return ExecuteEcho(vector[1]);				//TODO Clean it
	}
	else if (0 == strncmp(cmd, "ps", 2))
	{		
		return ExecutePS();				//TODO Clean it
	}
	else if (0 == strncmp(cmd, "kill", 4))
	{		
		return ExecuteKill(vector[1]);				//TODO Clean it
	}
	// else if (0 == strncmp(cmd, "export", 6))
 //    {
 //    		LOGG("Export");
 //            return ExportEnvVar(vector[1]);                              //TODO Clean it
 //    }
	// else if (0 == strncmp(cmd, "exit", 4))
	// {
	// 	SHELL("Shutting Down!\n");
	// 	exit(1);
	// }
	// else if (CheckIfPipeCommand(vector, vecCount))
	// {
	// 	LOG("WHY AM AI COMING HERE\n");
	// 	command* cmd1 = malloc(sizeof(command)); command* cmd2 = malloc(sizeof(command));
	// 	ParsePipeCommand(cmd1, cmd2, vector, vecCount);
	// 	ExecutePipe(cmd1, cmd2);
	// 	return 0;
	// }
	else
	{
		//We dont support anything other thatn built in commands
		// SHELL("We dont support anything other thatn built in commands\n");
		return 1;
	}

	LOGG("Not match");
	return 0;

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
		SHELL("\nFailed!\n");
	}
	

	UpdateShellPrompt();

	return ret;
}
