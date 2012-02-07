/**
 * @author Sean McGary <spm8975@cs.rit.edu> 
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "input.h"
#include "run_command.h"
#include "history.h"


#define INPUT_BUFFER_SIZE	64

void read_input(struct history *hist){
	char *input_buffer;
	int num_bytes = 0;
	int c;
	int buffer_pos = 0;

	input_buffer = (char *) malloc(sizeof(char) * 500);
	
	while(1){
		
		while(printf("> "), fgets(input_buffer, 500, stdin)){
			
			if(strlen(input_buffer) > 1){
				// create a new string with length of the max buffer size + 1 (null byte)
				char *command = (char *) malloc(sizeof(char) * INPUT_BUFFER_SIZE + 1);
			
				// remove the new line
				char *formatted_input = (char *) malloc(sizeof(char) * 500);
			
				if((strlen(input_buffer) - 1) < INPUT_BUFFER_SIZE){
				
					strncpy(formatted_input, input_buffer, strlen(input_buffer) - 1);
					process_input(formatted_input, hist);
				} else {
					// print to stderr that the users exceeded the buffer size
					fprintf(stderr, "\nExceeded input buffer size\n");
				}
			}
		}

	}
}

void process_input(char *input, struct history *hist){
	struct hist_node *new_node = (struct hist_node*) malloc(sizeof(struct hist_node));
	
	new_node->command = (char *) malloc(sizeof(char) * strlen(input) + 1);
	strcpy(new_node->command, input);
	
	// parse out the input into a list to send to exec
	int arg_count = 1;
	for(int i = 0; i < strlen(input); i++){
		if((int)input[i] == 32){
			arg_count++;
		}
	}
	
	char **arg_list = (char **) malloc(sizeof(char *) * arg_count);
	char *command;
	// count the number of arguments
	char *token = strtok(input, " ");
	
	arg_count = 0;
	while(token != NULL){
		arg_list[arg_count] = token;
		token = strtok(NULL, " ");
		arg_count++;
	}

	command = arg_list[0];
	
	char *hist_command = "history";
	
	if(strcmp(command, hist_command) == 0){
		hist->print_hist(hist);
	} else {
		// add the command to the history

		hist->insert(hist, new_node);

		run_command(command, arg_list);
	}
}
