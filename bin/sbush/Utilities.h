#include <stdio.h>
#include <stdlib.h>

// #include <string.h>

void COMMON_Print(char *format, ...);

#define DEBUG 0

#define LOG(fmt, ...) \
        do { if (DEBUG) COMMON_Print(fmt, ##__VA_ARGS__);	\
    		} while (0)

#define CONSOLE(fmt, ...) COMMON_Print(fmt, ##__VA_ARGS__);

#define SHELL(fmt) shputs(fmt);

#define LOGG(fmt)\
        do { if (DEBUG) puts(fmt);	\
    		} while (0)



// #define LOG(fmt, ...) 
//         do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); 
//     		} while (0)

// #define CONSOLE(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__);

struct com{
    char* cmd;
    char* args[4];
} typedef command;

char* TakeInput();
int RedirectCommand(char* input);
void UpdateShellPrompt();
int ExecuteBuiltIn(char** vector, int vecCount); 
int __Execute(char** vector, int vecCount); 
int ExecuteBinary(char** vector, int vecCount);
int ExecuteAbsBinary(char *cmd, char** vector, int vecCount);
int __ExecuteVE(char** vector, int vecCount);
//Soumya
int bootsetenv();
char * getenvArgs(int pos,char *envvar);
char *getargs(char *args);
int ExportEnvVar(char *args);
int Execute(char *cmd, char** vector, int vecCount);

//Libc Wrapper
char* GetCurrentDir();
int ChangeDirectory(char* path);
int MyLS(char* path);


//Util
char** SplitString(char* str, int* vecCount);
int ValidString(char* line);
int CheckIfPipeCommand(char** vector, int vecCount);
void ParsePipeCommand(command* cmd1, command* cmd2, char** vector, int vecCount);
struct envVar** getAllEnvironmentVaiables(int*);
void print(struct envVar** allVariables, int count);
char* mystrtok(char *str, const char delim, int* pos);
int shputs(const char *s);

//For Async Process
struct BackgroundChildStruct {
    int pid;
    struct BackgroundChildStruct * next;
    struct BackgroundChildStruct * prev;
};
typedef struct BackgroundChildStruct BackgroundChild;

void AddNewBackgroundChildProcess(pid_t pid);
void RemoveBackgroundChildProcess(pid_t);
BackgroundChild* getHead();
void printLL();
void poll();

int setenv(const char *name, const char *value, int overwrite);
char* getenv(const char *name);

//Temporary jugaad
// ssize_t getline(char **lineptr, size_t *n, FILE *stream);
