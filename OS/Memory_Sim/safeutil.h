/**
 ******************************************************************************
 * @author		   : Micah Briney
 * @date		   : 5-12-2023
 * @version        : 1
 * @file           : safeutil.h
 * @brief          : This is the header file for the safe utilities source
 *                    file. This includes function prototypes for malloc, fopen 
 *                    and realloc.
 *
 * OS	    : Unix 
 * compiler	: gcc
 ******************************************************************************
 */
#ifndef SAFE_UTIL_H
#define SAFE_UTIL_H

FILE *safefOpen(char *file);
void *safeMalloc(size_t size);
void *safeRealloc(void *ptr, size_t new_size, size_t curr_size, size_t type_size);
long safeStrtol(const char *str);

#endif