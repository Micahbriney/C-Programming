/* 1-9 Exercise replaces multiple blanks(spaces) with single spaces
   1-10 Exercise replaces tabs with \t backspace with \b 
        and backslash with \\ */
/* Learning note. I first used Else if to find tab, backspace, backslash
   resulted in problems with a backslash following a space.*/
#include <stdio.h>

int main (int argc, char *argv[]){
  int c;

  while(EOF != (c = getchar())){
    if(c == ' '){
      putchar(c);
      while(EOF != (c = getchar()) && c == ' ')
	; /* DO NOTHING. Null statement. This will skip the spaces*/
    }
    if(c == '\t'){ /* Tab found*/
      putchar('\\');
      putchar('t');
      continue;
    }
    if(c == '\b'){ /* backspace found*/
      putchar('\\');
      putchar('b');
      continue;
    }
    if(c == '\\'){ /* backslash found*/
      putchar('\\');
      putchar('\\');
      continue;
    }
    if(c == ' '){ /* Space found*/
      putchar('\\');
      putchar('s');
    }
      
    putchar(c);
  }

  return 0;
}
