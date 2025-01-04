#include "options.h"
#include "safeutil.h"

uint8_t verbosity = 0;

/**
 * @fn	   : parseOptions
 * @brief  : This function parse the user input options. It checks for usage
 *            errors and will populate the filename, number of frams and the
 *            specified algorithm. It can also set a verbosity flag.
 *            
 * @retval : void
 * @parms  : int argc, char * const argv[], char **filename, 
 *           uint8_t *num_frames, uint8_t *algorithm
 */
void parseOptions(int argc, 
                  char * const argv[], 
                  char **filename, 
                  uint8_t *num_frames, 
                  Algorithm *algorithm)
{
    char *prog_name = argv[PROG_NAME_LOC_];
    /* Check for usage errors*/
    if(argc < MIN_ARGS_)
        usage(prog_name);

    if(argc > MIN_ARGS_ && (tolower(argv[VERBOSITY_LOC_][0]) == 'v' || 
                            tolower(argv[VERBOSITY_LOC_][1]) == 'v')) /* Verbosity flag might be set*/
        verbosity = 1;
    else if (argc > MIN_ARGS_)
        usage(prog_name);
    
    *filename = argv[FILE_NAME_LOC_];
    *num_frames = getFrameNum(argv[FRAME_NUM_LOC_]);
    *algorithm = getAlgorithm(argv[ALGO_NUM_LOC_]);

    if(verbosity){
        printf("Filename: %s.\n", *filename);
        printf("Number of Frames: %d.\n", (*num_frames) + 1);
        printf("Algorithm: %d. Note FIFO = 0, LRU = 1, OPT = 2\n", *algorithm);
    }

    return;
}


/**
 * @fn	   : getFrameNum
 * @brief  : This function will check and convert the user input frame number
 *            from a string to a uint8_t.
 *            
 * @retval : void
 * @parms  : char *frame_num
 */
uint8_t getFrameNum(char *frame_num)
{
    long ret = 0;
    
    if(!frame_num) /* Somehow frame_num string is null*/
        errorout("Set Frame Number: Empty frame_num string.\n");
    
    ret = safeStrtol(frame_num) - 1; /* Adjust from 1-256 to 0-255*/
    
    if(ret < 0 || ret > (UINT8_MAX)){/* Choose not to error. Use default on input error*/
        printf("Frame Number defaulted to max 255\n");
        return MAX_FRAMES_;
    } /* No valid Frame Number found*/
    
    return (uint8_t)ret;
}


uint8_t getAlgorithm(char *algorithm){
    char lower_algorithm[MAX_ALGO_NAME_LEN_ + 1] = {0};
    uint8_t ret = INVALID; /* Will default to FIFO*/
    int i;
    static const algo_map a_map[] = {
        {"fifo", FIFO}, 
        {"lru", LRU},
        {"opt", OPT}
    };
    static const uint8_t num_algorithms = sizeof(a_map) / sizeof(a_map[0]);

    if(!algorithm) /* Something went wrong*/
        errorout("Get Algorithm: Empty Algorithm string.\n");

    for(i = 0; i < (int)strlen(algorithm); i++) /* Remove case sensitivity*/
         lower_algorithm[i] = tolower(algorithm[i]); /* Send to lower*/
    
    lower_algorithm[i] = '\0'; /* Null terminate*/
    
    /* Check if user input matches existing algorithm*/
    for(i = 0; i < num_algorithms; i++) /* While not null*/
        if(strcmp(lower_algorithm, a_map[i].algorithm_name) == 0)
            ret = a_map[i].algorithm;
    
    /* Choosing not to error on input error.*/
    if(ret == INVALID){ /* Using default on input error*/
        printf("Algorithm defaulted to FIFO.\n");
        return DEFAULT_ALGO_;
    }

    return ret;
}

/**
 * @fn	   : usage
 * @brief  : This function prints usage information and exits the program.
 *            
 * @retval : void
 * @parms  : char *prog_name
 */
void usage(char *prog_name)
{
    fprintf(stderr,
            "Usage: %s <reference-sequence-file.txt> <FRAMES> <PRA> [v | -v]\n",
            prog_name);
    exit(EXIT_FAILURE);
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