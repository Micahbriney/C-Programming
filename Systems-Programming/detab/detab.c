/*
Author: Micah Briney
Create Date: 4/1/2022

Program: detab will read in a file and find the tabs and
         expand them into spaces. It will consider a tabstop
	 of 8 spaces. So at most a tab will expand to 8 spaces
	 at least a tab will expand to 1 space. Using Diff on
	 the input and output files should show where all of the
	 tabs were changed to spaces. The lines shown by diff
	 should look identical.

*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define TABSTOP 8

int main(int argc, char *argv[]){

  int c;
  int pos; /* 0 = left margin */
  int i;
  /* check if file can be opened by detab */
  if(ferror(stdin)) {
    printf("Somthing went wrong. %s cannot not open stdin.\n",argv[0]);
    perror("Reason");
    exit(1);
  }

  if(ferror(stdout)){
    printf("Somthing went wrong. %s cannot not write to stdout.\n",argv[0]);
    perror("Reason");
    exit(1);
  }

  pos = 0;
  while((c = getchar()) != EOF){
    if(c == '\n' || c == '\r'){  /* Put new line and reset position */
      putc(c, stdout);
      pos = 0;
      continue;
    }
    if(c == '\b' && pos == 0){  /* Don't allow pos to go below 0*/
      putc(c, stdout);
      continue;
    }
    if(c == '\b'){  /* Put backspace and decrement position*/
      putc(c, stdout);
      pos--;
      continue;
    }
    if(c == '\t'){  /* Pos % TABSTOP for num of spaces to put */
      for(i = pos % TABSTOP; i < TABSTOP; i++){
	putc(' ', stdout);  /* add ' ' till end of tabstop */
	pos++;
      }
    }
    else{
      putc(c, stdout);
      pos++;
    }
  }
  return 0;
}
