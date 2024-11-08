/* Source file: readLongWord
   Description: readlongWord will read user input and store it. 
                The length of user input will not be assumed. The function
                allows for arbitrarily long words to be processed. The 
                function parameters are a FILE pointer that will point
                to the stored user input. Funtion will return a 
                char pointer.
   Parameter(s): File pointer.
   Notes: Used by the uniq program */
#include "readLongWord.h"
#include <ctype.h>
#define CHUNK 100

char *readLongWord(FILE *fpt){
  char *buffer;
  int size, c, i = 0;

  size = CHUNK;
  /* Allocate memory for user input, plus error checking*/
  buffer = (char *)malloc(size * sizeof(char));
  if(!buffer){ /* Null check malloc*/
    perror("Malloc buffer failed in readLongWord");
    exit(EXIT_FAILURE);
  }

  /* Check file input chars.
     When EOF reached return buffer.
     If Chunk limit reached before EOF then increase buffer size
     Don't keep any non alpha chars. Error check realloc*/
  /* Loop until non alpha char found*/
  while(isalpha(c = fgetc(fpt))){
    if(i >= size - 2){ /* Reached end of CHUNKed space. Get more.*/
      size += CHUNK;   /* Increase CHUNK size and realloc*/
      buffer = (char *)realloc(buffer, size * sizeof(char));
      if(!buffer){ /* Null check realloc*/
	perror("Realloc buffer failed in readLongWord");
	exit(EXIT_FAILURE);
      }
    }
    if(isupper(c)) /* Make words lower case*/
      buffer[i++] = tolower(c);
    else
      buffer[i++] = c;
  }

  /* Null terminate buffer*/
  if(i > 0)
    buffer[i] = '\0';
  else if(i == 0 && c == EOF)/* fpt is empty and at EOF*/
    return buffer = NULL;
  else if(i == 0)/* fpt is not empty but first char was a non alpha*/
    buffer = readLongWord(fpt);
  else{ /* Unconsidered edge case*/
    fprintf(stderr, "Edge case found");
    exit(EXIT_FAILURE);
  }
  
  /* Shrink buffer before returning*/
  if(buffer){ /* Shrink only if buffer is not NULL*/
    buffer = (char *)realloc(buffer, (i + 1) * sizeof(char));
    if(!buffer){/* Null check shrinking realloc*/
      perror("Shrinking Realloc buffer failed in readlongline");
      exit(EXIT_FAILURE);
    }
  }

  return buffer;
}
