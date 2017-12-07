#ifndef __KPRINTF_H
#define __KPRINTF_H

#include <sys/defs.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/gdt.h>
char *getcurrdisp();
void setcurrdisp();
void kprintf(const char *fmt, ...);
void flushtime(int seconds);
void flushLastKeyPress(char a, char b);
void flushchar(const char ch);
void flushbackspace();
void blankpage();
void resetdatawritten();
void flushcharcolor(const char ch);
void flushcolor(const char* text);

#endif
