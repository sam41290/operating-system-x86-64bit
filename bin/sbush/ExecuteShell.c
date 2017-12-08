#include <stdio.h>
#include <stdlib.h>
#include "Utilities.h"
#include <string.h>
#include <unistd.h>


int Execute(char *cmd, char** vector, int vecCount)
{
        if (vecCount < 1 || cmd == NULL)
        {
                return -1;
        }

        if (cmd[0] == '.' || cmd[0] == '/')
        {
                return ExecuteAbsBinary(cmd, vector, vecCount);
        }

	//TODO: check if inbuilt command. if yes then run and exit
        // puts("Enter Execute");
        if (ExecuteBuiltIn(vector, vecCount) == 0)
        {
                return 0;
        }

        return -1;
        LOGG(cmd);
	//File checking part starts here-----------------------------------
                char *path;
                LOGG("Hi i am here");

                //TODO commented this remove later
                // path=getenv("PATH");
                path = "/Tryme";


                LOGG( path);
                // int cmdind=0;
                char dir[100];
                int k=0;



//Go to each path defined in env and search the command
                int pathlen=strlen(path);
                for (int j = 0; j<=pathlen; ++j)
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
                                for(int l=0;cmd[l]!='\0';l++)
                                {
                                        dir[k++]=cmd[l];
                                }
                                dir[k]='\0';
                                k=0;



                                //execute shell script
                                // puts(dir);
                                File *script=fopen(dir,"r");
                                if (script == NULL)
                                {
                                        LOGG("NULL");
                                        LOGG( dir);
                                }
                                else
                                {
                                        LOGG("Not NULL");
                                        LOGG( dir);
                                        // puts("Fopen success");
                                        //Read first 18 chars to confirm it is sbush script "#!rootfs/bin/sbush"
                                        char compare[19]; int comp = 0;
                                        for (; comp < 18; ++comp)
                                        {
                                                char c = fgetc(script);
                                                if (c == '\n' || c == '\0')
                                                {
                                                        break;
                                                }
                                                else
                                                {
                                                        compare[comp] = c;
                                                }

                                        }       
                                        compare[comp] = '\0';


                                        // puts(compare);
                                        LOGG("compare %s\n");



                                        fclose(script);
                                        if(strncmp(compare,"#!rootfs/bin/sbush",18)==0)
                                        {
                                                LOGG("strncmp match");
                                                File *fp=fopen(dir,"r");
                                                char c;
                                                char cmd[100];
                                                int k=0;
                                                c=fgetc(fp);
                                                int t=0;
                                                while(c!='\0')
                                                {
                                                        if(c=='#'){t=1;}        //Ignore Comments
                                                        
                                                        if(c=='\n')
                                                        {
                                                                cmd[k-1]='\0';
                                                                // puts("I am here--");puts(cmd);
                                                                if(t==0 && k>0)
                                                                {
                                                                        LOGG("executing command:%s\n");LOGG(cmd);
                                                                        int vecCount2;
                                                                        char** vector2 = SplitString(getargs(cmd), &vecCount2);
                                                                        // puts("Execute cmd");puts(vector2[0]);
                                                                        Execute(cmd, vector2, vecCount2);//replace with correct function
                                                                }

                                                                k=0;t=0;
                                                        }
                                                        else if(t==0)
                                                        {         
                                                                putchar(c);
                                                                cmd[k++]=c;
                                                        }
                                                        c=fgetc(fp);
                                                        putchar(c);
                                                }
                                                fclose(fp);
                                                return 1;
                                        } //TO DO: else run execvp to execute binary
                                        else
                                        {
                                                LOGG("%s is not Sbush script\n");LOGG(dir);
                                                LOG("%s will execute as binary\n", dir);
                                                // vector[0] = dir;
                                                return ExecuteBinary(vector, vecCount);
                                        }
				
										    
        		        } 

		        }

                }

                // printf("Not a builtin neither script\n" );
                // If nobody could run leave it to execvp
                LOGG("Not a builtin neither script\n");
                return ExecuteBinary(vector, vecCount);

}

int ExecuteAbsBinary(char *cmd, char** vector, int vecCount){


        //TODO: check if inbuilt command. if yes then run and exit
        LOGG("Enter ExecuteAbsBinary");


        LOGG(cmd);
        //File checking part starts here-----------------------------------

        char* dir = GetCurrentDir();
        int l = 0;
        while(dir[l] != '\0'){
                l++;
        }

        char* path = vector[0];

        for (int i = 2; path[i] != '\0'; ++i)
        {
                dir[l++] = path[i];
        }
        dir[l] = '\0';


        //execute shell script
        // puts(path);
        File *script=fopen(dir,"r");
        if (script == NULL)
        {
                LOGG("NULL");
                LOGG( dir);
        }
        else
        {
                LOGG("Not NULL");
                LOGG( dir);
                //Read first 18 chars to confirm it is sbush script "#!rootfs/bin/sbush"
                char compare[19]; int comp = 0;
                for (; comp < 18; ++comp)
                {
                        char c = fgetc(script);
                        if (c == '\n' || c == '\0')
                        {
                                break;
                        }
                        else
                        {
                                compare[comp] = c;
                        }

                }       
                compare[comp] = '\0';



                LOGG("compare %s\n");



                fclose(script);
                if(strncmp(compare,"#!rootfs/bin/sbush",18)==0)
                {
                        LOGG("strncmp match");
                        File *fp=fopen(dir,"r");
                        char c;
                        char cmd[100];
                        int k=0;
                        c=fgetc(fp);
                        int t=0;
                        while(c!='\0')
                        {
                                if(c=='#'){t=1;}        //Ignore Comments
                                
                                if(c=='\n')
                                {
                                        cmd[k-1]='\0';

                                        if(t==0 && k>0)
                                        {
                                                LOGG("executing command:%s\n");LOGG(cmd);
                                                int vecCount2;
                                                char** vector2 = SplitString(getargs(cmd), &vecCount2);
                                                // puts(cmd);
                                                Execute(cmd, vector2, vecCount2);//replace with correct function
                                        }

                                        k=0;t=0;
                                }
                                else if(t==0)
                                {
                                        cmd[k++]=c;
                                }
                                c=fgetc(fp);
                        }
                        fclose(fp);
                        return 1;
                } //TO DO: else run execvp to execute binary
                else
                {
                        return -1;
                        // puts("%s is not Sbush script\n");puts(dir);
                        LOG("%s will execute as binary\n", dir);
                        // vector[0] = dir;
                        // puts(vector[0]);putchar(48+vecCount);
                        return __ExecuteVE(vector, vecCount);
                }
        }                        
  
        return -1;      
}

