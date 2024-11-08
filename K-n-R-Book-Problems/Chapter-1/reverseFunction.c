/* Program: Reverse line
   Description: This program will reverse most lines of any length
                This program will probably not handle carriage returns
                well. It also assumes lines end with \n */
#include <stdio.h>

#define MAXLINE 1000

int getLine(char s[], int lim);
void reverse(char ret[], char s[], int lim);

int main(int argc, char *argv[]){
  char line[MAXLINE], revline[MAXLINE];
  int len;

  while((len = getLine(line, MAXLINE)) > 0){
    reverse(revline, line, len);
    printf("%s", revline);
    while(len + 1 == MAXLINE &&
       line[len - 1] != '\n' &&
       (len = getLine(line, MAXLINE)) > 0){
      reverse(revline, line, len);
      printf("%s",revline);
    }
  }

  return 0;
}

/* getLine: limited getline function returns len of line terminated \w \n*/
int getLine(char s[], int lim){
  int c, i;

  for(i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; i++)
    s[i] = c;
  if(c == '\n')
    s[i++] = c;
  s[i] = '\0';

  return i;
}

/* reverse: this function assumes lines have a \n and are null terminated*/
void reverse(char ret[], char s[], int lim){
  int i, downcount;

  /* 3 because: 0 = char, 1 = '\n' 2 = '\0' */
  if(lim < 3){ /* Cases: blank line, single char*/
    for(i = 0; i < lim; i++)
      ret[i] = s[i];
    ret[i] = '\0';
    return;
  }

  if(s[lim - 1] == '\n'){ /* Line length is 999 chars or less*/
    downcount = lim - 2;
    for(i = 0; i < lim - 1; i++)
      ret[i] = s[downcount--];
    ret[i++] = s[lim - 1];
    ret[i] = s[lim];
  }
  else{ /* Line length is 1000 chars or more. or Does not end in \n*/
    downcount = lim - 1;
    for(i = 0; i < lim - 1; i++)
      ret[i] = s[downcount--];
    ret[i++] = s[lim - 1];
    ret[i] = s[lim];
  }
  
  return;
}
