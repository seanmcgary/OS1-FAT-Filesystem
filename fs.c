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
		if(i == 0 || i == 1 || i == 2){
			// set cluster 0, the br, as reserved
			// set cluster 1, the FAT, as reserved
			fat[i] = 0xFFFE;
		} /*else if(i == (num_clusters - 1)){
			// last entry in the table
			fat[i] = 0xFFFF;
		}*/ else {
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
	seek_to_cluster(fs, boot_record.fat, boot_record.cluster_size);
	fwrite(&fat, 1, sizeof(fat), fs);
	
	// write the root directory
	seek_to_cluster(fs, boot_record.root_dir, boot_record.cluster_size);
	fwrite(&root_dir, 1, boot_record.cluster_size, fs); 

	// read it back out to test
	/*seek_to_cluster(fs, 2, boot_record.cluster_size);

	struct directory_entry tmp_root[boot_record.cluster_size / sizeof(struct directory_entry)];
	printf("tmp_root size: %d\n", sizeof(tmp_root));
	
	//tmp_root = (struct directory_entry *) malloc(boot_record.cluster_size / sizeof(struct directory_entry));

	fread(&tmp_root, sizeof(tmp_root), 1, fs);

	// read back fat to test
	seek_to_cluster(fs, boot_record.fat, boot_record.cluster_size);
	int new_fat[calc_num_clusters(boot_record.size, boot_record.cluster_size)];
	printf("Size of new_fat: %d\n", sizeof(new_fat));
	fread(&new_fat, sizeof(new_fat), 1, fs);

	for(int i = 0; i < calc_num_clusters(boot_record.size, boot_record.cluster_size); i++){
		printf("Fat[%d]: %x\n", i, new_fat[i]);
	}
	*/

	fclose(fs);
	return 1;
}

void seek_to_beginning(FILE *file){
	if(fseek(file, 0, SEEK_SET) < 0){
		fprintf(stderr, "Seek erorr: %s\n", strerror(errno));
	}
}

void seek_to_cluster(FILE *file, int cluster_num, unsigned int cluster_size){
	int offset = calc_cluster_offset(cluster_num, cluster_size);

	if(fseek(file, offset, SEEK_SET) < 0){
		fprintf(stderr, "Error seeking to cluster: %s\n", strerror(errno));
	}
}

int calc_num_clusters(unsigned int fs_size, unsigned int cluster_size){
	return fs_size/cluster_size;
}

int calc_cluster_offset(int cluster_number, unsigned int cluster_size){
	return (cluster_number * cluster_size);
}

struct fs *open_fs(char *fs_name){
	struct fs *file_system;
	file_system = (struct fs*) malloc(sizeof(struct fs));

	file_system->fs = fopen(fs_name, "r+");

	seek_to_beginning(file_system->fs);

	fread(&file_system->boot_record, 1, sizeof(struct br), file_system->fs);

	return file_system;
}

