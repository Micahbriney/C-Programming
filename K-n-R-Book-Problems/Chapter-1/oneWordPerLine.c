/* Exercise 1-12 program prints one word per line*/
#include <stdio.h>

#define IN 1
#define OUT 0

int main(int argc, char *argv[]){
  int c, state;

  state = OUT;

  while(EOF != (c = getchar())){
    if((c >= 'A' && c <= 'Z') ||
       (c >= 'a' && c <= 'z')){ /*Is an alpha char*/
      state = IN;   /* In a word */
      putchar(c);
    }
    else if(state == IN){
      putchar('\n'); /* End of word. print a newline*/
      state = OUT; /* Set state to inside of word*/
    }
  }
  return 0;
}
