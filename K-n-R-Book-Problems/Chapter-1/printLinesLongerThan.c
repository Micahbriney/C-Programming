/* Exercise 1-17: Write a program to print all lines longer 
                  than 80 chars*/
#include <stdio.h>

#define MAXLINE 1000
#define MAXLENGTH 80

int getLine(char s[], int lim);

int main(int argc, char *argv[]){
  char line[MAXLINE];    /* Store the lines*/
  int len, linenum = 0;  /* Length and line number of line[]*/

  while((len = getLine(line, MAXLINE)) > 0){
    linenum++;
    if(len > MAXLENGTH)
      printf("\nThe following line exceeds 80 chars:\nLine %d: %s\n",
	     linenum, line);
    if(len == MAXLINE - 1 && line[len - 1] != '\n'){
      while((len = getLine(line, MAXLINE)) > 0){
	printf("%s", line);
	if(line[len - 1] == '\n')
	  break;
      }
    }
  }

  return 0;
}

int getLine(char s[], int lim){
  int i, c;
  for(i = 0; i < lim - 1 && (c = getchar()) != '\n' && c != EOF ; i++)
    s[i] = c;
  if(c == '\n'){
    s[i] = c;
    ++i;
  }

  s[i] = '\0';
  return i;
}
