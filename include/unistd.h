#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/defs.h>

int open(const char *pathname, int flags);
int close(int fd);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int unlink(const char *pathname);

int chdir(const char *path);
char *getcwd(char *buf, size_t size);

pid_t fork();
int execvpe(const char *file, char *const argv[], char *const envp[]);
pid_t wait(int *status);
int waitpid(int pid, int *status, int options);

unsigned int sleep(unsigned int seconds);

// OPTIONAL: implement for ``on-disk r/w file system (+10 pts)''
off_t lseek(int fd, off_t offset, int whence);
//int mkdir(const char *pathname, mode_t mode);

// OPTIONAL: implement for ``signals and pipes (+10 pts)''
int pipe(int pipefd[2]);

//Our own requirement
pid_t getpid(void);
pid_t getppid(void);
void yield(void);
#include <stdio.h>
File *fopen(const char *path,const char *mode);
char fgetc(File* fp);
int fclose(File *fp);

//Just for sake of build
#define WNOHANG 1

int execvp(const char *file, char* argv[]);
int execve(const char *filename, char *const argv[], char *const envp[]);
int waitpid(int pid, int *status, int options);
int dup2(int oldfd, int newfd);

#endif
