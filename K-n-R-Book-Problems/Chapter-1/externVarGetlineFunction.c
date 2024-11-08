/* Program: externVarGetlineFunction
   Description: This is a revision of the getlineFunction.c program.
                Changes: variables that are used by more than one
                         function are made into global variables.
                         Function parameters have been removed as
                         the necessary variables are now global.*/
#include <stdio.h>

#define MAXLINE 1000   /* maximum input line size*/

int max;               /* maximum length seen so far*/
char line[MAXLINE];    /* current input line*/
char longest[MAXLINE]; /* longest line saved here*/

int getLine(void);
void copy(void);

int main(int argc, char *argv[]){
  int len;
  extern int max;
  extern char longest[];

  max = 0;
  while((len = getLine()) > 0)
    if(len > max){
      max = len;
      copy();
    }
  if(max > 0)
    printf("%s", longest);
  return 0;
}

/* getLine: global variable version*/
int getLine(void){
  extern char line[];
  int i, c;

  for(i = 0; i < MAXLINE - 1 &&	(c = getchar()) != EOF && c != '\n'; i++)
    line[i] = c;
  if(c == '\n')
    line[i++] = c;
  line[i] = '\0';
  return i;
}

/* copy: global variable version*/
void copy(void){
  int i;
  extern char line[], longest[];

  i = 0;
  while((longest[i] = line[i]) != '\0')
    ++i;
}
