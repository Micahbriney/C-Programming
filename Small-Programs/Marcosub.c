#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Working*/
#define HEIGHT(root) ((root) == (NULL)) ? (0) : (root->height)
/* usage: printf("First - Left word : %s\n", (char *)((Treeptr)(SMALLESTWORD(first))->word));*/
/* To avoid warnings need to cast SMALLESTWORD to Treeptr then cast ALL of it to a char **/
/* changed from (root)->left to (root->left) seems to have fixed the casting issues. 
    just watch the () when using*/
/* Traverse to smallest word. Working*/
#define SMALLESTWORD(root) ((root) == (NULL)) ? (root) : (root->left)

/* tree node struct and pointer. Working*/
/* Usage: printf("BALANCEFACTOR test: %d\n", (BALANCEFACTOR(first)));*/
#define BALANCEFACTOR(root) ((root) == (NULL)) ? \
                            (0) : ((HEIGHT(root->left)) - (HEIGHT(root->right)))

typedef struct Treenode_ *Treeptr;

typedef struct Treenode_{
  Treeptr left;  /* Child node that is lexicographically smaller*/
  Treeptr right; /* Child node that is lexicographically larger*/
  char *word;    /* String stored at this node*/
  int wordcount; /* The number of times the word was found*/
  int height;    /* The height of the node*/
} Treenode;

Treeptr create(char *word);


int main()
{
    
    Treeptr first;
    Treeptr second;
    Treeptr third;
    
    char *word1 = "Hello";
    char *word2 = "World";
    
    first  = create(word1);
    second = create(word2);
    
    first->left = second;
    
    printf("First Word : %s\n", first->word);
    printf("Second Word : %s\n", second->word);

    void * test = SMALLESTWORD(first);
    printf("First - Left word : %s\n", ((SMALLESTWORD(first))->word));
    
    printf("Height First : %d\n", HEIGHT(first));
    printf("Height first left : %d\n", HEIGHT(first->left));
    printf("Height first right : %d\n", HEIGHT(first->right));
    printf("BALANCEFACTOR test: %d\n", (BALANCEFACTOR(first)));
    first->left->height = 2;
    printf("Height first left : %d\n", HEIGHT(first->left));
    printf("BALANCEFACTOR test: %d\n", (BALANCEFACTOR(first)));
    return 0;
}

Treeptr create(char *word){
    Treeptr node;

    node = (Treeptr)malloc(sizeof(Treenode));
    
    if(!node){ /* If still null then new node malloc failed*/
        perror("create node malloc failed");
        exit(EXIT_FAILURE);
    } /* When root created initilized node*/
    
    node->left = NULL;  /* New nodes won't have leaves*/
    node->right = NULL; /* New nodes won't have leaves*/
    
    /* Allocate some space for node->word*/
    node->word = (char *)malloc(strlen(word) + 1);
    
    if(!(node->word)){
        perror("create node->word malloc failed");
        exit(EXIT_FAILURE);
    }
    
    // node->word = word;
    strcpy(node->word, word); /* Copy word parameter to root wood*/
    
    node->wordcount = 1; /* Set node count to 1*/
    node->height = 1;
    return node;
    

}
