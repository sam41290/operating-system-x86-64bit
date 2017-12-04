#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utilities.h"


void COMMON_Print(char *format, ...)
{
    // va_list args;

    // va_start(args, format);
    // vprintf(format, args);
    // va_end(args);
}

int shputs(const char *s)
{
  for( ; *s; ++s) if (putchar(*s) != *s) return EOF;
  return 0;
}

int CheckIfPipeCommand(char** vector, int vecCount){
    // Currently we support only 1 pipe

    for (int i = 0; i < vecCount; ++i)
    {
        if (0 == strncmp(vector[i], "|", 1))
        {
            LOG("Its a pipe command!!\n");
            return 1;
        }
    }
    return 0;
}

void ParsePipeCommand(command* cmd1, command* cmd2, char** vector, int vecCount){

    int pos = 1;
    cmd1->cmd = vector[0]; cmd1->args[0] = vector[0]; 
    for (; (strncmp(vector[pos],"|",1)!=0) && pos<vecCount; ++pos)
    {
        cmd1->args[pos] = vector[pos];
    }
    cmd1->args[pos] = NULL;

    if (pos++ >= vecCount)
    {
        return;
    }          

    int i = 0;                                                          
    cmd2->cmd = vector[pos];
    for (; pos < vecCount; ++pos)
    {
        cmd2->args[i++] = vector[pos];
    }
    cmd2->args[i] = NULL;
}



// char** SplitString(char* str_copy, int* vecCount){

//     char* str = malloc(sizeof(char)*1024);
//     int j = 0;
//     for (; str_copy[j] != '\0'; ++j)
//     {
//         str[j] = str_copy[j];
//     }
//     str[j] = '\0';

// 	char** vector = malloc(sizeof(char*)*20);		//TODO: Change this 2 to something good
//     *vecCount = 0;

//     // Returns first token 
//     int i = 0;
//     char *token = mystrtok(str, ' ');
//    	vector[i++] = token;
//     *vecCount = *vecCount+1;

//     // Keep printing tokens while one of the
//     // delimiters present in str[].
//     while (token != NULL)
//     {
//         // printf("%s\n", token);
//         token = mystrtok(NULL, ' ');
//     	vector[i++] = token;
//         *vecCount = *vecCount+1;
//     }


//     return vector;
// }




int ValidString(char* line){

    if (line[0] == '\n')
    {
        LOG("Just Enter\n");
        return 0;
    }       
    else
    {
        int empty = 0;
        for (int i = 0; i < strlen(line); ++i)
        {
            if (line[i] == ' ')
            {
                empty++;
            }
        }
        return (empty < strlen(line)-1);
    }
    return 0;
}


// char* MakeProfileFile(){

//     FILE *fp;
//     fp = fopen("a.profile" ,"a");
//     if (!fp)
//     {
//         CONSOLE("Can't create profile file! Shutting Down!");
//         exit(0);
//     }

//     fputs("This is testing for fputs...\n", fp);


// }

static BackgroundChild* BgChildLL = NULL;

BackgroundChild* getHead(){
    return BgChildLL;
}

void AddNewBackgroundChildProcess(pid_t pid){
    if (BgChildLL == NULL)
    {
        BgChildLL = malloc(sizeof(BackgroundChild));
        BgChildLL->pid = pid;
        BgChildLL->next = NULL;
        BgChildLL->prev = NULL;
    }
    else
    {
        BackgroundChild* newNode = malloc(sizeof(BackgroundChild));
        newNode->pid = pid;newNode->next = NULL; newNode->prev = NULL;
        BackgroundChild* lastNode = BgChildLL;
        while(lastNode->next != NULL){
            lastNode = lastNode->next;
        }
        lastNode->next = newNode;
        newNode->prev = lastNode;
    }
}

void RemoveBackgroundChildProcess(pid_t pid){
    BackgroundChild* temp = BgChildLL;
    while(temp != NULL){
        if (temp->pid == pid)
        {
            BackgroundChild* prevNode = temp->prev;
            BackgroundChild* nextNode = temp->next;
            if (prevNode!=NULL)
            {
                prevNode->next = nextNode;
                if (nextNode!=NULL)
                {
                    nextNode->prev = prevNode;
                }
            }
            else
            {
                BgChildLL = nextNode;
                if (nextNode!=NULL)
                {
                    nextNode->prev = BgChildLL;
                }
            }
        }
        temp = temp->next;
    }  
}

void printLL(){
    BackgroundChild* temp = BgChildLL;
    while(temp != NULL){
        LOG(" %d -> ", temp->pid);
        temp = temp->next;
    }
}

char* mystrtok(char *str, const char delim, int* pos){

    int startFound = 0; int start = 0;
    int endFound = 0; int end = 0;
    int i = *pos;
    for (; str[i] != '\0'; ++i)
    {
        if (str[i] != delim && !startFound)
        {
            start = i; startFound = 1;
        }
        else if (str[i] == delim && startFound)
        {
            end = i-1; endFound = 1;
            break;
        }
    }
    if (startFound)
    {
        end = i-1; endFound = 1;
    }

    if (endFound)
    {
        char* token = malloc(sizeof(char)*end-start+5);
        int j = 0;
        for (int i = start; i <= end; ++i)
        {
            token[j++] = str[i];
        }
        token[j] = '\0';
        *pos = end+1;
        return token;
    }

    return NULL;

}


char** SplitString(char* str_copy, int* vecCount){

    if (str_copy == NULL)
    {
        return NULL;
    }

    char* str = malloc(sizeof(char)*1024);
    int j = 0;
    for (; str_copy[j] != '\0'; ++j)
    {
        str[j] = str_copy[j];
    }
    str[j] = '\0';

    char** vector = malloc(sizeof(char*)*20);       //TODO: Change this 2 to something good
    *vecCount = 0;

    // Returns first token 
    int i = 0;
    int pos = 0;
    char *token = mystrtok(str, ' ', &pos);
    vector[i++] = token;
    *vecCount = *vecCount+1;
    // printf("%s", token);

    // Keep printing tokens while one of the
    // delimiters present in str[].
    while (token != NULL)
    {
        // printf("%s\n", token);
        token = mystrtok(str, ' ', &pos);
        vector[i++] = token;
        *vecCount = *vecCount+1;
    }
    *vecCount = *vecCount-1;
    // printf("%d", *vecCount);
    // for (int i = 0; i < *vecCount; ++i)
    // {
    //     printf("%s ", vector[i] );
    // }
    return vector;
}
