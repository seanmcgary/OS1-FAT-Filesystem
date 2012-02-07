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

void run_command(char *cmd, char **cmd_args){
	pid_t pid = fork();

	switch(pid){
		case -1: 
			// error
			break;
		case 0:
			// child
			execvp(cmd, cmd_args);
			break;
		default:
			//parent
			wait(pid);
			break;
	}

}
