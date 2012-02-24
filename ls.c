#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"

int main(int argc, char *argv[]){
	
	putenv("LOCALFS=some_fs.filesystem");
	
	char *fs_name = getenv("LOCALFS");
	
	struct fs *file_system;

	file_system = open_fs(fs_name);

	struct dir_wrap *dir;

	dir = get_directory_table(file_system);

	for(int i = 0; i < dir->num_files; i++){
		if(dir->dir[i].name[0] != 0x00 && dir->dir[i].name[0] != 0xFF){
			printf("%s\n", dir->dir[i].name);	
		}
	}

	close_fs(file_system);

	return 1;
}
