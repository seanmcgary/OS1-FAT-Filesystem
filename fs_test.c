#include <stdio.h>
#include <stdlib.h>
#include "fs.h"

static char *test_content = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer at lacinia purus. Etiam ipsum nulla, congue quis placerat blandit, facilisis id massa. Suspendisse vel mattis dolor. Praesent egestas tincidunt justo ut hendrerit. Integer malesuada sagittis urna, ut ultricies orci fermentum a. Maecenas eros diam, elementum viverra mollis et, porta nec erat. Ut convallis porttitor tellus et euismod. Nunc ligula lectus, luctus sed sagittis eu, sollicitudin et purus. Praesent tortor nunc, laoreet quis dictum vitae, condimentum nec erat. Vestibulum magna massa, tincidunt sed tincidunt eget, fermentum id tellus. Ut eget sapien neque, a consequat est. Aenean eu nunc eu libero pulvinar tincidunt sed ac tortor. Nunc malesuada vestibulum tellus vitae lobortis. Integer varius condimentum dui vitae viverra. Nam vitae porttitor massa.Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer at lacinia purus. Etiam ipsum nulla, congue quis placerat blandit, facilisis id massa. Suspendisse vel mattis dolor. Praesent egestas tincidunt justo ut hendrerit. Integer malesuada sagittis urna, ut ultricies orci fermentum a. Maecenas eros diam, elementum viverra mollis et, porta nec erat. Ut convallis porttitor tellus et euismod. Nunc ligula lectus, luctus sed sagittis eu, sollicitudin et purus. Praesent tortor nunc, laoreet quis dictum vitae, condimentum nec erat. Vestibulum magna massa, tincidunt sed tincidunt eget, fermentum id tellus. Ut eget sapien neque, a consequat est. Aenean eu nunc eu libero pulvinar tincidunt sed ac tortor. Nunc malesuada vestibulum tellus vitae lobortis. Integer varius condimentum dui vitae viverra. Nam vitae porttitor massa.Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer at lacinia purus. Etiam ipsum nulla, congue quis placerat blandit, facilisis id massa. Suspendisse vel mattis dolor. Praesent egestas tincidunt justo ut hendrerit. Integer malesuada sagittis urna, ut ultricies orci fermentum a. Maecenas eros diam, elementum viverra mollis et, porta nec erat. Ut convallis porttitor tellus et euismod. Nunc ligula lectus, luctus sed sagittis eu, sollicitudin et purus. Praesent tortor nunc, laoreet quis dictum vitae, condimentum nec erat. Vestibulum magna massa, tincidunt sed tincidunt eget, fermentum id tellus. Ut eget sapien neque, a consequat est. Aenean eu nunc eu libero pulvinar tincidunt sed ac tortor. Nunc malesuada vestibulum tellus vitae lobortis. Integer varius condimentum dui vitae viverra. Nam vitae porttitor massa.Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer at lacinia purus. Etiam ipsum nulla, congue quis placerat blandit, facilisis id massa. Suspendisse vel mattis dolor. Praesent egestas tincidunt justo ut hendrerit. Integer malesuada sagittis urna, ut ultricies orci fermentum a. Maecenas eros diam, elementum viverra mollis et, porta nec erat. Ut convallis porttitor tellus et euismod. Nunc ligula lectus";

int main(int argc, char *argv[]){
	printf("Hello\n");

	char *fs_name = "some_fs.filesystem";
	
	struct fs *file_system;
	
	create_fs(fs_name);

	file_system = open_fs(fs_name);
	
	int free_clusters = available_clusters(file_system);

	printf("Free clusters: %d\n", free_clusters);

	get_fat(file_system);

	char *new_file = "My_File";

	char *large_file;
	large_file = (char *) malloc((strlen(test_content) * 4) + 1);

	for(int i = 0; i < 4; i++){
		strcat(large_file, test_content);
	}
	
	printf("File size: %d\n", sizeof(char) * strlen(large_file)); 

	create_file(file_system, new_file, large_file, sizeof(char) * strlen(large_file), 0x0000);
	
	//_print_fat(file_system);
	//_print_dir(file_system);
	char *noob;

	noob = get_file(file_system, "My_File");
	
	if(noob == NULL){
		printf("file doesnt exist\n");
	} else {
		printf("%s", noob);
	}

	return 0;
}
