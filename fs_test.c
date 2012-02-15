#include <stdio.h>
#include <stdlib.h>
#include "fs.h"

int main(int argc, char *argv[]){
	printf("Hello\n");
	create_fs("my_fancy_fs.filesystem");
	return 0;
}
