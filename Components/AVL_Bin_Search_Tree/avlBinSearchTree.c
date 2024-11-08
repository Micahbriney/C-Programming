/*
  Source: avlbinsearchtree.c
  Description: AVL Binary Search Tree is a self balaning tree. AVL is a 
               tree that prevents a skew tree from forming. It does this
               by using "rotations" to maintain a max height difference of
               (+1 or -1) at every node. The AVL tree utilizes four
               different rotations; left, right, left-right, and 
               right-left. The height of a node is the following:
               h = max{height(left_child), height(right_child)} + 1
	       
*/

#include <stddef.h>
#include "avlBinSearchTree.h"

#define LEFT_HEAVY 1
#define RIGHT_HEAVY -1
#define BALANCED 0
#define INIT_TREE 3
#define INCREMENT_TREE 2

static Treeptr createNode(char *word);
static Treeptr balanceRoot(Treeptr root);
static Treeptr leftRotate(Treeptr root);
static Treeptr rightRotate(Treeptr root);
static void newHeight(Treeptr root);

int uniqwords = 0; /* Number of uniq words found*/
int largestwordcount = 0; /* Largest count of words*/

/* Binary search tree function called with first word*/
Treeptr avlBstInit(void){
  Treeptr root;
  
  root = (Treeptr)malloc(sizeof(Treenode));

  if(!root){ /* If still null then root malloc failed*/
    perror("New root malloc failed");
    exit(EXIT_FAILURE);
  } /* When root created initilized node*/

  root->left = NULL;        /* Init node doesn't have children*/
  root->right = NULL;
  root->word = NULL;        /* Init node does not have a word*/
  root->wordcount = 0;      /* Init node starts with count of 0*/
  root->height = 1;         /* Init node starts with height of 1*/

  return root;
}

static Treeptr createNode(char *word){
  Treeptr node;
  
  node = (Treeptr)malloc(sizeof(Treenode));
  if(!node){ /* If still null then new node malloc failed*/
    perror("new node malloc failed");
    exit(EXIT_FAILURE);
  } /* When root created initilized node*/
  node->left = NULL;  /* New nodes won't have leaves*/
  node->right = NULL; /* New nodes won't have leaves*/

  /* Make room for word in struct using strlen + 1(for null terminator)*/
  node->word = (char *)malloc(strlen(word) + 1);
  if(!(node->word)){ /* If still null then malloc for word failed*/
    perror("createNode node->word malloc failed");
    exit(EXIT_FAILURE);
  } /* When root created initilized node*/
  strcpy(node->word, word); /* Copy word parameter to root wood*/
  node->wordcount = 1; /* Set node count to 1*/
  node->height = 1;
  uniqwords++;
  
  return node;
}

/*
 *function: insertNode
 *brief:    TODO add brief
*/
Treeptr insertNode(Treeptr root, char *word){
  int strdiff;

  if(!root->word){
    root->word = (char *)malloc(strlen(word) + 1);
    if(!(root->word)){ /* If still null then malloc for word failed*/
      perror("insertNode root->word malloc failed");
      exit(EXIT_FAILURE);
    } /* When root created initilized node*/
    strcpy(root->word, word); /* Copy word parameter to root wood*/
    root->wordcount++;
    return root;
  }
  
  strdiff = strcmp(word, root->word);
  
  /* Check if root already exists*/
  if(strdiff == 0)
    root->wordcount++;
  if(root->wordcount > largestwordcount)
    largestwordcount = root->wordcount;
  else if(strdiff > 0){/* Add new word right if larger*/
    if(root->right) /* If the right child exists, traverse right*/
      root->right = insertNode(root->right, word);
    else /* If the right child does not exist. Make a new node*/
      root->right = createNode(word);
  }
  else if(strdiff < 0){ /* Add new word left if smaller*/
    if(root->left) /* If the left child exists, traverse left*/
      root->left = insertNode(root->left, word);
    else /* If the left child does not exist. Make a new node*/
      root->left = createNode(word);
  }

  newHeight(root);
  root = balanceRoot(root);
  
  return root;
}


