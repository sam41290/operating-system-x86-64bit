#ifndef _STDIO_H
#define _STDIO_H

#define O_RDONLY	0x0000
#define O_WRONLY	0x0001
#define O_RDWR		0x0002
#define O_TRUNC		0x0800
#define O_APPEND	0x1000


typedef struct _iobuf {
    int cnt;          
    char *ptr;        
    char *base;       
    int flag;         
    int fd;           
} File;

static const int EOF = -1;

int putchar(int c);
int puts(const char *s);
int putcharcolor(int c);

int gets(char *s);

//Temporary support to make life easy
int printf(const char *format, ...);

#endif
