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
#include <limits.h>
#include "options.h"

FILE *safefOpen(char *file){
    FILE *file_pt;

    errno = 0; /* Clear any errno*/

    if(NULL == (file_pt = fopen(file, "r"))) /* Check for fopen error*/
        errorout("Fopen failed\n");
    
    return file_pt;
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
    if((new_ptr = (void *)realloc(ptr, new_size * type_size)) == NULL) /* Check for realloc error*/
        errorout("Safe realloc failed\n");

    
    memset((char*)new_ptr + (curr_size * type_size), 
           0, 
           (new_size - curr_size) * type_size); /* Zero out new mem*/
    
    return new_ptr;
}

long safeStrtol(const char *str){
    char *endptr;
    long value = strtol(str, &endptr, 10);

    if(!str) /* String is null*/
        return 0;

    if(str[0] == '\0' || endptr == str) /* Nothing found || No nums found*/
        return 0;
    
    /* Check that value is within long range*/
    if(!(*endptr == '\0' && value >= LONG_MIN && value <= LONG_MAX))
        return 0;

    return value;
}