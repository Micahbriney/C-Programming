#ifndef HTH_
#define HTH_

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE_   4096
#define ARRSIZECHAR_  256
#define ARRSIZECOUNT_ 1
#define ENCODESIZE_   (ARRSIZECHAR_ + (ARRSIZECHAR_ - 1))
#define T_LEFT  '0'
#define T_RIGHT '1'


typedef struct hnode_ *listptr;
typedef struct hnode_ *treeptr;
typedef struct hnode_ *hnodeptr;

typedef struct hnode_{
  int32_t count;      /* Total count character was found*/
  int c;          /* The chararacter as an int*/
  hnodeptr next;  /* Next ptr for list mode*/
  hnodeptr left;  /* Left child pointer for binary tree*/
  hnodeptr right; /* Right child pointer for binary tree*/
  char *codestr;  /* String of the character's code*/
}hnode;

/* -----------   Operation functions  ---------------- */

/* Histogram Functions*/
int get_uniqnum(uint32_t freqArr[][ARRSIZECOUNT_]);
void create_histogram(hnode histogram[]);
void build_histogram(hnode histogram[],
		     uint32_t freqArr[][ARRSIZECOUNT_]);

/* List Functions*/
listptr init_list(hnode histogram[], int uniqchars);
int compare_count(const void *curr, const void *next);
void insert_node(listptr newNode);
void insert_node_location(listptr newNode, listptr head);
listptr truncate_list(listptr head, int limit);
void free_list(listptr head);

/* Binary Tree functions*/
treeptr init_tree(listptr head);
treeptr create_node(void);
treeptr create_super_node(listptr curr, listptr next);
void insert_super_node(treeptr newSuperNode, listptr head);
void encode_post_order_traversal(treeptr root, int index, char encodeArr[]);
void set_encode_str(treeptr root, int size, char encodeArr[]);

/* IO function*/
void read_input(int infd, uint32_t freqArr[][ARRSIZECOUNT_]);
void print_bits(treeptr root, char encodedArr[]);
/* void print_tree_bits(); */
void print_tree_table_code(treeptr node);
void usage_hdecode_error(char *prog);
void usage_hencode_error(char *prog);
void usage_htable_error(char *prog);


/* Testing function*/
void print_pre_order_traversal(treeptr root,
			       void (*print_tree_node)(treeptr node));
void print_tree_node_full(treeptr node); /* For print tree nodes*/
void print_ordered_list(hnode histogram[]);
void print_structures(treeptr root, hnode histogram[]);


#endif
