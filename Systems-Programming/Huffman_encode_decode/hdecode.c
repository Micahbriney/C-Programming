/**
 ***************************************************************************
 * @author : Micah Briney
 * @date   : 10-19-2022
 * @file   : hdecode
 * @brief  :  
 *
 ***************************************************************************
 */

#include "htable.h"
#include <sys/stat.h>

#define OPTARGS_ "--"
#define ALLUSERPERMS_ (S_IRUSR | S_IWUSR | S_IXUSR)
#define SIZE_ 4096

extern char *optarg;

int getopt(int argc, char *const argv[], const char *optstring);
char *read_encode_file(int infd, int outfd);

/**
 **************************************************************************
 * @fn    : main
 * @brief : Entry into decode application.
 *
 **************************************************************************
 */
int main(int argc, char *argv[]){
  int opt;              /* Hold opt value returned from getopt*/
  int stdinflag;
  int infd, outfd;
  char *infilename, *outfilename;
  char *bitstream;
  
  /* First check for operations*/
  if(argc > 1 && argc <= 3){
    stdinflag = 0;
    while(-1 != (opt = getopt(argc, argv, OPTARGS_))){
      switch(opt){
      case 1:     /* Standard in flag set*/
	if(optarg && !strcmp(optarg, "-"))
	  stdinflag = 1;
	break;
      default: /* '?' */
	;/*usage_hdecode_error(argv[0]);*/
      }
    }
  }

  if(argc == 1){
    infd  = STDIN_FILENO;
    outfd = STDOUT_FILENO;
  }

  if(argc == 2){
    if(!stdinflag){
      infilename = argv[1];
      if(-1 == (infd = open(infilename, O_RDONLY))){
	perror("Infile Open");
	exit(EXIT_FAILURE);
      }
    }
    else
      infd = STDIN_FILENO;
    outfd = STDOUT_FILENO;
  }

  if(argc == 3){
    if(!stdinflag){
      infilename = argv[1];
      if(-1 == (infd = open(infilename, O_RDONLY))){
	perror("Infile Open");
	exit(EXIT_FAILURE);
      }
    }
    else
      infd = STDIN_FILENO;
    outfilename = argv[2];
    outfd = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, ALLUSERPERMS_);
  }

  /* Check for open infile errors*/
  if(-1 == infd){
    perror("infile");
    exit(EXIT_FAILURE);
  }
  else if(-1 == outfd){ /* Check for open outfile errors */
    perror("outfile");
    exit(EXIT_FAILURE);
  }

  bitstream = read_encode_file(infd, outfd);

  return 0;
}


/* Function will take in an encoded array
   Then it will read the uniqnum header which is 1 byte wide
   this is one uniqnum to short so add 1
   next it will read the char which is 1 byte wide
   next it will read in the count which is 4 bytes wide
   next it will fix or no op htonl byte order
   then loop untill i < uniqnum + 1
   then it can read the encoded bits.
*/

char *read_encode_file(int infd, int outfd){
  char *buff;
  int size, n;
  char *bitstream;
  
  /* Create room for buffer*/
  if(!(buff = (char *)(malloc(sizeof(char) * SIZE_)))){
    perror("buffer malloc");
    exit(EXIT_FAILURE);
  }

  size = 0;
  /* Read input file*/
  while(0 < (n = read(infd, buff, SIZE_))){ /* Read a chunk*/
    if(n < 0){ /* Problem with reading input file*/
      perror("Read infile");
      exit(EXIT_FAILURE);
    }
    /* Create more space for buffer*/
    
    size += n; /* Gather total size of file*/      
  }

  /* Check for empty file*/
  if(!size){/* Empty file found. Close files and exit program*/
    free(buff);
    close(infd);
    close(outfd);
    exit(EXIT_SUCCESS);
  }

  /* Create room for buffer*/
  if(!(bitstream = (char *)(malloc(sizeof(char) * size)))){
    perror("buffer malloc");
    exit(EXIT_FAILURE);
  }
  
  if(buff)/* If buffer is populated */
    strncpy(bitstream, buff, size);

  free(buff);

  return bitstream;
}

