#include <sys/vfs.h>
#include <stdio.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/kstring.h>
#include <sys/virt_mem.h>
#include <sys/task_manager.h>
#include <sys/kmalloc.h>
#include <sys/vfs.h>


inode* root_inode;

int findFamily(inode* node, char* token, uint64_t tokenLength){
	for (int i = 1; i < node->familyCount; ++i)
	{
		if (0 == strncmp(node->family[i]->inodeName, token, tokenLength))
		{
			return i;
		}
	}
	return 0;
}

uint64_t appendToFamily(inode* parent_inode, char* token, uint64_t start, uint64_t end, uint64_t type){
		
	if (parent_inode->familyCount >= MAX_FAMILY)
	{
		kprintf("We dont support more than 9 files/directory inside a directory!\n");
		return -1;
	}

	inode* new_node = (inode*)kmalloc(sizeof(inode));
	new_node->start = start;
	new_node->end = end;
	new_node->type = type;
	for (int i = 0; i < strlen(token); ++i)
	{
		new_node->inodeName[i] = token[i];
	}
	new_node->inodeName[strlen(token)] = '\0';

	new_node->family[0] = parent_inode;
	new_node->familyCount = 1;	
	// kprintf("Inserting at parent %s ", parent_inode->inodeName);
	parent_inode->family[parent_inode->familyCount] = new_node;
	parent_inode->familyCount++;
	return parent_inode->familyCount-1;
}

void InsertIntoRoot(char* path, uint64_t start, uint64_t end, int type)
{
	// kprintf("Request to insert %s", path);
    int pos = 0;
    char *token = mystrtok(path, '/', &pos);
    inode* node = root_inode;

	while(token != NULL){
	
		int familymember = findFamily(node, token, strlen(token));

		if ((familymember == 0))
		{
			// kprintf("could not find member %s\n", token);
			//Append the token to node->family
			familymember = appendToFamily(node, token, start, end, type);
		}

		// kprintf("%d ", familymember);
		if (familymember > 0 && familymember < MAX_FAMILY)
		{	
			// kprintf("Appended %s to parent %s\n", node->family[familymember]->inodeName, node->inodeName);
			node = node->family[familymember];
		}

		// kprintf("token : %s - ", token);

		// kfree((uint64_t)token, strlen(token));
    	token = mystrtok(path, '/', &pos); 	
  	 
	}

	// kprintf("\n");
}

void init_tarfs(){

	root_inode = (inode*)kmalloc(sizeof(inode));
	root_inode->start = 0;
	root_inode->end = 1;
	root_inode->inodeName[0] = '/';
	root_inode->inodeName[1] = '\0';
	root_inode->family[0] = root_inode; //Parent is me
	root_inode->familyCount = 1;

	struct posix_header_ustar *header =  (struct posix_header_ustar*)(&_binary_tarfs_start);
    struct posix_header_ustar *end =  (struct posix_header_ustar*)(&_binary_tarfs_end);
	uint64_t size=0;
    while(header < end)
    {
    	size=oct_to_dec(header->size);
    	uint64_t dataStart = (uint64_t)(header+1); uint64_t datEnd = (uint64_t)(dataStart+size);
    	if ((strlen(header->name) > 0) && (header->size[0]!='\0'))
    	{
	   		// kprintf("Found %s %s %d!!\n", header->name, header->typeflag, datEnd-dataStart);
    		if (strncmp(header->name, "lib/", 4) != 0 && strncmp(header->name, "bin/", 4) != 0)	//Dont parse libc folder. Some bug in printf
    		{
	    		if (header->typeflag[0] == '0')
	    		{
	    			//File
	    			InsertIntoRoot(header->name, dataStart, datEnd, FILE);
	    		}
	    		else if (header->typeflag[0] == '5')
	    		{
	    			//Directory
	     			InsertIntoRoot(header->name, dataStart, datEnd, DIR);
	   			}
   			}
    	}

    	// kprintf("Tinsert root done\n");

		if((size % sizeof(struct posix_header_ustar))==0)
		{
			header= header + 1 + (int)(size/sizeof(struct posix_header_ustar));
		}
		else
		{
			header= header + 2 + (int)(size/sizeof(struct posix_header_ustar));
		}
	}

}

