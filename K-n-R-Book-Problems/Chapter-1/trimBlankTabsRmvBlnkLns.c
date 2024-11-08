#include <stdio.h>

#define MAXLINE 1000

int getLine(char s[], int lim);

int main(int argc, char *argv[]){
  char line[MAXLINE];
  int len;

  while((len = getLine(line, MAXLINE)) > 0){
    /* Case MAXLINE exceeded*/
    if(len - 1 == MAXLINE && line[len - 1] != '\n')
      printf("Line to long. If you need this to work then\n"
	     "major changes to main or getLine will need to be made.\n"
             "Hard part would be removing tabs blanks that overlap \n"
	     "line[] and nextline[]");
    if(len > 2){ /* If lines are not blank trim trailing blanks and tabs*/
      while(line[len - 2] == ' ' || line[len - 2] == '\t'){
	line[len - 2] = line[len -1]; /* move newline up*/
	line[len - 1] = line[len];    /* move null terminator up*/
	--len; /* decrement index*/
      }
    }
    /* While a blank or tab exists at the end (before newline and null)*/
    if(line[0] == '\n') /* Blank line found*/
      continue; /* Continue loop without printing line*/

    printf("%s", line);
  }

  return 0;
}

int getLine(char s[], int lim){
  int c, i;

  for(i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; i++)
    s[i] = c;
  if(c == '\n')
    s[i++] = c;
  s[i] = '\0';
  
  return i;
  
}
