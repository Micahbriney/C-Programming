#include <errno.h>
#include <limits.h>
#include "avlBinSearchTree.h"
#include "readLongWord.h"
#include "wordList.h"

#define OPTS "n:" /* n option with associated value ie -n <num>*/
#define BASE 10
#define MIN 10

extern char *optarg;
extern int optind;

int printcount = 0; /* Count number of times printed to screen*/

int getopt(int argc, char *const argv[], const char *optstring);

void countCompare(Treeptr root, int numwords, int maxcount);

int main(int argc, char *argv[]){
  int opt;
  long numwords = MIN;
  char *endptr = NULL;
  char *currword;
  Treeptr root;
  /* Treeptr roothead;*/
  FILE *fptr;
  char *filename;
  /* WordListPtr wlist, listhead, temphead; */


  /* Find options*/
  while((opt = getopt(argc, argv, OPTS)) != -1){
    switch(opt){
    case 'n':
      errno = 0;
      numwords = strtol(optarg, &endptr, BASE);
      if(numwords < 0){ /* No negative numbers*/
	fprintf(stderr,
		"Cannot count %ld number of words.\n",
		numwords);
	exit(EXIT_FAILURE);
      }
      if(endptr && strcmp(endptr, optarg) == 0){/* -n usage error check*/
	fprintf(stderr, "\"-n\" switch found without a num.\n Usage: %s"
		"[-n num] [file1 [file2 ...]]\n", argv[0]);
	exit(EXIT_FAILURE);
      }
      break;
    default: /* Unknow opt returns '?'*/
      fprintf(stderr, "Usage: %s [-n num] [file1 [file2 ...]]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  /* Check validity of opt argument*/
  if((errno == ERANGE &&  /* ERANGE set by strtol for over/underflow */
      (numwords == LONG_MAX || numwords == LONG_MIN))
     || (errno != 0 && optarg == 0)){ /* Some other error. ie. EINVAL*/
    perror("strtol");
    exit(EXIT_FAILURE);
  }

  /* Error check stdout*/
  if(ferror(stdout)){
    fprintf(stderr,"Somthing went wrong in %s with stdout.\n",
	   argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Check if stdin or FILE is provided*/
  if(optind < argc){/* If argc is greater than optind then files are used*/
    while(optind < argc){
      filename = argv[optind++];
      fptr = fopen(filename, "r");
      if(!fptr){ /* File failed to open. Returns NULL*/
	printf("\n** Something went wrong with file \"%s\".\n", filename);
	perror("Problem with fopen"); /* Print error then keep going*/
	printf("\n");
	continue;
      }
      while(NULL != (currword = readLongWord(fptr))){
	root = insertNode(root, currword);
	free(currword);
      }   
    }
  }
  else if(optind == argc){ /* stdio used*/
    /* Error check stdin*/
    if(ferror(stdin) || feof(stdin)){
      fprintf(stderr, "Somthing went wrong in %s with stdin.\n",
	     argv[0]);
      exit(EXIT_FAILURE);
    }
    root = avlBstInit();
    if(NULL == (currword = readLongWord(stdin))){
      printf("No input was found from stdin.\n");
      return 0;
    }
    root = insertNode(root, currword);
    free(currword);
    while(NULL != (currword = readLongWord(stdin))){
      root = insertNode(root, currword);
      free(currword);
    }
  }


  /* Not working*/
  /* roothead = findSmallestRoot(root); */

  /* if(roothead) */
  /*   listhead = createListNode(roothead->word, roothead->wordcount); */

  /* temphead = listhead; */

  printf("The top %ld words (out of %d) are:\n", numwords, getUniqWords());
  while(printcount < numwords){
    createInorderList(root, numwords, countCompare);
    decrementLargestWordCount();
  }

  /*wlist = createInorderList(listhead, root, insertListNode);*/
  
  return 0;
}


void countCompare(Treeptr root, int numwords, int maxcount){
  if(maxcount == 0)
    return;
  if(maxcount == root->wordcount && printcount < numwords){
    printf("%7d %s\n", root->wordcount, root->word);
    printcount++; /* Increment the print counter*/
  }
  return; 
}
