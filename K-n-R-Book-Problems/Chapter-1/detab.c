/* Program: detab
   Description: Replaces strings of blanks with the minimum number
                of tabs and blanks to acheive the same spacing.
*/
#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 1000
#define TABSTOP 8

int getLine(char s[], int lim);
int entab(); /* Maybe use*/

int main(int argc, char *argv[]){
  int pos, i, c;

  /* Error checking */
  if(ferror(stdin)){
    printf("Somthing went wrong, %s cannot write to stdin.\n", argv[0]);
    perror("Reason");
    exit(1);
  }
  if(ferror(stdout)){
    printf("Somthing went wrong, %s cannot write to stdout.\n", argv[0]);
    perror("Reason");
    exit(1);
  }
  
  while((c = getchar()) != EOF){
    if(c == '\n' || c == '\r'){/* New line char found. reset position*/
      putc(c, stdout);
      pos = 0;
      continue;
    }
    if(c == '\b' && pos == 0){ /* Don't allow pos to go below 0*/
      putc(c, stdout);
      continue;
    }
    if(c == '\b'){ /* Decrement position due to position*/
      putc(c, stdout);
      pos--;
      continue;
    }
    if(c == '\t')
      for(i = pos % TABSTOP; i < TABSTOP; i++){
	putc(' ', stdout); /* Insert ' ' untill end of tabstop*/
	pos++;
      }
    else{
      putc(c, stdout);
      pos++;
    }
  }
  return 0;
}
