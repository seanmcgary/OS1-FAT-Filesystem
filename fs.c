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

	boot_record.cluster_size = 8 * KILOBYTE;
	boot_record.size = 10 * MEGABYTE;
	boot_record.fat = 0;
	boot_record.root_dir = 0;


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
						boot_record.size = (size * MEGABYTE);
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
						boot_record.cluster_size = (size * KILOBYTE);
						i++;
					}
				} else if((int)input_buffer[0] == 10){
					i++;
				}
			}
		}
	}

	printf("Creating FS with:\n\tName: %s\n\tSize: %d\n\tClusters: %d\n------\n", 
			fs_name, boot_record.size, boot_record.cluster_size);
	
	// create the FAT
	int num_clusters = calc_num_clusters(boot_record.size, boot_record.cluster_size);
	int fat[num_clusters];

	for(int i = 0; i < num_clusters; i++){
		if(i == 0 || i == 1){
			// set cluster 0, the br, as reserved
			// set cluster 1, the FAT, as reserved
			fat[i] = 0xFFFE;
		} else if(i == (num_clusters - 1)){
			// last entry in the table
			fat[i] = 0xFFFF;
		} else {
			// set everything else as a free cluster
			fat[i] = 0x0000;
		}
	}

	// set the fat in the boot record
	boot_record.fat = 1;
	
	// set the root_dir to cluster 2
	boot_record.root_dir = 2;

	// initialize the root dir
	//struct directory_entry *root_dir = (struct directory_entry *) malloc(boot_record.cluster_size);	
	int num_files = boot_record.cluster_size / sizeof(struct directory_entry);
	struct directory_entry root_dir[num_files];

	// set all files to available for use
	for(int i = 0; i < num_files; i++){
		root_dir[i].name[0] = (char)0x00;
	}
	

	printf("Size of FAT: %d\n", sizeof(fat));

	FILE *fs = create_empty_file(fs_name, boot_record.size);

	// move the cursor to the start of the file
	seek_to_beginning(fs);

	// write the boot record
	fwrite(&boot_record, 1, sizeof(struct br), fs);
	
	
	// write the FAT to the filesystem
	seek_to_cluster(fs, 1, boot_record.cluster_size);
	fwrite(&fat, 1, sizeof(fat), fs);
	
	// write the root directory
	seek_to_cluster(fs, 2, boot_record.cluster_size);
	fwrite(&root_dir, 1, boot_record.cluster_size, fs); 


	seek_to_cluster(fs, 2, boot_record.cluster_size);

	struct directory_entry tmp_root[boot_record.cluster_size / sizeof(struct directory_entry)];
	printf("tmp_root size: %d\n", sizeof(tmp_root));
	
	//tmp_root = (struct directory_entry *) malloc(boot_record.cluster_size / sizeof(struct directory_entry));

	fread(&tmp_root, sizeof(tmp_root), 1, fs);
	
}

void seek_to_beginning(FILE *file){
	if(fseek(file, 0, SEEK_SET) < 0){
		fprintf(stderr, "Seek erorr: %s\n", strerror(errno));
	}
}

void seek_to_cluster(FILE *file, int cluster_num, int cluster_size){
	int offset = calc_cluster_offset(cluster_num, cluster_size);

	if(fseek(file, offset, SEEK_SET) < 0){
		fprintf(stderr, "Error seeking to cluster: %s\n", strerror(errno));
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

int calc_num_clusters(int fs_size, int cluster_size){
	return fs_size/cluster_size;
}

int calc_cluster_offset(int cluster_number, int cluster_size){
	return (cluster_number * cluster_size);
}

int open_fs(char *fs_name){

}
