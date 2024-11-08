#include <stdio.h>

int main(int argc, char *argv[]){
  int numLines,numBlanks, numTabs, c;

  numLines = 0;
  numBlanks = 0;
  numTabs = 0;
  while(EOF != (c = getchar())){
    if(c == '\n') /* new line found*/
      numLines++;
    else if(c == ' ') /* blank found*/
      numBlanks++;
    else if(c == '\t') /* tab found*/
      numTabs++;
  }
  printf("Number of lines: %d\n", numLines);
  printf("Number of blanks: %d\n", numBlanks);
  printf("Number of tabs: %d\n", numTabs);

}