/*
 * function: balanceRoot
 * brief: When the root is unbalanced check how to rebalance the root by
 *         rotating the root and children. There are four rotations: LL,
 *         LR, RR, RL. LL is when the root is left heavy and the child is
 *         left heavy. To fix perform a right rotation. LR is when the
 *         root is left heavy but its child is right heavy. Two rotations
 *         need to be performed. First a left rotation around the left 
 *         child. Then a right rotation around the root. RR is the inverse
 *         of LL and RL is the inverse of LR.
*/
static Treeptr balanceRoot(Treeptr root){
  int balancefactor;
  int bfleftchild = BFACTOR(root->left);
  int bfrightchild = BFACTOR(root->right);
  
  balancefactor = BFACTOR(root);
  if(balancefactor > LEFTHEAVY || balancefactor < RIGHTHEAVY){
    if(balancefactor > LEFTHEAVY && bfleftchild >= 0)
      root = rightRotate(root);
    else if(balancefactor > LEFTHEAVY){
      root->left = leftRotate(root->left);
      root = rightRotate(root);
    }
    else if(balancefactor < RIGHTHEAVY && bfrightchild <= 0)
      root = leftRotate(root);
    else{
      root->right = rightRotate(root->right);
      root = leftRotate(root);
    }
  }
  return root;
}

/*
 * function: leftRotate
 * brief: When the root is right heavy and left child is
 *         either left/right heavy
*/
static Treeptr leftRotate(Treeptr root){
  Treeptr temproot; 

  if(!root || !root->right) /* No rotation possible*/
    return root;
  
  /* Perform left rotation about root node*/
  temproot = root->right; /* Hold right child*/
  root->right = temproot->left;
  temproot->left = root;

  /* Hights were changed*/
  newHeight(root); /* root is lower than temproot. So change first*/
  newHeight(temproot);

  return temproot;
}


/*
 * function: rightRotate
 * brief: This is the inverse rotation of the leftRotate. This will 
 *         change the root when it is left heavy and left child is
 *         either left/right heavy
*/
static Treeptr rightRotate(Treeptr root){
  Treeptr temproot; 

  if(!root || !root->left) /* No rotation possible*/
    return root;
  
  /* Perform left rotation about root node*/
  temproot = root->left; /* Hold left child*/
  root->left = temproot->right;
  temproot->right = root;

  /* Hights were changed*/
  newHeight(root);    /* root is lower than temproot. So change first*/
  newHeight(temproot);

  return temproot;
}


/*
 *function: newHeight
 *brief: When rotations occur the heigh will need to be fixed.
 *        This is done by comparing the path heights of the children
          returning the max and incrementing it by 1 for the current node.
*/
static void newHeight(Treeptr root){
  if(root)
    root->height = MAX(HEIGHT(root->left), HEIGHT(root->right));
}


Treeptr traverseLeft(Treeptr root){
  Treeptr leftnode;
  leftnode = root->left;
  if(leftnode) /* If left node exists*/
    return leftnode;
  else
    return root;
}

int getUniqWords(){
  return uniqwords;
}

int getLargestWordCount(){
  return largestwordcount;
}

void decrementLargestWordCount(){
  largestwordcount--;
}

Treeptr findSmallestRoot(Treeptr root){
  return (root->left != NULL) ? findSmallestRoot(root->left) : (root);
}


void createInorderList(Treeptr root, int numwords,
		       void (*newNode)(Treeptr root, int numwords,
					     int maxcount)){
   if(root){
     if(root->left)
       createInorderList(root->left, numwords, newNode);

     newNode(root, numwords, largestwordcount);
     
     if(root->right)
       createInorderList(root->right, numwords, newNode);
     }
   return;
 }

/* Below is Not working*/

/* WordListPtr createInorderList(WordListPtr head, Treeptr root, */
/* 			      WordListPtr(*newNode)(char *word, int count)){ */
/*   WordListPtr nextnode; */
/*   if(root){ */
/*     if(root->left){  /\* If left child found then traverse it*\/ */
/*       if(strcmp(root->left->word, head->word) == 0) */
/* 	head->next = newNode(root->word, root->wordcount); */
/*       else */
/* 	nextnode = createInorderList(head, root->left, newNode); */
/*     } */
/*     /\* if(!head->next) *\/ */
/*     /\*   head->next = nextnode; *\/ */
/*     nextnode = newNode(root->word, root->wordcount); /\* No leftchild *\/ */
/*     if(root->right) /\* Left child not found. So travere right if able to*\/ */
/*       nextnode = createInorderList(head, root->right, newNode); */
/*   } */
/*   return head;      /\* Only return when no more children are found*\/ */
/* } */
