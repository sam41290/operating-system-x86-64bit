#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Utilities.h"

int bootsetenv()
{
        puts("Need to implement bootsetenv\n");
        return -1;

	File *fp;
        fp=fopen("a.profile","r");
        if(fp)
                LOG("profile loaded\n");
        else
        {
                LOG("no profile\n");
                return -1;
        }
        char var[100];
        char varval[200];
        char c;
        int i=0;
        c=fgetc(fp);
        while (c!=EOF)
        {
                if(c=='$')
                {
                        i=0;
                        while((c=fgetc(fp))!='=')
                        {
                                var[i]=c;
                                i=i+1;
                        }
                        var[i]='\0';
                        i=0;
                        c=fgetc(fp);
                        while(c!='\n' && c!=EOF)
                        {
                                varval[i]=c;
                                i=i+1;
                                c=fgetc(fp);
                        }
                        varval[i]='\0';
                        if(c=='\n')
                                c=fgetc(fp);
                }

        setenv(var,varval,1);
        }
        char *home;
        home=getenv("HOME");
        if(chdir(home)<0)
        {
                LOG("failed to go to home directory..\n");
                return 0;
        }
        char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
        LOG("YOU ARE SBUSH\n\nOWNER:SOUMYAKANT\n\nYOUR HOME DIR:%s\n\n", cwd);
        else
        LOG("getcwd() error");
        //fclose(fp);
        if(getcwd(cwd, sizeof(cwd))==NULL)
        {
                LOG("failed to go to home directory..\n");
                return 0;
        }
        LOG("%s",cwd);
        LOG(":sbushc>");
        return 1;
}



char * getenvArgs(int pos,char *envvar)
{
        puts("Need to implement getenvArgs\n");
        return NULL;
                       char *tempargs;
                        int j=pos + 1;
                        int k=0;
                        char varname[1024];
                        while(envvar[j]!='\0' && envvar[j]!=' ' && envvar[j]!=':')
                        {
                                varname[k]=envvar[j];
                                j=j+1;
                                k=k+1;
                        }
                        varname[k]='\0';
                        tempargs=getenv(varname);
//printf("testenv%s\n",tempargs);

                        //shputs(tempargs);
                        return tempargs;
}

char *getargs(char *args)
{
        // puts("Need to implement getargs\n");
        return args;
                // shputs("test:");
                // puts(args);
                int argspos=0;
                int finargspos=0;
                char* passargs = malloc(sizeof(char)*1024);
                while(args[argspos]!='\0')
                {
                        if(args[argspos]=='$')
                        {
                                char *envargs;
                                envargs=getenvArgs(argspos,args);
                                // shputs("envargs:");puts(envargs);
//                                printf("%s\n",envargs);
                                if (envargs==NULL)
                                        {
                                                char temp[]="Environment variable not defined";
                                                envargs=temp;
                                                return envargs; }
                                int pos=0;
                                while(envargs[pos]!='\0')
                                {
                                        passargs[finargspos]=envargs[pos];
                                        pos=pos+1;
                                        finargspos=finargspos+1;
                                }
                                while(args[argspos]!=' ' && args[argspos]!='\0' && args[argspos]!=':')
                                        argspos=argspos+1;
                        } else
                        {
                                passargs[finargspos]=args[argspos];
                                finargspos=finargspos+1;
                                argspos=argspos+1;
                        }
                }
                passargs[finargspos]='\0';
                //LOG("test args:%s\n",passargs);
                //char *returnargs=passargs;
                // shputs("args:");
                // puts(passargs);
                return passargs;
}
