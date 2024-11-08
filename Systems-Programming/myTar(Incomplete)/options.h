#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OPTSTRING_ "ctxvSf"

#define C_FLAG_MASK_ 0x01 /* Create flag*/
#define T_FLAG_MASK_ 0x02 /* List Table of contents flag*/
#define X_FLAG_MASK_ 0x04 /* Extract flag*/
#define V_FLAG_MASK_ 0x08
#define F_FLAG_MASK_ 0x10
#define S_FLAG_MASK_ 0x20
#define ACTION_MASK_ (C_FLAG_MASK_ | T_FLAG_MASK_ | X_FLAG_MASK_)

void options(int argc, char *argv[], int *flags);
void print_usage_exit(char *prog);
int errors();

#endif
