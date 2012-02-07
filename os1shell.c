/**
 * @author Sean McGary <spm8975@cs.rit.edu> 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include "input.h"
#include "history.h"

#define MIN_HIST_COUNT	20

struct history *shell_history;

/**
 * Handle Ctrl-C
 */
void sigint_handler(int signum){
	printf("\n");
	exit(0);
}

int main(int argc, char *argv[]){
	
	// lets handle signals in the correct POSIX way to work with c99
	signal(SIGINT, sigint_handler);
	
	shell_history = (struct history*) malloc(sizeof(struct history));

	shell_history = init_history();

	struct hist_node *test = (struct hist_node*) malloc(sizeof(struct hist_node));

	//test->command = "ls";

	//shell_history->insert(shell_history, test);
	
	//shell_history->print_hist(shell_history);
	

	read_input(shell_history);

}
