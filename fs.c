#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "fs.h"

int create_fs(char *fs_name){
	char *input_buffer;

	char **prompts = (char **) malloc(sizeof(char *) * 3);

	prompts[0] = "Are you sure you want to create a new file system [Y]? ";
	prompts[1] = "Enter the maximum size for this file system in MB [10]: ";
	prompts[2] = "Enter the cluster size for this file system in KB [8]: ";

	input_buffer = (char *) malloc(sizeof(char) * 64);
	
	int make_filesystem = 1;
	int i = 0;

	struct br boot_record;

	boot_record.cluster_size = 8;
	boot_record.size = 10;


	while(make_filesystem == 1 && i < 3){

		printf("%s", prompts[i]), fgets(input_buffer, 64, stdin);
		
		if(i == 0){
			if(strlen(input_buffer) > 0){
				int in = (int)input_buffer[0];
				if(in != 10 && in != 89 && in != 121){
					make_filesystem = 0;
					printf("FS creation aborted\n");
				} else {
					i++;
				}
			}

		} else if(i == 1){
			if(strlen(input_buffer) > 0){
				if((int)input_buffer[0] != 10){
					int size = atoi(input_buffer);

					if(size <= MAX_FS_SIZE && size >= MIN_FS_SIZE){
						boot_record.size = size;
						i++;
					}
				} else if((int)input_buffer[0] == 10){
					i++;
				}
			}
		} else {
			if(strlen(input_buffer) > 0){
				if((int)input_buffer[0] != 10){
					int size = atoi(input_buffer);

					if(size <= MAX_CLUSTER_SIZE && size >= MIN_CLUSTER_SIZE){
						boot_record.cluster_size = size;
						i++;
					}
				} else if((int)input_buffer[0] == 10){
					i++;
				}
			}
		}
	}

	printf("Creating FS with:Name: %s\nSize: %d\nClusters: %d\n", fs_name, boot_record.size, boot_record.cluster_size);
	
	int fd;

	if((fd = open(fs_name, O_RDWR | O_CREAT, 0777)) == -1){
		perror("Cannot open output file\n");
		exit(1);
	}

	printf("FD: %d\n", fd);
	// set to the beginning
	if(lseek(fd, 0, SEEK_SET) < 0){
		perror("Error in lseek\n");
	}

	printf("Cluster size: %d\n", sizeof(boot_record.cluster_size));
	
}

int open_fs(char *fs_name){

}
