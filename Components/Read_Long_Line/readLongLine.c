/*
 *Author     : Micah Briney
 *Date       : 9-26-2022
 *Version    : 2.0
 *Source     : readLongLine.c
 *Description: readLongLine will accept a file pointer and make 

*/


#define CHUNK 100
#include "readLongLine.h"

void printError(const char *errStr);

char *readLongLine(FILE * fptr){
  char *line;
  int c, size, i;

  size = CHUNK;
  if(!(line = (char *)malloc(size * sizeof(char)))){
    printError("readLongLine: Malloc failed.");
  }

  i = 0;
  while(EOF != (c = getc(fptr))){
    if(i == size - 1)
      if(!(line = (char *)realloc(line, (size += CHUNK) * sizeof(char))))
	printError("readLongLine: Realloc expand failed.");
    if(c == '\n' || c == '\r' || c == '\f'){
      line[i++] = '\0';
      if(!(line = (char *)realloc(line, i * sizeof(char))))
	printError("readLongLine: Realloc shrink failed.");
      break;
    }
    else
      line[i++] = c;
  }
  

  if(i == 0){
    free(line);
    line = NULL;
  }
  
  return line;
}

/*
 *@fn      :printError
 *@details :Takes in a string to tell stderr 
 */
void printError(const char *errStr){
  perror(errStr);
  exit(EXIT_FAILURE);
}
