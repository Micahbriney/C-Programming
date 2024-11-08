/* PROGRAM: getLineFunction 
   description: getLineFunction works by getting a long line of 
                characters from the stdin. The function getLine is
                called with parameters line[] and MAXLINE. The 
                function will step through using getchar() function
                until a '\n' char, EOF is found or MAXLINE - 1 is 
                reached. It will insert a null terminator char at
                the end of s[]. The function returned the len of 
                the long line. The main keeps track of the longest line.
   Revision: exercise 1-16: Handle lines longer than MAXLINE.
                            This was done by printing each line that
                            exceeds the MAXLINE length along with the 
                            length of that line. A overmax
                            line length was tracked to let the user
                            find the longest of the lines that exceeded
                            MAXLINE.*/
#include <stdio.h>

#define MAXLINE 1000 /* maximum input line size + null terminator*/

int getLine(char s[], int lim);
void copy(char to[], char from[]);

/* print longest input line*/
int main(int argc, char *argv[]){
  int len;               /* current line length*/
  int max, overmax;      /* maximum lenght seen so far*/
  char line[MAXLINE];    /* current input line*/
  char longest[MAXLINE]; /* longest line saved here*/
  char temp[MAXLINE];    /* holds line[] when Max line length reached*/

  max = overmax = 0;     /* init max length to zero*/
  while((len = getLine(line, MAXLINE)) > 0){
    if(len > max){       /* if true then new largest line found*/
      max = len;
      copy(longest, line); /* copy line to longest arr*/
    }
    
    copy(temp, line); /* Store line incase it is overwritten*/
    if((len + 1) == MAXLINE &&
       line[len - 1] != '\n' && /* Case for MAX chars*/
       (len = getLine(line, MAXLINE)) > 0){ 
      printf("Line exceeded Maximum line length %d\n", MAXLINE - 1);
      printf("%s%s", temp, line);
      max += len;
      if((len + 1) == MAXLINE){ /* Lines longer than MAXLINE*/
	/* Keep reading the line untill it is smaller than MAXLINE */
	while((len = getLine(line, MAXLINE)) > 0){
	  max += len; /* increment max length*/
	  printf("%s", line);
	}
      }
      /* end of overlimit line reached*/
      printf("Length: %d\n\n", max); /* Print max for over limit lines*/
      if(max > overmax)  /* Compare over limit maxes*/
	overmax = max;   /* Store largest over limit max*/
      max = MAXLINE - 1; /* Lower max back to max limit*/
    }
    
    if(len == 0) /* Case where line is exactly 1000 chars*/
      continue;
  }
  
  if(max > 0 && max > overmax){ /* Line exists and limit not exceeded*/
    printf("%s", longest);
    printf("Length: %d\n", max);
  }
  else if(overmax > max) /* MAXLINE limit exceeded at least once*/
    printf("The line(s) with length %d is/are the longest.\n", overmax);

  return 0;
}

/* getline: read a line into s, return length*/
int getLine(char s[], int lim){
  int c, i;

  /* Any char that isn't a newline store char in array*/
  for(i = 0; i < lim - 1 && (c = getchar()) != EOF && c!= '\n'; ++i)
    s[i] = c;
  if(c == '\n'){ /* store newline char when found*/
    s[i] = c;
    ++i;
  }
  s[i] = '\0'; /* Null terminate array*/
  return i;
}

/* copy: copy 'from' into 'to'; assume to is big enough*/
void copy(char to[], char from[]){
  int i;

  i = 0;
  while((to[i] = from[i]) != '\0')
    ++i;
}
