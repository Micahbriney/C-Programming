#include <stdio.h>

/* 1.5.2 Char Counting Version 2
   Version 1 used a while loop
   Version 2 replaces the wile loop with a for loop
   it uses a null statement ; since the for loop handles the increment*/

int main(int argc, char *argv[]){
  double numChars;

  for(numChars = 0; getchar() != EOF; ++numChars)
    ; /* DO NOTHING. Null statement.*/
  printf("%.0f\n", numChars);
      
  return 0;
}
