/* Program: foldLongLines
   Description: "Folds" long lines into two or more shorter lines.
                This occurs after the last non-blank character that
                occurs before the n-th column of input.
*/

#include <stdio.h>

#define MAXLINE 1000
#define NTHCOL 10

int getLine(char s[], int lim);
int foldline(char s[], int start, int lim);

int main(int argc, char *argv[]){
  char line[MAXLINE];
  int i, foldlen, len, start, end;

  end = NTHCOL - 1;
  start = 0;
  
  while((len = getLine(line, MAXLINE)) > 0){
    if(len == MAXLINE - 1 && line[len - 1] != '\n'){ /* Line over MAXLINE*/
      foldlen = foldline(line, end, len); /* Get length of allowable fold*/
      ; /* Do something with long line*/
    }
    while(start != len){
      foldlen = foldline(line, end, len); /* Get length of allowable fold*/
      /* Print chars*/
      for(i = start; i < foldlen; i++)
	putchar(line[i]);
      putchar('\n');
      start = foldlen;
      if((end = foldlen + NTHCOL - 1) > len)
	end = len;
      else
	end = foldlen + NTHCOL - 1;
    }
  }
    
  return 0;
}

int getLine(char s[], int lim){
  int i, c;
  
  for(i = 0;(c = getchar()) != EOF && c != '\n' && i < lim - 1; i++)
    s[i] = c;
  if(c == '\n')
    s[i++] = c;
  s[i] ='\0';

  return i;
}

void copy(char to[], char from[], int len){
  int i;

  for(i = 0; i < len; i++)
    to[i] = from[i];

  return;
}

int foldline(char line[], int end, int lim){
  int i;

  /* Find the first space at or after the nth column*/
  for(i = end; i < lim && line[i] != ' '; i++)
    ; /* Do nothing*/

  /* Find the first non-blank char after the nth column*/
  for(; i < lim && line[i] == ' '; i++)
    ; /* Do nothing*/
  
  return i; /* Length of fold line*/
}
