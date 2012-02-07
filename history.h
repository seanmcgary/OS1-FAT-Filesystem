/**
 * @author Sean McGary <spm8975@cs.rit.edu> 
 *
 */

#ifndef _HISTORY_H
#define _HISTORY_H

struct history;
struct hist_node;

void insert_hist_node(struct history *hist, struct hist_node *new_node);

typedef void (*_insert_hist_node)(struct history *hist, struct hist_node *new_node);

void print_history(struct history *hist);
void print_history_item(struct hist_node *node);

typedef void (*_print_history)(struct history *hist);

struct hist_node {
	char *command;
	struct hist_node *next_node;
	struct hist_node *prev_node;
};

struct history {
	_insert_hist_node insert;
	_print_history print_hist;
	struct hist_node *head;
	struct hist_node *tail;
	int hist_size;
};

struct history * init_history();

#endif
