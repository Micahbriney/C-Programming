/* Exercise 1-13 Histogram of lengths of words
   Trying for both vertical and horizontal var histogram*/
#include <stdio.h>

#define EMPTY 0
#define NOTEMPTY 1
#define IN 1
#define OUT 0
#define MAXWORDLEN 45

void downVerticalGraph(int hist[], int size);
void upVerticalGraph(int hist[], int size);
void horizontalGraph(int hist[], int size);

int main(int argc, char *argv[]){
  int c, lenCount, state;
  int size = MAXWORDLEN + 1;
  int hist[size]; /* Words up to max word length*/

  /* Initilize state and lenght counter*/
  state = OUT;
  lenCount = 0;
  
  for(int i = 0; i < size; i++) /* Initilize arr to zero*/
    hist[i] = 0;
  while(EOF != (c= getchar())){
    if(c == '\t' || c == '\n' || c == ' '){ /* Found whitespace char*/
      state = OUT;      /* We are now outside a word*/
      ++hist[lenCount]; /* Increment histogram counter*/
    }
    else if(state == OUT){ /* No whitespace char found. And state is OUT*/
      state = IN; /* Word found*/
      lenCount = 1;
    }
    else if(state == IN) /* If still inside word increment lenght counter*/
      lenCount++;
  }

  horizontalGraph(hist, size);
  downVerticalGraph(hist, size);
  upVerticalGraph(hist, size);

}


/* The down vertical graph histogram works by scanning through 
   the array. If a word count is found then a tick mark is made. 
   Also the empty flag is reset to not empty*/
void downVerticalGraph(int hist[], int size){
  int printHist[size];
  int empty, i, numWords;  /* array empty flag*/

  for(i = 0; i < size; i++)
    printHist[i] = hist[i];
  
  printf("\nWord Length Histogram\n");

  /* x-axis label spacing*/
  printf("     ");
  /* Print number labels*/
  for(i = 1; i < size; i++)
    printf("%2d ", i);

  
  printf("\n");
  /* x-axis padding for y-axis labels*/
  printf("-----");
  /* Print line to separate labels from tick marks*/
  for(i = 1; i < size; i++)
    printf("---");

  /* Init while loop*/
  printf("\n");
  empty = NOTEMPTY; /* Set empty flag to enter loop*/
  numWords = 1;
  
  while(empty){
    printf("%3d :", numWords++); /* print y-axis labels*/
    empty = EMPTY; /* clear array flag*/
    for(i = 1; i < size; i++){
      if(printHist[i] > 0){ /* Some count remains*/
	--printHist[i]; /* Decrement the word count*/
	printf(" # ");
	empty = NOTEMPTY;
      }
      else if(empty == EMPTY && i = size - 1) /* Nothing left to graph*/
	break;
      else
	printf("   ");
    }
    printf("\n");
  }
}


void upVerticalGraph(int hist[], int size){
  int empty, i;  /* array empty flag*/
  int longestCurrWord = 0;
  int printHist[size];

  for(i = 0; i < size; i++)
    printHist[i] = hist[i];
  
  printf("\nWord Length Histogram\n");

  /* Find longest current word*/
  for(i = 1; i < size; i++){
    if(longestCurrWord < hist[i])
      longestCurrWord = hist[i];
  }

  printf("\n");
  empty = NOTEMPTY; /* Set empty flag to enter loop*/
  while(empty){
    if(longestCurrWord < 1){ /* All elements have been printed*/
      empty = EMPTY; /* clear array flag*/
      continue;
    }
    printf("%3d :", longestCurrWord);

    for(i = 1; i < size; i++){
      if(printHist[i] == longestCurrWord){ /* Some count remains*/
	--printHist[i]; /* Decrement the word count*/
	printf(" # ");
	empty = NOTEMPTY;
      }
      else
	printf("   ");
    }
    printf("\n");
    --longestCurrWord; /* Decrement the current longest word*/
  }

  /* Align x-axis*/
  printf("-----");
  
  for(i = 1; i < size; i++)
    printf("---");
  printf("\n");
  
  /* Align labels under x-axis*/
  printf("     ");
  
  /* Print number labels*/
  for(i = 1; i < size; i++)
    printf("%2d ", i);

  printf("\n");
}


void horizontalGraph(int hist[], int size){
  printf("\nWord Length Histogram\n");

  for(int i = 1; i < size; i++){ /* There are no words of size 0 so i = 1*/
    printf("%2d|", i);
    for(int j = 0; j < hist[i]; j++)
      printf("#");
    printf("\n");
  }
}
