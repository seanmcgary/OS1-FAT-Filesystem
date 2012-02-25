#ifndef _FS_H
#define _FS_H

// filesystem attributes in MB
#define MAX_FS_SIZE 		50
#define MIN_FS_SIZE			5
// cluster size in KB
#define MIN_CLUSTER_SIZE	8
#define MAX_CLUSTER_SIZE	16

#define KILOBYTE			1024
#define MEGABYTE			1048576

#define FREE_CLUSTER 		0x0000
#define RESERVED_CLUSTER	0xFFFE
#define LAST_CLUSTER		0xFFFF


// Boot record
struct br {
	unsigned int cluster_size;
	unsigned int size;
	unsigned int root_dir;
	unsigned int fat;
};

// An entry in the directory table
struct directory_entry {
	char name[112];
	unsigned int index;
	unsigned int size;
	unsigned int type;
	unsigned int creation;
};

struct fs {
	struct br boot_record;
	FILE *fs;
};

struct fat_wrap {
	int *fat;
	int fat_size;
};

struct dir_wrap {
	struct directory_entry *dir;
	int num_files;
};
	

// Create a new filesystem with a boot record and FAT. Prompts
// user for specifications of the fs.
int create_fs(char *fs_name);

// Creates a file with a specified file name and size
FILE *create_empty_file(char *file_name, unsigned int size);

// Seek to the beginning of the file
void seek_to_beginning(FILE *file);

// seek to a cluster in the filesystem
void seek_to_cluster(FILE *file, int cluster_num, unsigned int cluster_size);

// calculate the number of clusters in the FS
int calc_num_clusters(unsigned int fs_size, unsigned int cluster_size);

// get the byte offset for the cluster
int calc_cluster_offset(int cluster_number, unsigned int cluster_size);

// Opens a file system for manipulation. Reads the boot record into memory,
// returning a struct with the boot record and a pointer to the open file
struct fs *open_fs(char *fs_name);

// get a file from the file system and return it as a buffer
char *get_file(struct fs *file_system, char *file_name);

// traverses filesystem, piecing together a file from clusters
char *read_fs_for_file(struct fs *file_system, struct directory_entry file);

// get the FAT from disk
struct fat_wrap *get_fat(struct fs *file_system);

// create a new file
void create_file(struct fs *file_system, char *file_name, char *file_content, unsigned int file_size, unsigned int file_type);

// actually insert the file into the filesystem
void insert_file(struct fs *file_system, struct directory_entry new_file, char *file_content);

// write data to a cluster
void write_to_cluster(struct fs *file_system, unsigned int cluster_num, char *cluster_content, int content_size);

// utility function to grab the next available entry in the FAT
int get_next_available_cluster(struct fat_wrap *fatty);

// reads the directory table from disk into memory
struct dir_wrap *get_directory_table(struct fs *file_system);

// find the next available file in the directory table and add it
void add_file_to_dir(struct fs *file_system, struct directory_entry new_file);

void update_timestamp(struct fs *file_system, char *file_name);

// debug utility to print the FAT as it is on the disk
void _print_fat(struct fs *file_system);

// debug utility to print the root directory as it is on the disk
void _print_dir(struct fs *file_system);

#endif
