#ifndef AVLH
#define AVLH

#include "wordList.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Getter Macros*/
/* MAX returns max height between left and right node then adds 1*/
#define MAX(a,b) (((a)>(b))?(a):(b)) + 1
/* HEIGHT returns 0 if node is null otherwise return the node's height*/
#define HEIGHT(root) ((root) == (NULL)) ? (0) : ((root)->height)
/* Get the balance factor at the called node*/
#define BFACTOR(root) ((root) == (NULL)) ? \
                       (0) : \
                       ((HEIGHT((root)->left)) - (HEIGHT((root)->right)))
#define LEFTHEAVY   1
#define RIGHTHEAVY -1

typedef struct Treenode_ *Treeptr;

typedef struct Treenode_{
  Treeptr left;  /* Child node that is lexicographically smaller*/
  Treeptr right; /* Child node that is lexicographically larger*/
  char *word;    /* String stored at this node*/
  int wordcount; /* The number of times the word was found*/
  int height;    /* The height of the node*/
} Treenode;

Treeptr avlBstInit(void);
Treeptr insertNode(Treeptr root, char *word);
int getUniqWords();
int getLargestWordCount();
void decrementLargestWordCount();

/* Traveral function*/
Treeptr findSmallestRoot(Treeptr root);
void preorder(Treeptr root, void (*visit)(Treeptr node));
void createInorderList(Treeptr root, int numwords,
		       void(*newNode)(Treeptr root,
				      int numwords,
				      int maxcount));





#define PRINT_TREE 1

#endif
