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

	printf("Creating FS with:Name: %s\nSize: %d\nClusters: %d\n", 
			fs_name, boot_record.size, boot_record.cluster_size);
	
	
	FILE *fs = create_empty_file(fs_name, boot_record.size * MEGABYTE);

	// move the cursor to the start of the file
	if(fseek(fs, 0, SEEK_SET) < 0){
		fprintf(stderr, "Seek erorr: %s\n", strerror(errno));
	}
	
	char test[] = "Hello world";

	fwrite(&test, 1, sizeof(test), fs);
	
	seek_to_beginning(fs);

	char other_test[strlen(test) + 1];

	fread(&other_test, 1, sizeof(test), fs);

	printf("Read: %s\n", other_test);
}

void seek_to_beginning(FILE *file){
	if(fseek(file, 0, SEEK_SET) < 0){
		fprintf(stderr, "Seek erorr: %s\n", strerror(errno));
	}
}

FILE *create_empty_file(char *file_name, int size){
	FILE *new_file;

	new_file = fopen(file_name, "wb+");
	
	if(new_file == NULL){
		fprintf(stderr, "Could not create file %s\n", file_name);
	}

	// set the filesize by seeking to size - 1 and writing a 0 to the place
	if(fseek(new_file, size - 1, SEEK_SET) < 0){
		fprintf(stderr, "Seek erorr: %s\n", strerror(errno));
	}

	fwrite("", 1, sizeof(""), new_file);

	return new_file;

}

int open_fs(char *fs_name){

}
