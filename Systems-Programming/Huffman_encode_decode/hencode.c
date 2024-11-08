/**
 ***************************************************************************
 * @author : Micah Briney
 * @date   : 10-19-2022
 * @file   : hencode
 * @brief  :  
 *
 ***************************************************************************
 */

#include "htable.h"
#include <sys/stat.h>
#include <limits.h>

#define MSBYTE_ 1
#define LSBYTE_ 4
#define ONEBYTE_   (1 << 8)
#define TWOBYTE_   (1 << 16)
#define THREEBYTE_ (1 << 24)
#define BYTEMASK_   0xFF

void write_hencode_file(listptr head, int outfd, int uniqchars);
void build_header(listptr head, char bitstream[], int uniqchars, int outfd);
void print_list_info(listptr head, int uniqchars);

int main(int argc, char * argv[]){
  int infd, outfd;
  int uniqchars, i, index;
  char *infilename, *outfilename;
  uint32_t freqArr[ARRSIZECHAR_][ARRSIZECOUNT_];
  hnode histogram[ARRSIZECHAR_];
  char encodedArr[ENCODESIZE_];
  listptr head;
  treeptr root;
  
  if(argc > 1){
    infilename = argv[1];
    if(-1 == (infd = open(infilename, O_RDONLY))){
      perror("Infile Open");
      exit(EXIT_FAILURE);
    }
    if(argc == 3){
      outfilename = argv[2];
      if(-1 == (outfd = open(outfilename, O_RDONLY))){
	perror("Outfile Open");
	exit(EXIT_FAILURE);
      }
    }
    else if(argc > 3)
      usage_hencode_error(argv[0]);
    else
      outfd = STDOUT_FILENO;
  }
  else{
    infd = STDIN_FILENO;
    outfd = STDOUT_FILENO;
  }
  
  /* Read input*/
  read_input(infd, freqArr);

  /* Find the number of unique charaters that exist in the file*/
  uniqchars = 0; /* Initialize to zero*/
  uniqchars = get_uniqnum(freqArr);

  /* Initilized an array of histogram nodes*/
  create_histogram(histogram);
  
  /* Create an array of histogram nodes*/
  build_histogram(histogram, freqArr);

  /* Sort histogram by count. They remain sorted alphabetically on tie*/
  qsort(histogram, uniqchars, sizeof(hnode), compare_count);

  /* Create list and return head*/
  head = init_list(histogram, uniqchars);

  /* Truncate list*/
  head = truncate_list(head, uniqchars);

  /* Turn list into tree*/
  root = init_tree(head);

  /* Scrub encodedArr*/
  for(i = 0; i < ENCODESIZE_; i++)
    encodedArr[i] = 0;   /* Zero out count*/

  index = 0;
  encode_post_order_traversal(root, index, encodedArr);

  /* ----- For Testing ------*/
  /*print_list_info(head, uniqchars);*/

  write_hencode_file(head, outfd, uniqchars);
  
  return 0;
}


void write_hencode_file(listptr head, int outfd, int uniqchars){
  char *headerbitstream;
  ssize_t n;
  if(!(headerbitstream = (char *)malloc((sizeof(char) * uniqchars) + 1))){
    perror("Malloc headerbitstream");
    exit(EXIT_FAILURE);
  }

  headerbitstream[0] = (char)(uniqchars - 1);
  
  /* Set the header count*/
  if(-1 == (n = write(outfd, headerbitstream, 1))){
    perror("Write head count");
    exit(EXIT_FAILURE);
  }
	    
  build_header(head, headerbitstream, uniqchars, outfd);

  /* TODO Add the encoded bits to the file*/
  
  return;
}


void build_header(listptr head, char bitstream[], int uniqchars, int outfd){
  listptr temp;
  int i, j, limit, n;
  char *bit;

  if(!(bit = (char *)malloc(sizeof(char)))){
    perror("Malloc bit");
    exit(EXIT_FAILURE);
  }
    
  temp = head;
  bitstream++; /* advance the address past the head counter*/
  for(i = 0; i < (uniqchars + 1) * 5; i *= 5){ /*+1 head count. *5 offset*/
    if(temp->c == '\0') /* Skip super nodes*/
      i--;
    else{
      *bit = (char)temp->c;
      if(-1 == (n = write(outfd, bit, 1))){
	perror("Write char bit");
	exit(EXIT_FAILURE);
      }


      limit = temp->count;
      /* For each loop logical shift right by a byte and add j. If its 
	 larger than a byte + j then keep going*/
      for(j = 0; j < limit; j++){
	*bit = '\0';
	if(-1 == (n = write(outfd, bit, 1))){
	  perror("Write char count null");
	  exit(EXIT_FAILURE);
      	}
	/* Shift off lower byte. Increase remaining bits by j. Incase
	   remaining bits a less then j after shift*/
	limit = (limit >> 8) + j;
      }
      
      /* For the remaining bytes. Shift count >> by (32-8*j); 24, 16, 8*/
      for( ; j <= 4; j++){
	*bit = (char)(temp->count >> (32 - 8 * j));
	if(-1 == (n = write(outfd, bit, 1))){
	  perror("Write char count null");
	  exit(EXIT_FAILURE);
	}	
      }     
    }
    
    if(temp->next)
      temp = temp->next;
  }
  return;
}


/*
 *@fn: print_list_info
 *@brief: Print ordered list skipping super nodes. Used for testing how 
 *         the header will be built.
*/
void print_list_info(listptr head, int uniqchars){
  listptr temp;
  int i;
  temp = head;
  
  for(i = 0; i < uniqchars; i++){
    if(temp->c == '\0') /* Skip super nodes*/
      i--;
    else{
      printf("Character: %c. ", temp->c);
      printf("Count: %d. ", temp->count);
      printf("Encode: %s.\n", temp->codestr);
    }

    if(temp->next)
      temp = temp->next;
    
  }
  return;
}
