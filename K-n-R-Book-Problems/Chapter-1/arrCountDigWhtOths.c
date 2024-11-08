/* Book example 1.6 Arrays pg 22
   The program will count the following:
   digits, 
   white space chars (blank, tab, newline)
   and other characters*/

#include <stdio.h>

int main(int argc, char *argv[]){
  int c, i, numWhite, numOther; /* Whitespace (\b, \t, space */
  int numDigits[10]; /* store the number count for 0-9*/

  numWhite = numOther = 0;
  for(i = 0; i < 10; ++i) /* Inilize array num counter w/ zeros*/
    numDigits[i] = 0;

  while(EOF != (c = getchar())){
    if(c >= '0' && c <= '9') /* If num found, increment array element*/
      ++numDigits[c - '0']; /* Diff between char literal and '0' */
    else if(c == '\t' || c == '\n' || c == ' ') /* Found Whitespace char*/
      ++numWhite;
    else            /* Not num nor whitespace char was found*/
      ++numOther;
  }

  printf("Digits   : Found Count\n");
  for(i = 0; i < 10; i++)
    printf("Number %d: %d\n", i, numDigits[i]);

  printf("Number of white space chars found: %d\n", numWhite);
  printf("Number of other chars found: %d\n", numOther);
}