FILE *create_empty_file(char *file_name, unsigned int size){
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

int available_clusters(struct fs *file_system){
	// get the FAT

	seek_to_cluster(file_system->fs, 
					file_system->boot_record.fat, 
					file_system->boot_record.cluster_size);

	int num_clusters = calc_num_clusters(file_system->boot_record.size, 
										 file_system->boot_record.cluster_size);
	int fat[num_clusters];

	fread(&fat, 1, sizeof(fat), file_system->fs);
	
	int free_clusters = 0;

	for(int i = 0; i < num_clusters; i++){
		if(fat[i] == 0x00){
			free_clusters++;
		}
	}

	return free_clusters;
}

struct dir_wrap *get_directory_table(struct fs *file_system){
	seek_to_cluster(file_system->fs,
					file_system->boot_record.root_dir,
					file_system->boot_record.cluster_size);

	 
	int num_files = file_system->boot_record.cluster_size / sizeof(struct directory_entry);
	
	struct directory_entry *root_dir;
	root_dir = (struct directory_entry *) malloc(sizeof(struct directory_entry) * num_files);

	fread(root_dir, 1, sizeof(struct directory_entry) * num_files, file_system->fs);

	struct dir_wrap *dir;
	dir = (struct dir_wrap*) malloc(sizeof(struct dir_wrap));

	dir->dir = root_dir;
	dir->num_files = num_files;
	
	return dir;
}

char *get_file(struct fs *file_system, char *file_name){
	
	struct dir_wrap *root_dir;
	root_dir = get_directory_table(file_system);
	
	char *fbuff;
	fbuff = NULL;

	printf("looking for %s\n", file_name);
	for(int i = 0; i < root_dir->num_files; i++){
		// if the first byte of the filename isnt 0x00, then check its name
		if(root_dir->dir[i].name[0] != 0x00 && root_dir->dir[i].name[0] != 0xFF){
			printf("comparing file names\n");
			// compare file names
			printf("Current file: %s\n", root_dir->dir[i].name); 
			int cmp = strcmp(file_name, root_dir->dir[i].name);
			printf("strcmp: %d\n", cmp);
			
			if(cmp == 0){
				printf("Compare valid\n");
				// make sure its not a directory
				
				if(root_dir->dir[i].type == 0x0000){
					// create a buffer the size of the file
					printf("File found: %s\n", file_name);
					// get the file from the file system
					fbuff = read_fs_for_file(file_system, root_dir->dir[i]);

				}
			} else {
				printf("no match\n");
			}
		}
	}

	return fbuff;
}

char *read_fs_for_file(struct fs *file_system, struct directory_entry file){
	struct fat_wrap *fatty;
	fatty = (struct fat_wrap*) malloc(sizeof(struct fat_wrap));

	fatty = get_fat(file_system);

	int current_index = file.index;
	int size_remaining = file.size;

	char *file_buff;

	file_buff = (char *) malloc(sizeof(char) * file.size + 1);
	
	// loop to piece together the blocks of the file from the file system
	while(size_remaining > 0){
		seek_to_cluster(file_system->fs, current_index, file_system->boot_record.cluster_size);

		char *tmp_buff;
		
		int bytes_to_read = -1;

		if(size_remaining > file_system->boot_record.cluster_size){
			tmp_buff = (char *) malloc(file_system->boot_record.cluster_size);
			bytes_to_read = file_system->boot_record.cluster_size;
			size_remaining -= file_system->boot_record.cluster_size;
		} else {
			tmp_buff = (char *) malloc(size_remaining);
			bytes_to_read = size_remaining;
			size_remaining = 0;
		}

		fread(tmp_buff, 1, bytes_to_read, file_system->fs);

		strcat(file_buff, tmp_buff);

		if(size_remaining > 0){
			current_index = fatty->fat[current_index];
		}
	}

	return file_buff;
}

struct fat_wrap *get_fat(struct fs *file_system){
	seek_to_cluster(file_system->fs,
					file_system->boot_record.fat,
					file_system->boot_record.cluster_size);

	int num_clusters = calc_num_clusters(file_system->boot_record.size,
										 file_system->boot_record.cluster_size);

	int *fat;
	fat = (int *) malloc(sizeof(int) * num_clusters);

	fread(fat, 1, sizeof(int) * num_clusters, file_system->fs);

	struct fat_wrap *fatty;
	fatty = (struct fat_wrap*) malloc(sizeof(struct fat_wrap));

	fatty->fat = fat;
	fatty->fat_size = num_clusters;

	return fatty;
}

void create_file(struct fs *file_system, char *file_name, char *file_content, unsigned int file_size, unsigned int file_type){	

	struct directory_entry new_file;

	if((strlen(file_name) + 1) > 112){
		perror("File name too long\n");
		return;
	}
	
	// build the new struct
	strcpy(new_file.name, file_name);
	new_file.size = file_size;
	new_file.type = file_type;
	new_file.creation = time();

	insert_file(file_system, new_file, file_content);
}

int get_next_available_cluster(struct fat_wrap *fatty){
	
	int cluster = -1;
	for(int i = 0; i < fatty->fat_size; i++){
		if(fatty->fat[i] == 0x0000){
			cluster = i;
			break;
		}
	}

	return cluster;

}

void write_to_cluster(struct fs *file_system, unsigned int cluster_num, char *cluster_content, int content_size){
	seek_to_cluster(file_system->fs,
					cluster_num,
					file_system->boot_record.cluster_size);
	printf("Write file to cluster #%d\nWrite %d bytes\n", cluster_num, content_size);
	fwrite(cluster_content, 1, content_size, file_system->fs);

}

void add_file_to_dir(struct fs *file_system, struct directory_entry new_file){
	struct dir_wrap *root_dir;
	root_dir = get_directory_table(file_system);

	for(int i = 0; i < root_dir->num_files; i++){
		if(root_dir->dir[i].name[0] == 0x00){
			root_dir->dir[i] = new_file;
			break;
		} 
	}

	save_directory_tree(file_system, root_dir->dir);
	
}

void insert_file(struct fs *file_system, struct directory_entry new_file, char *file_content){
	struct fat_wrap *fatty;
	fatty = (struct fat_wrap*) malloc(sizeof(struct fat_wrap));

	fatty = get_fat(file_system);
	
	// find the first available fat entry
	int index_cluster = get_next_available_cluster(fatty);
	
	new_file.index = index_cluster;

	int size_remaining = new_file.size;

	printf("file size: %d\n", new_file.size);
	printf("cluster size: %d\n", file_system->boot_record.cluster_size);
	
	int current_index = new_file.index;
	while(size_remaining > 0){
		int bytes_to_write = 0;

		if(size_remaining < file_system->boot_record.cluster_size){
			bytes_to_write = size_remaining;
			size_remaining = 0;
		} else {
			bytes_to_write = file_system->boot_record.cluster_size;
			size_remaining -= file_system->boot_record.cluster_size;
		}

		write_to_cluster(file_system, current_index, file_content, bytes_to_write);
		
		if(size_remaining > 0){
			// move the pointer in the content to the next cluster size
			file_content += (file_system->boot_record.cluster_size / sizeof(char));
			
			// get a new index in the fat table
			// reserve the current one so that we dont repeat it
			fatty->fat[current_index] = 0xfffe;
			int new_index = get_next_available_cluster(fatty);
			fatty->fat[current_index] = new_index;
			current_index = new_index;
		} else {
			// since we dont have anything left, mark the current index as the end
			fatty->fat[current_index] = 0xffff;
		}
	}

	save_fat(file_system, fatty);
	add_file_to_dir(file_system, new_file);
}

void save_fat(struct fs *file_system, struct fat_wrap *fatty){
	seek_to_cluster(file_system->fs, 
					file_system->boot_record.fat, 
					file_system->boot_record.cluster_size);
	printf("saving fat\n\tsize: %d\n", fatty->fat_size);
	fwrite(fatty->fat, 1, sizeof(int) * fatty->fat_size, file_system->fs);
}

void save_directory_tree(struct fs *file_system, struct directory_entry *dir){
	seek_to_cluster(file_system->fs, 
					file_system->boot_record.root_dir, 
					file_system->boot_record.cluster_size);

	fwrite(dir, 1, file_system->boot_record.cluster_size, file_system->fs);
}

void _print_fat(struct fs *file_system){
	struct fat_wrap *fatty;
	fatty = (struct fat_wrap*) malloc(sizeof(struct fat_wrap));

	fatty = get_fat(file_system);
	
	for(int i = 0; i < fatty->fat_size; i++){	
		printf("Fat[%d]: %x\n", i, fatty->fat[i]);
	}

}

void _print_dir(struct fs *file_system){
	struct dir_wrap *root_dir;
	root_dir = get_directory_table(file_system);

	for(int i = 0; i < root_dir->num_files; i++){
		if(root_dir->dir[i].name[0] == 0x00){
			printf("File[%d]: %x\n", i, root_dir->dir[i].name[0]);
		} else {
			printf("File[%d]: %s\n", i, root_dir->dir[i].name);
		}
	}

}

void update_timestamp(struct fs *file_system, char *file_name){
	struct dir_wrap *dir = get_directory_table(file_system);

	for(int i = 0; i < dir->num_files; i++){
		if(dir->dir[i].name[0] != 0x00 && dir->dir[i].name[0] != 0xFF){
			int cmp = strcmp(file_name, dir->dir[i].name);
			
			if(cmp == 0){
				dir->dir[i].creation = time();
				save_directory_tree(file_system, dir);
			}
		}
	}
}

void close_fs(struct fs *file_system){

	fclose(file_system->fs);
}
