#include <unistd.h>
#include <stdlib.h>
#include "Utilities.h"
#include <dirent.h>

int ChangeDirectory(char* path)
{
	LOG("%s\n",path);
    return chdir(path);
}


int ExportEnvVar(char *args)
{
    puts("Need to implement ExportEnvVar\n");
    return -1;

        int t=0,j=0,k=0;
        char varname[1024];
        char varval[2048];
	   LOG("%s\n",args);
        for(int i=0;args[i]!='\0';i++)
        {
                if(args[i]=='=')
                {
                        t=1;
                }
                else
                if(t==1)
                {
                        
                        varval[k]=args[i];
                        k=k+1;
                }
                else
                {
                        varname[j]=args[i];
                        j=j+1;
                }
        }
        varname[j]='\0';
        varval[k]='\0';
        LOGG(varname);
        LOGG(varval);
return  setenv(varname,varval,1);
}

char* GetCurrentDir()
{
	char* cwd = malloc(sizeof(char)*1024);
   	if (getcwd(cwd, 1024) != NULL)
       return cwd;
   	else
       return NULL;
}

int MyLS(char* path)
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (path)) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir (dir)) != NULL) {
	    printf ("%s\n", ent->d_name);
	  }
	  closedir (dir);
	} else {
	  /* could not open directory */
	  LOG ("");
	  return -1;
	}	

	return 1;
}
