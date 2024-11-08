/* Exercies 1-14  Print out a histogram of frequencies
   of different characters in its inputs*/

#include <stdio.h>

#define NUMCHARS 256

void horizontalHistogram(int hist[], int size);

int main(int argc, char *argv[]){
  int c, i;
  int hist[NUMCHARS];

  for(i = 0; i < NUMCHARS; i++) /* Zero out hist array*/
    hist[i] = 0;
  
  while(EOF != (c = getchar()))
    ++hist[c];

  horizontalHistogram(hist, NUMCHARS);

  return 0;
}

void horizontalHistogram(int hist[], int size){
  int i, j;
  printf("\nHorizontal Histogram. Character count graph\n\n");
  for(i = 0; i < size; i++){
    if(hist[i] > 0){
      if(i == '\t')
	printf("\\t :");
      else if(i == '\n')
	printf("\\n :");
      else if(i == ' ')
	printf("' ':");
      else
	printf("%c  :", i);
      for(j = 0; j < hist[i]; j++)
	printf("#");
      printf("\n");
    }
  }
}
