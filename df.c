#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "fs.h"


int main(int argc, char *argv[]){
	
	putenv("LOCALFS=some_fs.filesystem");
	
	char *fs_name = getenv("LOCALFS");
	
	struct fs *file_system;

	file_system = open_fs(fs_name);
	
	printf("Filesystem:\n%s\n\n", fs_name);

	printf("Cluster Size:\n%d bytes\n\n", file_system->boot_record.cluster_size);

	int sum_used = 0;

	struct dir_wrap *dir;

	dir = get_directory_table(file_system);

	for(int i = 0; i < dir->num_files; i++){
		if(dir->dir[i].name[0] != 0x00 || dir->dir[i].name[0] != 0xFF){
			sum_used += dir->dir[i].size;
		}
	}

	printf("Used:\n%d bytes\n\n", sum_used);

	printf("Available\n%d bytes", file_system->boot_record.size - sum_used);

	close_fs(file_system);

	return 1;
}

