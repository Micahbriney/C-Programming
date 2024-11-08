#ifndef HT
#define HT
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct h_node{
  struct h_node* next;  /* For h_list*/
  struct h_node* right; /* For h_tree*/
  struct h_node* left;  /* For h_tree*/
  int c;        /* Letter*/
  int num;      /* Number of letters*/
  int h_code;   /* huffman code*/
};

typedef struct h_node node;


#define ASCII 256


/* ----- Functions ----- */
char *fdErrorCheck(int fd_in, int *count);
node* createNode(int c);
void printList(node *head); /* For testing*/
node* shrinkList(node* head);
#endif


