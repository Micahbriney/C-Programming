/* Program: getint
   Description: getint function will get pull integers from a string
                and store them in an array. The function will store any
                numbers it finds in an array. When a non numeric char is
                found the char is pushed back onto the buffer (stdin).
                This includes newlines. So any numbers after a non digit
                is ignored. */
#define BUFSIZE 100
#include <ctype.h>
#include <stdio.h>

/* The global vars are to simulate terminal inputs*/
char buf[BUFSIZE]; /* buffer for ungetch*/
int bufp = 0;      /* Buffer position*/

int getint(int *pn);
int getch(void);  /* Book version of getchar*/
void ungetch(int c);   /* Book version of ungetc*/

int main(int argc, char * argv[]){
  int n, i, array[BUFSIZE];

  for(i = 0; i < BUFSIZE; array[i++] = 0) /* zero out array*/
    ;
  /* scan through input the size of BUFSIZE or smaller looking
     for ints. Each loop calls getint funct which will store
     a positive or negative char as an int in the array.
     When there are no more ints or a char is found then
     it will return 0. Otherwise it will return EOF when EOF is
     reached. */

  /*-------- For testing---------
  for(n = 0; n < BUFSIZE; n++)
    if(getint(&array[n]) == EOF)
      break;
  }
  -------- end testing--------*/
    
  for(n = 0; n < BUFSIZE && getint(&array[n]) != EOF; n++)
    ; /* logic is in the forloop*/

  printf("Integers Found.\n");
  for(i = 0; i < n - 1; i++)
    printf("%d\n",array[i]);
  
  return 0;

}

int getint(int *pn){
  int c, sign;

  while(isspace(c = getch())) /* Ignore white space*/
    ;
  if(!isdigit(c) && c != EOF && c != '+' && c != '-'){
    ungetch(c); /* Not a number*/
    return 0;
  }
  sign = (c == '-') ? -1 : 1;
  if(c == '+' || c == '-')
    if(!isdigit(c = getch())){
      ungetch(c);
      ungetch(sign);
      return 0;
    }
  for(*pn = 0; isdigit(c); c = getchar())
    *pn = 10 * *pn + (c - '0');
  *pn *= sign;
  if(c != EOF)
    ungetch(c);
  return c;
}


int getch(void){ /* get a (possibly pushed back) character*/
  return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c){
  if(bufp >= BUFSIZE)
    printf("ungetch: too many characters.");
  else
    buf[bufp++] = c;
}
