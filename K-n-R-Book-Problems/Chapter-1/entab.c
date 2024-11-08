/* Program: entab
   Description: exchanges multiple blanks (spaces) with tabs, when appropriate, 
                to achieve the same spacing between input and output. 
*/

#include <stdio.h>
#include <stdlib.h>

#define TABSTOP 8

int main(int argc, char *argv[]){
  int c, i, location, prevchar, spcount;

  location = spcount = 0;

  if(ferror(stdin)){
    printf("stdin failed to work with %s.", argv[0]);
    perror("Reason:");
    exit(EXIT_FAILURE);
  }
  if(ferror(stdout)){
    printf("stdout failed to work with %s.", argv[0]);
    perror("Reason:");
    exit(EXIT_FAILURE);
  }

  while((c = getchar()) != EOF){
    if(c == '\n' || c == '\r'){
      putc(c, stdout);     
      location = 0;
      prevchar = c;
      continue;
    }
    if(c == '\b' && location == 0){ /* Don't allow location to go below 0*/
      putc(c, stdout);
      prevchar = c;
      continue;
    }
    if(c == '\b' && prevchar == '\t'){ /* TODO fix me*/
      putc(c, stdout);
      continue;
    }
    if(c == '\b'){
      putc(c, stdout);
      location--;
      prevchar = c;
      continue;
    }
    if(c == ' '){
      spcount++;
      location++;
      prevchar = c;
      while((c = getchar()) != EOF && c == ' '){
	spcount++;
	location++;
	if(location % TABSTOP == 0){
	  putc('\t', stdout);
	  spcount = 0;
	  prevchar = '\t';
	}
      }
      for(i = 0; i < spcount; i++){
	putc(' ', stdout);
      }
      spcount = 0;
      putc(c, stdout);
      location++;
    }
    else{
      putc(c, stdout);
      location++;
    }
  }
}
