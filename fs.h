#ifndef _FS_H
#define _FS_H

// filesystem attributes in MB
#define MAX_FS_SIZE 		50
#define MIN_FS_SIZE			5
// cluster size in KB
#define MIN_CLUSTER_SIZE	8
#define MAX_CLUSTER_SIZE	16


#define FREE_CLUSTER 		0x0000
#define RESERVED_CLUSTER	0xFFFE
#define LAST_CLUSTER		0xFFFF

struct br {
	unsigned int cluster_size;
	unsigned int size;
	unsigned int root_dir;
	unsigned int fat;
};

// create a filesystem
int create_fs(char *fs_name);

int open_fs(char *fs_name);


#endif
