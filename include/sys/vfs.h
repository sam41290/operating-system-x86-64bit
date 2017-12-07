#ifndef _VFS_H_
#define _VFS_H_

#include <sys/defs.h>

#define MAX_FAMILY 10	//Maximum children inside a floder
#define NAME_MAX 255	//Maximim length of a foldername

enum inodeType{
	DIR=0,
	FILE=1
};

struct inode_t{
	uint64_t start;     //Start address of the file data
	uint64_t end;      //End address of the file data
	struct inode_t* family[MAX_FAMILY];       //family 0 stores parent and rest all store child. We support max 9 children
	int familyCount;
	enum inodeType type;
	char inodeName[NAME_MAX];
};

typedef struct inode_t inode;

struct file_object_t{
	uint64_t currentoffset;     //Current offset in inode
	struct inode_t* node;
};

struct dirent_t {
 char d_name[NAME_MAX+1];
};

typedef struct dirent_t dirent;

struct dir_t{
	char path[NAME_MAX+1];
	uint64_t currInode;
	dirent currDirent;
	struct dir_t* nextdir;
};

typedef struct dir_t dir;

typedef struct file_object_t file_object;
void init_tarfs();
void walkthough_inode(inode* node);
inode* GetInode(char* path);
void recycledirstruct(dir* nodeToRemove);
dir* getdir_from_recylebin();
dir* getnewdir();
#endif