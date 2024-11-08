#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#define OPTS "cd"

int length(const char *a);                          /* Returns length */
void translate(const char *set1, const char *set2); /* Translate set1 to set2 */
void delete(const char *set1, int comp);            /* Del input using set1 */
char *extend(const char *set1, const char *set2,
	     int lenSet1, int lenSet2);             /* Extend set2 */

int main(int argc, char *argv[]){
  extern int optind;                    /* Index of arguments */
  extern int optopt;                    /* Opt that doesn't match */
  int opt;                              /* Store opt code char */
  int optc = 0;                         /* c option Flag */
  int optd = 0;                         /* d option Flag */
  char *set1;                           /* Set 1 argument */
  char *set2;                           /* Set 2 argument */
  int lenSet1;                          /* Length of Set 1 argument */
  int lenSet2;                          /* Length of Set 2 argument */
  int minOpt = 0;                       /* Min num opts allowed. FUTURE USE */
  int minArgc = 3;                      /* prog set1 set2 OR prog -d set1 */
  int maxArgc = 4;                      /* prog -d -c set1 */  

  /* PARSE OPTIONS */
  while((opt = getopt(argc, argv, OPTS)) != EOF){
    switch(opt){
    case 'c':
      optc = 1;
      break;
    case 'd':
      optd = 1;
      break;
      /* case '?': */
    default:
      fprintf(stderr,"Unreconized option used: -%c\n", optopt);
      exit(EXIT_FAILURE);
    }
  }
  
  /* XLAT MODES */
  if(optd &&                    /* d option exists */
     argv[optind] &&            /* Set1 exists */
     argv[optind + 1] == NULL){ /* Set2 does not exist */

    /* DELETE MODE*/
    
    set1 = argv[optind];
    
    delete(set1, optc);
  }
  else if(!optc &&                     /* c option does not exist */
	  !optd &&                     /* d option does not exist */
	  argv[optind + 1] &&          /* Set 2 exists*/
	  *argv[optind + 1] != '\0' && /* Set 2 is not empty*/
	  argv[optind + 2] == NULL){   /* Set 3 does not exist*/

    /* TRANSLATE MODE */

    set1 = argv[optind];
    set2 = argv[optind + 1];
  
    /* LENGTH CHECK */

    lenSet1 = length(set1);
    lenSet2 = length(set2);

    if(lenSet2 < lenSet1){
      set2 = extend(set1, set2, lenSet1, lenSet2); /* Extend Set2 */
    }
    
    set2[lenSet1] = '\0';  /* Truncate Set2 to same length as Set1 */
    
    translate(set1, set2);
  }
  else{ /* ERROR HANDELING */
    if(argc < minArgc){
      fprintf(stderr, "Too few arguments\n");
      exit(EXIT_FAILURE);
    }
    else if(argc > maxArgc || (minOpt + argc) > 4){
      fprintf(stderr, "Too many arguments\n");
      exit(EXIT_FAILURE);
    }
    else if(optind >= argc){
      fprintf(stderr, "Expected arguement after options\n");
      exit(EXIT_FAILURE);
    }
    else{
      fprintf(stderr, "Incorrect Input");
      exit(EXIT_FAILURE);
    }
  }
  return 0; 
}

/* Translate input using the last occurring char in Set1 with the */
/* corresponding character in Set2. The correspondence is */
/* determined by their index*/
void translate(const char *set1, const char *set2){
  int c;
  char *p;
  
  while((c = getchar()) != EOF){
    p = strrchr(set1, c);
    if(p){                  /* If char exists in Set1 and input*/
      c = set2[p - set1];   /* replace with char in set 2 */
      putchar(c);
    }
    else{                   /* Keep char */
    putchar(c);
    }
  }
}

/* Delete matching input using the last occurring char in Set 1*/
/* OR delete all non matching input chars if compliment flag exists */
void delete(const char *set1, int comp){
  int c;
  char *p;

  while((c = getchar()) != EOF){
    p = strrchr(set1, c);
    if(comp && (p && c)){ /* Compliment exists, char found in set and input */
      putchar(c);         /* Keep char */
    }
    else if(!comp && (!p || !c)){ /* No Compliment flag, char not found */
      putchar(c);
    } 
  }
}


int length(const char *a){
  int res = 0;
  while (*a++){
    res++;
  }
  return res;
}


char *extend(const char *set1, const char *set2, int lenSet1, int lenSet2){
  char *newSet2;
  int i;

  if(NULL == (newSet2 =  malloc(sizeof(set1) * (lenSet1 + 1)))){
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  else{
    for(i = 0; i < lenSet1; i++){       /* Copy set1 to newSet2 */
      if(i < lenSet2){
	newSet2[i] = set2[i];
      }
      else{
	newSet2[i] = set2[lenSet2 - 1]; /* Extend newSet using last char */
      }
    } 
  }

  return newSet2;
}
