#ifndef _STDIO_H
#define _STDIO_H

static const int EOF = -1;

int putchar(int c);
int puts(const char *s);

int gets(char *s);

//Temporary support to make life easy
int printf(const char *format, ...);

#endif
