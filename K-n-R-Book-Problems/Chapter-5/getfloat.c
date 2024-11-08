/* Unfinished*/

#include <stdio.h>
#include <ctype.h>

#define BUFFSIZE 100

int getch(int *pn);
void ungetch(int c);
int getfloat(char *pn);

int main(int argc, char *argv[]){
  int i, n;
  float arr[BUFFSIZE];

  for(i = 0; i < BUFFSIZE; i++)
    arr[i] = 0.0;

  for(n = 0; i < BUFFSIZE && getfloat(arr[n]); n++)
    ; /* Do nothing. Logic is in for loop*/
  
}

int getch(int pn*){
  return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c){
  if(bufp >= BUFFSIZE)
    printf("ungetch: too many characters.");
  else
    buf[bufp++] = c;
}

int getfloat(char *pn){
  int c, sign;

  
}

