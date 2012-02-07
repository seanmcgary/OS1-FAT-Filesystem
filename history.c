/**
 * @author Sean McGary <spm8975@cs.rit.edu> 
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "history.h"

#define MAX_HIST_SIZE	20

/**
 * Insert a history node into the history linked list
 *
 */
void insert_hist_node(struct history *hist, struct hist_node *new_node){
	
	
	// if the size of the history is > 20, move the head to head->next, and append to the tail
	if(hist->hist_size == MAX_HIST_SIZE){
		hist->head = hist->head->next_node;
	}

	new_node->next_node = (struct hist_node*) malloc(sizeof(struct hist_node));
	new_node->prev_node = (struct hist_node*) malloc(sizeof(struct hist_node));

	// new node->prev == current tail
	new_node->prev_node = hist->tail;
	
	new_node->next_node = NULL;

	if(hist->hist_size == 0){
		hist->head = new_node;
	}


	// set the tails next node to the new node
	hist->tail->next_node = new_node;

	// make the tail the new node
	hist->tail = hist->tail->next_node;

	if(hist->hist_size < MAX_HIST_SIZE){
		hist->hist_size++;
	}
}

/**
 * Print the command of a history node.
 *
 */
void print_history_item(struct hist_node *node){
	printf("%s\n", node->command);

	if(node->next_node != NULL){
		print_history_item(node->next_node);
	}
}

/**
 * Iterate over the linked list of history items
 *
 */
void print_history(struct history *hist){
	printf("\n");

	// traverse the history
	if(hist->head != NULL){
		print_history_item(hist->head);
	}	
}


/**
 * Lets create a history object that will store the head and tail
 * of linked list items. Also store some function pointers to handle
 * printing of items as well as inserting them into the list.
 *
 */
struct history* init_history(){
	struct history *new_history = (struct history*) malloc(sizeof(struct history));
	new_history->insert = insert_hist_node;
	new_history->print_hist = print_history;
	
	new_history->head = (struct hist_node*) malloc(sizeof(struct hist_node));
	new_history->tail = (struct hist_node*) malloc(sizeof(struct hist_node));

	new_history->hist_size = 0;
	
	return new_history;
}

