#ifndef _FS_H
#define _FS_H

// filesystem attributes in MB
#define MAX_FS_SIZE 		50
#define MIN_FS_SIZE			5
// cluster size in KB
#define MIN_CLUSTER_SIZE	8
#define MAX_CLUSTER_SIZE	16

#define MEGABYTE			1048576

#define FREE_CLUSTER 		0x0000
#define RESERVED_CLUSTER	0xFFFE
#define LAST_CLUSTER		0xFFFF

/**
 * Boot record
 */

struct br {
	unsigned int cluster_size;
	unsigned int size;
	unsigned int root_dir;
	unsigned int fat;
};

/**
 * Create a new filesystem with a boot record and FAT. Prompts
 * user for specifications of the fs.
 */
int create_fs(char *fs_name);

/**
 * Opens a file system for manipulation. Reads the boot record and
 * fat into memory.
 */
int open_fs(char *fs_name);

/**
 * Creates a file with a specified file name and size
 */
FILE *create_empty_file(char *file_name, int size);

/**
 * Seek to the beginning of the file
 */
void seek_to_beginning(FILE *file);

#endif
