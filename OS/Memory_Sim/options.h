#ifndef OPT_H
#define OPT_H

#include <stdint.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define MIN_ARGS_          4
#define PROG_NAME_LOC_     0
#define FILE_NAME_LOC_     1
#define FRAME_NUM_LOC_     2
#define ALGO_NUM_LOC_      3
#define VERBOSITY_LOC_     4
#define MAX_FRAMES_        255
#define DEFAULT_ALGO_      FIFO
#define MAX_ALGO_NAME_LEN_ 4

#define MAX_FRAME_SIZE_ 256

typedef enum {FIFO, LRU, OPT, INVALID = 255} Algorithm;

extern uint8_t verbosity;

typedef struct algorithm_map{
    const char* algorithm_name;
    Algorithm algorithm;
} algo_map;

void parseOptions(int argc, char * const argv[], char **filename, uint8_t *num_frames, Algorithm *algorithm);
uint8_t getFrameNum(char *frame_num);
uint8_t getAlgorithm(char *algorithm);
void usage(char *prog_name);
void errorout(char *error);

#endif