#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"


int main(int argc, char *argv[]){
	
	if(argc != 2){
		printf("usage:\ntouch file\n");
		return 1;
	}

	putenv("LOCALFS=some_fs.filesystem");
	
	char *fs_name = getenv("LOCALFS");
	
	struct fs *file_system;

	file_system = open_fs(fs_name);
	
	char *file = get_file(file_system, argv[1]);

	if(file == NULL){
		//create the file 
		create_file(file_system, argv[1], "", 0, 0x0000);
	} else {
		// update the timestamp on the file
	}

	close_fs(file_system);

	return 1;
}
