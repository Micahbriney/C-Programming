/**
 ******************************************************************************
 * @author		   : Micah Briney
 * @date		   : 5-12-2023
 * @version        : 1
 * @file           : options.c
 * @brief          : This is the header file for options source file. It 
 *                    includes libraries, macros and function prototypes
 *                    used by the program to interrpret terminal input.
 *
 * OS	    : Unix 
 * compiler	: gcc
 ******************************************************************************
 */
#ifndef OPT_H
#define OPT_H

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

typedef enum {SRTN = 0, FIFO = 1, RR = 2, INVALID = 255} Algorithm;

typedef struct algorithm_map{
    const char* algorithm_name;
    Algorithm algorithm;
} algo_map;

#define MINARGS_          1      /* Min number of arguments: File name*/
#define OPTSTRING_        "q:p:vQ:P:V" /* Usage Options */
#define DEFAULT_ALGORITHM_ FIFO
#define DEFAULT_QUANTUM_   1
#define MAX_ALGO_NAME_LEN_ 4

extern uint8_t verbosity;

void parseOptions(int argc, char * const argv[], uint8_t *algorithm, unsigned int *quantum);
uint8_t setAlgorithm(char *algorithm);
unsigned int setQuantum(char *algorithm);
long safeStrtol(const char *str);
void usage(char *argv);

#endif