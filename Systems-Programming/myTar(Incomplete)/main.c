/**
 * @Author: Micah Briney
 * @Date: 11/2/2022
 * @File: main.c
 * @Brief:  
 *          
 *          
 *          
 *          
 *          
 *          
 *          
 *          
 *          
 *          
 *          
 *          
**/

#include "options.h"
#include "modes.h"

/**
 *@fn: main
 *@brief: Entry into the application.
 *@params: int argc, char *argv[]
 *@return: int
**/
int main(int argc, char *argv[]){
  char *archivefile;
  char **files;
  int optflags;
  int result;
  options(argc, argv, &optflags);

  archivefile = argv[2]; /* Should be here*/
  files = argv + 3; /* Files/dirs should start here*/
  
  if(optflags & C_FLAG_MASK_)/* Check Action flag Create archive*/
    result = create_mode(archivefile, files, optflags);
  else if(optflags & T_FLAG_MASK_) /* Check Action flag Table of Contents*/
    result = table_contents_mode(archivefile, files, optflags);
  else if(optflags & X_FLAG_MASK_) /* Check Action flag Extract archive*/
    result = extract_mode(archivefile, files, optflags);
  
  
  return result;
}

/**
 *@fn: errors
 *@brief: Keep track of the errors here. Unsure if I can make a generic 
           error function for all errors. Probably not.
 *@return: int
 *@params: none
 **/
int errors(){
  static int numerrors = 0; /* Keep a running total of errors*/
  
  /* Might need to store certain error messages??*/
  return numerrors;
}
