/* Program: removeComments
   Description: This program removes comments from c code. It looks for 
                open '/ * and close '* /' (no spaces) comment blocks.
                It currently fails to handle string literal comments. 
                IE. printf(" this is a comment block \* *\\");
                will become printf(" this is a comment block");
                also make note that when using inqoute case there are
                some odd cases such as 
		char p[] =
		"0/!10\"@40\'@50.@01/@11*!2"
Here is the prob"1\"/41\'/5"            / * added by RAL * /
	    "1./02*!32.!23/ 03*!33.!24\"@04\\@64.@45\'@05\\@75.@56.@47.@5";

*/


#include <stdio.h>

#define MAXLINE 1000

int getLine(char s[], int lim);
void copy(char to[], char from[]);
int removeComments(char s[], int lim);

int multilineFlag = 0;

int main(int argc, char *argv[]){
  char line[MAXLINE];
  int i, len;
  printf("\* *\\");
  while((len = getLine(line, MAXLINE)) > 0){
    len = removeComments(line, len);
      
    for(i = 0; i < len; i++)
      putchar(line[i]);
  }
  
  return 0;
}

void copy(char to[], char from[]){
  int i;

  for(i = 0; to[i] != '\0'; i++)
    to[i] = from[i];

  return;
}

int getLine(char s[], int lim){
  int c, i;

  for(i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; i++)
    s[i] = c;
  if(c == '\n')
    s[i++] = '\n';
  s[i] = '\0';

  return i;
}

int removeComments(char s[], int lim){
  int i, j, inquote;
  char temp[MAXLINE];

  i = j = inquote = 0;
  
  for( ; i < lim; i++){
    if(j != 0)
      break;
    if(s[i] = '"';
       inquote = 1; /* More needs to be added for inquotes cases*/
    if((s[i] == '/' && s[i + 1] == '*') || multilineFlag){ /* open found*/
      for(j = i ; j < lim; j++){
	/* If end of line. Then comments continues on next line */
	if((s[j] == '\n' && s[j + 1] == '\0')){ /* End of line */
	  temp[i++] = '\n';
	  temp[i] = '\0';
	  multilineFlag = 1; /* Multi line comments found*/
	  break;
	}
	else if(s[j] == '*' && /* end of comment found*/
		s[j + 1] == '/' &&
		s[j + 2] == '\n' &&
		s[j + 3] == '\0'){
	  temp[i++] = '\n';
	  temp[i] = '\0';
	  multilineFlag = 0;
	  break;
	}
	else if(s[j] == '\0'){ /* Line is over MAXLENGTH w/ no end comment*/
	  s[i] = '\0';
	  multilineFlag = 1;
	  break;
	}/* Comments inbetween code. Odd case*/
        else if(s[j] == '*' && s[j + 1] == '/' && s[j + 2] != '\0'){
	  multilineFlag = 0;
	  for( ; j < lim; j++) /* Continue saving code*/
	    temp[i++] = s[j];
	  break;
	}
      }
    }
    else /* No open comment found*/
      temp[i] = s[i];
  }

  copy(s, temp); /* Call copy to save temp back into line in main*/

  return i;
}
