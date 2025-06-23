/* Source file: readLongLine
   Description: readlongline will read user input and store it. 
                The length of user input will not be assumed. The function
                allows for arbitrarily long lines to be processed. The 
                function parameters are a char pointer that will point
                to the stored user input. Funtion will return a 
                char pointer.
   Parameter(s): File pointer.
   Notes: Used by the uniq program */
#include "readlongline.h"
#define CHUNK 1024

char *readLongLine(FILE *fpt){
  char *buffer;
  int size, c, i = 0;

  size = CHUNK;
  /* Allocate memory for user input, plus error checking*/
  buffer = (char *)malloc(size * sizeof(char));
  if(!buffer){ /* Null check malloc*/
    perror("Malloc buffer failed in readlongline");
    exit(EXIT_FAILURE);
  }

  /* Check file input chars.
     When EOF reached return buffer.
     If Chunk limit reached before EOF then increase buffer size
     Don't keep newline, carrage return or formfeeds chars
     Error check if realloc fails */

  /* Loop until EOF, newline, carrage return or formfeeds is found*/
  while((c = fgetc(fpt)) != EOF && c != '\n' && c != '\r' && c != '\f' ){
    if(i >= size - 1){ /* Reached end of CHUNKed space. Get more.*/
      size += CHUNK;   /* Increase CHUNK size and realloc*/
      buffer = (char *)realloc(buffer, size * sizeof(char));
      if(!buffer){ /* Null check realloc*/
	      perror("Realloc buffer failed in readlongline");
	      exit(EXIT_FAILURE);
      }
    }
    buffer[i++] = c;
  }

  /* Null terminate buffer*/
  if(c == '\n' || c == '\r' || c == '\f' || i > 0)
    buffer[i] = '\0';
  else if(i == 0)  /* Null Case*/
    return buffer = NULL;
  else{ /* Unconsidered edge case*/
    fprintf(stderr, "Edge case found");
    exit(EXIT_FAILURE);
  }
  
  /* Shrink buffer before returning*/
  buffer = (char *)realloc(buffer, (i + 1) * sizeof(char));
  if(!buffer){/* Null check shrinking realloc*/
    perror("Shrinking Realloc buffer failed in readlongline");
    exit(EXIT_FAILURE);
  }
  
  return buffer;
}