inode* GetInode(char* path){
	// kprintf("Inside GetINode");
    int pos = 0;
    char *token = mystrtok(path, '/', &pos);
    inode* node = root_inode;


	while(token != NULL){
		int familymember = findFamily(node, token, strlen(token));

		if ((familymember == 0))
		{
			// kprintf("could not find member %s\n", token);
			//Return empty
			return NULL;
		}

		// kprintf("%d ", familymember);
		if (familymember > 0 && familymember < MAX_FAMILY)
		{	
			// kprintf("Appended %s to parent %s\n", node->family[familymember]->inodeName, node->inodeName);
			node = node->family[familymember];
		}

		// kprintf("token : %s - ", token);

		// kfree((uint64_t)token, strlen(token));
    	token = mystrtok(path, '/', &pos);
	}

	return node;
}



//Helper Test Functions
// void walkthough_tarfs(){
// 	struct posix_header_ustar *header =  (struct posix_header_ustar*)(&_binary_tarfs_start);
//     struct posix_header_ustar *end =  (struct posix_header_ustar*)(&_binary_tarfs_end);
// 	int size=0;
//     while(header < end)
//     {
//     	if ((strlen(header->name) > 0) && (header->size[0]!='\0'))
//     	{
//     		kprintf("Found %s !!\n ", header->name);
//     	}
    	
// 		size=oct_to_dec(header->size);
// 		if((size % sizeof(struct posix_header_ustar))==0)
// 		{
// 			header= header + 1 + (int)(size/sizeof(struct posix_header_ustar));
// 		}
// 		else
// 		{
// 			header= header + 2 + (int)(size/sizeof(struct posix_header_ustar));
// 		}
// 	}
// }


// void Query_test(){

// 	inode* query = GetInode("Tryme/");

// 	for (int i = 1; i < query->familyCount; ++i)
// 	{
// 		kprintf("%s -- ", query->family[i]->inodeName);
// 	}


// 	inode* query2 = GetInode("Sexy/");

// 	if (query2 == NULL)
// 	{
// 		kprintf("No entry found! Go back Simon\n");
// 		return;
// 	}
// 	for (int i = 1; i < query2->familyCount; ++i)
// 	{
// 		kprintf("%s -- ", query2->family[i]->inodeName);
// 	}

// 	return;	
// }

// void walkthough_inode(inode* node){

// 	Query_test();
// 	return;

// 	for (int i = 1; i < node->familyCount; ++i)
// 	{
// 		kprintf("%s -- ", node->family[i]->inodeName);
// 		// walkthough_inode(node->family[i]);
// 		// kprintf("\n");
// 	}
// 	kprintf("\n");
// 	node = root_inode->family[1];
// 	for (int i = 1; i < node->familyCount; ++i)
// 	{
// 		kprintf("%s -- ", node->family[i]->inodeName);
// 		// walkthough_inode(node->family[i]);
// 		// kprintf("\n");
// 	}	

// 	kprintf("\n");
// 	node = root_inode->family[2];
// 	for (int i = 1; i < node->familyCount; ++i)
// 	{
// 		kprintf("%s -- ", node->family[i]->inodeName);
// 		// walkthough_inode(node->family[i]);
// 		// kprintf("\n");
// 	}	

// 	kprintf("\n");
// 	node = root_inode->family[3];
// 	kprintf("Childre of %s:", node->inodeName);
// 	for (int i = 1; i < node->familyCount; ++i)
// 	{
// 		kprintf("%s -- ", node->family[i]->inodeName);
// 		// walkthough_inode(node->family[i]);
// 		// kprintf("\n");
// 	}		

// 	kprintf("\n");
// 	node = root_inode->family[4];
// 	kprintf("Childre of %s:", node->inodeName);
// 	for (int i = 1; i < node->familyCount; ++i)
// 	{
// 		kprintf("%s -- ", node->family[i]->inodeName);
// 		// walkthough_inode(node->family[i]);
// 		// kprintf("\n");
// 	}	


// 	kprintf("\n");
// 	node = root_inode->family[5];
// 	kprintf("Childre of %s:", node->inodeName);
// 	for (int i = 1; i < node->familyCount; ++i)
// 	{
// 		kprintf("%s -- ", node->family[i]->inodeName);
// 		// walkthough_inode(node->family[i]);
// 		// kprintf("\n");
// 	}	
// }