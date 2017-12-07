
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>
#include <dirent.h>



int main(int argc, char *argv[], char *envp[]) {

	char* queryPath = argv[1];

    struct dirent *pDirent;
    DIR *pDir;


    pDir = opendir (queryPath);
    if (pDir == NULL) {
        puts ("Cannot open directory \n");
        return 1;
    }
    puts("\n");
    while ((pDirent = readdir(pDir)) != NULL) {
        puts (pDirent->d_name);
        puts("\n");
    }
    // printf("readdir finished\n");
    closedir(pDir);

	return 1;
}