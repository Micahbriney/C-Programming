/* Program: Uniq.c
   Description: The program will take input from stdin and compare each
                line to the next subsequent line. If the two lines are 
                the same then one is discarded. This will continue until
                each successive line in stdout is unique compared to the
                previous line.
*/
#include <string.h>
#include "readlongline.h"

#define ARGCMAX 1

int main(int argc, char *argv[]){
  char *currline, *nextline;

  /* Error check argc*/
  if(argc != ARGCMAX){
    fprintf(stderr, "Usage: %s | [filename]", argv[0]);
    exit(EXIT_FAILURE);
  }
 
  /* Error check stdin*/  
  if(ferror(stdin)){
    fprintf(stderr, "Somthing is wrong. %s cannot not read from stdin.\n",
	    argv[0]);
    exit(EXIT_FAILURE);
  }
   
  /* Error check stdout*/
  if(ferror(stdout)){
    fprintf(stderr, "Somthing is wrong. %s cannot not read from stdin.\n",
	    argv[0]);
    exit(EXIT_FAILURE);
  }
  
  /* Get first line*/
  currline = readLongLine(stdin);
  /* Check if EOF was found*/
  while(currline != NULL){
    /* Get next line*/
    nextline = readLongLine(stdin);
    /* Check nextline for NULL*/
    if(nextline == NULL)
      break;
    /* Compare the two lines*/
    if(strcmp(currline, nextline)) /* No duplicate line found. Print line.*/
      puts(currline);
    free(currline); /* free up heap memory so we can malloc more later */
    currline = nextline; /* Point currline to nextline*/
  }

  if(currline != NULL)
    puts(currline);

  return 0;
}
