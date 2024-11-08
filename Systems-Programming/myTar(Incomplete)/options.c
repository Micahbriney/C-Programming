/**
 * @Author: Micah Briney
 * @Date: 11/2/2022
 * @File: options.c
 * @Brief: The options file will handle all the possible options when 
 *          the program mytar is envoked. The possible options are:
 *          c: Optional. Create an archive of the file.
 *          t: Optional. Print the table of contents of of the tarfile. 
 *          x: Optional. Extract the contents of an the archive tarfile.
 *          b: Optional. Increases the verbosity of the actions of mytar.
 *              Paired with 'c' mytar list the files as they are added.
 *              Paired with 't' mytar will give expanded info about the
 *              archived files. Such as permissions, owner/group, size, 
 *              last modified date (mtime) and the filename.
 *              Paired with 'x' mytar list the files as they are extracted.
 *          f: NON OPTIONAL: Specifies archive filename.
 *          S: Optional: Forces mytar to be strict about compliacne standards
 *          A flags vairable will hold which options where used. 
 *          c = 0x01, t = 0x02, x = 0x04, v = 0x08, f = 0x10, S = 0x20

**/

#include "options.h"

/**
 * @fn: options
 * @brief: parse terminal options and set the flag accordingly. On success
 *         options() will return the opt index of the tarfile.
 * @params: int argc, char *argv[], int &flags
 * @return: int
 **/
void options(int argc, char *argv[], int *flags){
  char *opts; /* Will point to opts*/
  int i, len;
  
  *flags = 0; /* Clear flags*/
  if(argc <= 2)
    print_usage_exit(argv[0]);  
  
  opts = *(++argv);/* Points at options*/
  len = strlen(opts);
  for(i = 0; i < len; i++){
    switch(opts[i]){
    case 'c':
      if(*flags & ACTION_MASK_) /* More than one action option was entered*/
	print_usage_exit(argv[0]);
      *flags |= C_FLAG_MASK_;   /* |= 0x1*/
      break;
    case 't':
      if(*flags & ACTION_MASK_) /* More than one action option was entered*/
	print_usage_exit(argv[0]);
      *flags |= T_FLAG_MASK_;   /* |= 0x02*/
      break;
    case 'x':
      if(*flags & ACTION_MASK_) /* More than one action option was entered*/
	print_usage_exit(argv[0]);
      *flags |= X_FLAG_MASK_;   /* |= 0x04*/
      break;
    case 'v':
      *flags |= V_FLAG_MASK_;   /* |= 0x08*/
      break;
    case 'f':
      *flags |= F_FLAG_MASK_;   /* |= 0x10*/
      break;
    case 'S':
      *flags |= S_FLAG_MASK_;   /* |= 0x20*/
      break;  
    default: /* ? */
      print_usage_exit(argv[0]);  
    }
  }

  /* f flag must be set*/
  if(!(*flags & F_FLAG_MASK_))
    print_usage_exit(argv[0]);
  
  return;
}

void print_usage_exit(char *prog){
  fprintf(stderr, "Usage: %s ([ctx])[v][S]f tarfile [path [...] ]\n", prog);
  exit(EXIT_FAILURE);
}
