#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>

DIR *opendir(const char *name){

	unsigned long syscallnumber = 60;
	unsigned long dir;

	__asm__(
	"movq %1, %%rax;\n"
	"movq %2, %%rdi;\n"
	"int $0x80;\n"
	"movq %%rax, %0;\n"
	: "=m" (dir)
	: "m" (syscallnumber), "m" (name)
	: "rax","rdi"
	);

	if (dir == 0)
	{
		return NULL;
	}

	return (DIR*)dir;
}
struct dirent *readdir(DIR *dirp){

	unsigned long syscallnumber = 61;
	unsigned long dentry;

	__asm__(
	"movq %1, %%rax;\n"
	"movq %2, %%rdi;\n"
	"int $0x80;\n"
	"movq %%rax, %0;\n"
	: "=m" (dentry)
	: "m" (syscallnumber), "m" ((unsigned long)dirp)
	: "rax","rdi"
	);

	return (struct dirent*)dentry;	
}

int closedir(DIR *dirp){
	return 0;
}