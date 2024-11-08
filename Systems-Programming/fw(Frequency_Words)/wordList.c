
#include "avlBinSearchTree.h"
#include "wordList.h"

WordListPtr initWordList(){
  WordListPtr head;
  
  head = createListNode(NULL, 0);
  
  return head;
}

WordListPtr createListNode(char *word, int count){
  WordListPtr node;
  
  node = (WordListPtr)malloc(sizeof(List));

  if(!node){ /* If still Null then head malloc failed*/
    perror("New WordListPtr head failed to malloc");
    exit(EXIT_FAILURE);
  }
  /* Make space for word inside node->word*/
  if(word){
    node->word = (char *)malloc(strlen(word) + 1); 
    
    if(!node->word){ /* If Malloc failed*/
      perror("Node->word failed to malloc");
      exit(EXIT_FAILURE);
    }
    strcpy(node->word, word);
  }
  else /* createNode was called with a NULL*/
    node->word = NULL;
    
  node->count = count;

  return node;
}

WordListPtr insertListNode(char *word, int count){
  WordListPtr head;
  
  if(!word){
    fprintf(stderr, "Trying to insert a null Word into list.");
    exit(EXIT_FAILURE);
  }

  head = createListNode(word, count);

  return head;
}
