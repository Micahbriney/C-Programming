#include "htable.h"

#define CHUNK 8000

int main(int argc, char *argv[]){
  /* First get file using open*/
  char *buff;
  int *buffSize;
  int readIn;
  int fdIn;
  int fdOut;
  int size = CHUNK;
  int stdinFlag = 0;    /* 0 == no*/
  node *hHead = NULL;
  node *hNew = NULL;
  node *search = NULL;
  node *updateNode = NULL;
  int i, j;

  /* TODO remove trailing newline char from STDIN*/
  
  if(NULL == (buffSize = (int*)malloc(sizeof(int)))){
    perror("malloc buffSize");
    exit(EXIT_FAILURE);
  }

  /* if(NULL == (buff = (char*)(malloc(sizeof(char) * CHUNK)))){ */
  /*   perror("malloc buffSize"); */
  /*   exit(EXIT_FAILURE); */
  /* } */

  /* Optimize using optget later*/
  if(argc == 1){ /* Use stdin*/
    buff = fdErrorCheck(fdIn = STDIN_FILENO,
			buffSize); /* open stdin, check for errors*/
  }
  else if(argc == 2){   /* Use input file or stdin if '-' and stdout*/
    if(*argv[1] == '-'){ /* Use stdin*/
      buff = fdErrorCheck(fdIn = STDIN_FILENO,
			  buffSize); /*open stdin, check for errors*/
      stdinFlag = 1;

    }
    else{ /* open file*/
      buff = fdErrorCheck(fdIn = open(argv[1], O_RDONLY),
			  buffSize); /* open file, check for errors*/
    }
    fdErrorCheck(fdOut = STDOUT_FILENO,
		 buffSize); /* open outfile, check for errors*/
  }
  /* Use input file or stdin if '-' and output file */
  else if(argc == 3){
    if(*argv[1] == '-'){ /* Use stdin*/
      buff = fdErrorCheck(fdIn = STDIN_FILENO,
			  buffSize); /* open stdin, check for errors*/
      fdErrorCheck(fdOut = open(argv[2], O_WRONLY, S_IRUSR | S_IWUSR),
		   buffSize); /* open outfile, check for errors*/
      stdinFlag = 1;
    }
    else{
      buff = fdErrorCheck(fdIn = STDIN_FILENO,
			  buffSize); /* open stdin*/
      fdErrorCheck(fdOut = open(argv[2], O_WRONLY, S_IRUSR | S_IWUSR),
		  buffSize); /* open outfile, check for errors*/
    }
  }
  else{
    perror("Usage: [filename | '-'] [filename]");
    exit(EXIT_FAILURE);
  }

  /* TODO redesign dynamic mem allocation for read*/
  /* Not working as intended. Read seems to keep going past size_t count*/
  /* Just make CHUNK size large for now*/
  /* Resize buff if still more to read*/
  while(*buffSize >= size){ /* Might need to consider one off issue*/
    /* increase buff memory */
    if(NULL == (buff = realloc(buff, size * sizeof(char)))){
      perror("Increasing buff realloc");
      exit(EXIT_FAILURE);
    }
    else{ /* sucess, strcat more stdin. Issue: Read seems to return everything*/
      strcat(buff, fdErrorCheck(fdIn = STDIN_FILENO,
			       buffSize)); /* open stdin. buffSize ignored*/
    }
  }

  if(*buffSize < size){/* Shrink*/
      if(NULL == (buff = realloc(buff, (*buffSize + 1) * sizeof(char)))){
	perror("Shrink buff realloc");
	exit(EXIT_FAILURE);
      }
    }

  /* Can probably free buffSize here*/

  /* For each ascii char make an empty node and link them up*/
  for(i = 0; i < ASCII; i++){
    hNew = createNode(i);
    hNew->next = hHead;
    hHead = hNew;
  }

  /* Increment through list. List is fixed size. Don't need to check node*/
  search = hHead;
  for(i = 0; i < *buffSize; i++){ /* Increment buffer*/
    for(j = 0; j < ((ASCII - 1) - buff[i]); j++){ /* increment specific c*/
      search = search->next;
    }
    search->num++;
    search = hHead; /* Point search at head and start over*/
  }

  
    
  /* write(STDOUT_FILENO, buff, *buffSize); For testing*/
  printList(hHead); /* For testing */

  hHead = shrinkList(hHead);

  printf("\n\n\n ---------------------------- \n\n\n");
  printList(hHead); /* For testing */

 
  close(fdIn);
  close(fdOut);
  
}


char *fdErrorCheck(int fd, int *count){
  char *ptr;
  int size = CHUNK;
  int readCount = 0;
  /* Allocate some memory for local pointer ptr*/
  if(NULL == (ptr = (char*)malloc(size * sizeof(char)))){
    perror("buffer malloc");
    exit(EXIT_FAILURE);
  }
  if(-1 == fd){
    perror("opening fd");
    exit(EXIT_FAILURE);
  }
  if(-1 == (readCount = read(fd, ptr, size))){/* Read input file, check errs*/
    perror("reading fd");
    exit(EXIT_FAILURE);
  }
  if(readCount > 0){ /* Increment the total size*/
    *count += readCount;
  }
  return ptr;
}

node *createNode(int c){ /* Build an empty table to be populated later*/
  node* new;
  if(NULL == (new = malloc(sizeof(node)))){
    perror("Node malloc");
    exit(EXIT_FAILURE);
  }
  new->next = NULL;  /* for list of hoffman values*/
  new->right = NULL; /* for creating the huffman tree*/
  new->left = NULL;  /* for creating the huffman tree*/
  new->c = c;        /* Save the ascii value*/
  new->num = 0;      /* Number of ascii values found*/
  new->h_code = 0;   /* huffman code*/
  return new;
}

/* For testing*/
void printList(node *head){
  node *curr = head;
  while(NULL != curr){
    printf("Node %c: . Count = %d.\n", curr->c, curr->num);
    curr = curr->next;
  }
}

node* searchNodes(node* head, int c){
  node* searchNode = head;
  while(NULL != searchNode){
    if(searchNode->c == c){
      return searchNode;
    }
    searchNode = searchNode->next;
  }

  return searchNode;
}

/* If num == 0 then drop node from list*/
node* shrinkList(node* head){
  node* newHead = head;
  node* currNode = head;
  node* temp;
  while(NULL != currNode->next){
    if(currNode->num == 0){ /* Unused node found*/
      if(newHead == currNode){ /* Removing the head of list*/
	newHead = currNode->next;
	temp = currNode->next;
	currNode->next = NULL;
	currNode = temp;
      }
      else{
	currNode = currNode->next;
      }
      /* else{ remove middle node*/
      /* 	if(temp == '/0' || currNode == NULL){ */
      /* 	  return newHead; */
      /* 	} */
      /* 	temp->next = NULL; /\* delete previous link to currNode */
      /* 	temp->next = currNode->next; / Link previous node to next, next */
      /* 	temp = currNode->next; / move temp to next valid node*/
      /* 	currNode->next = NULL; / remove next link from empty node */
      /* 	currNode = temp; /\* Move currNode to next valid node  */
      /* } */
    }
    else{
      temp = currNode; /* Hold the previous node*/
      currNode = currNode->next;
    }
  }
  return newHead;
}

