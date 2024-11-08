#include <stdio.h>
/* Note that EOF is a non char value
   a char value is 0-255
   the getchar must be stored in an int
   so that EOF (which is > 255)*/
/* Copy output to input*/
int main(int argc, char *argv[]){
  int c;

  /* while gets a char, assigned it to c, then tests for EOF*/
  /* Program will never end if using stdio
     IO redirect with a file works as file will have EOF value*/
  while((c = getchar()) != EOF){ /* While end of file has not been reached*/
    putchar(c); /* Output each char found*/
  }

  /* Exercise 1-6 and 1-7*/
  if(EOF == 0)
    printf("End of file = 0");
  else if(EOF == 1)
    printf("End of file = 1");
  else
    printf("EOF does not equal 1 or 0\n");

  printf("EOF value: %d\n", c);
  return 0;
}
