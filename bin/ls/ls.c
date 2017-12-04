
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>
#include <dirent.h>



int main(int argc, char *argv[], char *envp[]) {


	if(argc > 2)
	{
		puts("Invalid number of arguements");
		return -1;
	}
	else if(argc==2)
	{
		char* path = argv[1];
		printf("param %s\n", path);

		path = "/";

		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir (path)) != NULL) {
		  /* print all the files and directories within directory */
		  while ((ent = readdir (dir)) != NULL) {
		    printf ("%s\n", ent->d_name);
		  }
		  closedir (dir);
		} else {
		  /* could not open directory */
		  return -1;
		}	

	}

	return 1;
}