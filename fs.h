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

// Create a new filesystem with a boot record and FAT. Prompts
// user for specifications of the fs.
int create_fs(char *fs_name);

// Opens a file system for manipulation. Reads the boot record and
// fat into memory.
int open_fs(char *fs_name);

// Creates a file with a specified file name and size
FILE *create_empty_file(char *file_name, int size);

// Seek to the beginning of the file
void seek_to_beginning(FILE *file);

// calculate the number of clusters in the FS
int calc_num_clusters(int fs_size, int cluster_size);

// get the byte offset for the cluster
int calc_cluster_offset(int cluster_number, int cluster_size);

// seek to a cluster in the filesystem
void seek_to_cluster(FILE *file, int cluster_num, int cluster_size);

#endif
