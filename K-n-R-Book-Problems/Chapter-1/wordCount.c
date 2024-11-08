/* Bare bones unix wc program. Words are any char not 
   a tab, newline or space
   1.5.4 Word counting example*/
#include <stdio.h>

#define IN 1 /* Inside a word*/
#define OUT 0 /* Outside a word*/

/* Count lines, words and characters in input */
int main(int argc, char *argv[]){
  int c, nl, nw, nc, state;

  state = OUT; /* Start outside word. Dont know what first char is*/
  nl = nw = nc = 0; /* initilize counters*/

  while(EOF != (c = getchar())){
    ++nc;
    if(c == '\n') /* Found newline*/
      ++nl;       /* Increment new line counter*/
    if(c == ' ' || c == '\n' || c == '\t') /* Outside of a word yet?*/
      state = OUT; /* word has ended*/
    else if(state == OUT){ /* Entering a word */
      state = IN;
      ++nw;
    }

  }

  printf("Number of lines: %d\n"
	 "Number of chars: %d\n"
	 "Number of words: %d\n", nl, nc, nw);
}
