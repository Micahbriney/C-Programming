/**
 ******************************************************************************
 * @author		   : Micah Briney
 * @date		   : 5-12-2023
 * @version        : 1
 * @file           : safeutil.c
 * @brief          : This is the source file for some basic safe utilities.
 *                    This includes malloc, fopen and realloc. 
 *
 * OS	    : Unix 
 * compiler	: gcc
 ******************************************************************************
 */
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include "safeutil.h"

int safeOpen(char *pathname, int flags, int mode){
    int fd;

    errno = 0; /* Clear any errno*/
    if(-1 == (fd = open(pathname, flags, mode))) /* Check for fopen error*/
        errorout("open failed\n");
    
    return fd;
}

void *safeMalloc(size_t size){
    void *ptr = NULL;
    if((ptr = (void *)malloc(size)) == NULL)
        errorout("Safe malloc failed\n");
    
    memset(ptr, 0, size); /* Zero out memory*/

    return ptr;
}

void *safeRealloc(void *ptr, size_t new_size, size_t curr_size, size_t type_size){
    void *new_ptr = NULL;
    if((new_ptr = (void *)realloc(ptr, new_size)) == NULL) /* Check for realloc error*/
        errorout("Safe realloc failed\n");

    
    memset((char*)new_ptr + (curr_size * type_size), 
           0, 
           (new_size - curr_size) * type_size); /* Zero out new mem*/
    
    return new_ptr;
}

/**
 * @fn	   : errorout
 * @brief  : This function prints error information and exits the program.
 *            
 * @retval : void
 * @parms  : char *error
 */
void errorout(char *error)
{
    if(errno)
        perror(error);
    else
        fprintf(stderr, "%s", error);
    
    exit(EXIT_FAILURE);
}