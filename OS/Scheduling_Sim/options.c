/**
 ******************************************************************************
 * @author		   : Micah Briney
 * @date		   : 5-12-2023
 * @version        : 1
 * @file           : options.c
 * @brief          : This is the source file for parsing the options from the
 *                    terminal.
 *
 * OS	    : Unix 
 * compiler	: gcc
 ******************************************************************************
 */
#define _GNU_SOURCE
#include "options.h"
#include "schedSim.h"

uint8_t verbosity = 0;
const char *ALGORITHM_NAMES[] = {"srtn", "fifo", "rr", NULL};

void parseOptions(int argc, char * const argv[], uint8_t *algorithm, unsigned int *quantum){
    char *prog = argv[0];
    char opt;
    char *algorithm_input = "FIFO";  /* User Arg. Default FIFO*/
    char *quantum_input = "1";       /* User Arg. Defualt 1*/
    unsigned int numargs;

    if(argc < 2 || (argc == 2 && argv[1][0] == '-')){ /* Missing input file*/
        fprintf(stderr, "Input file required.\n");
        usage(prog);
    }


    while(-1 != (opt = getopt(argc, argv, OPTSTRING_))){
        switch(opt){
            case 'V':
            case 'v':         /* Verbose mode*/
                verbosity = 1;
                break;
            case 'P':
            case 'p':         /* Pick an algorithm*/
                algorithm_input = optarg;
                break;
            case 'Q':
            case 'q':         /* Set a quantum length for Round Robin*/
                quantum_input = optarg;
                break;
            default: /* '?' invalid option found*/
                usage(prog);
        }
        if(optind % 2 != 0 && !verbosity){  /* if optind is not even*/
            printf("Input file required.\n");
            usage(prog);
        }
    }

    numargs = argc - optind;

    if(numargs > (verbosity ? MINARGS_ + 1 : MINARGS_)){ /* To many args after opts*/
        fprintf(stderr, "To many arguments after options.\n");
        usage(prog);
    }
    else if(numargs < MINARGS_){ /* To few args after opts*/
        fprintf(stderr, "Too few options.\n");
        usage(prog);
    }


    if(verbosity){
        char *job_file = argv[optind];
        printf("Job files: %s\n", job_file);
        printf("Algorithm: %s\n", algorithm_input);
        printf("Quantum: %s\n\n",   quantum_input);
    }

    *algorithm = setAlgorithm(algorithm_input);
    *quantum   = setQuantum(quantum_input);

    if(verbosity){
        printf("String to int conversion:\n");
        printf("Algorithm: %d. Note SRTN = 0, FIFO = 1, RR = 2\n", *algorithm);
        printf("Quantum: %d\n", *quantum);
    }
    return;
}

uint8_t setAlgorithm(char *algorithm){
    char lowerAlgorithm[MAX_ALGO_NAME_LEN_ + 1] = {0};
    uint8_t ret = INVALID; /* Default*/
    int i;
    static const algo_map a_map[] = {
        {"srtn", SRTN}, 
        {"fifo", FIFO},
        {"rr", RR}
    };
    static const uint8_t num_algorithms = sizeof(a_map) / sizeof(a_map[0]);

    if(!algorithm) /* Something went wrong*/
        errorout("Set Algorithm: Empty Algorithm string.\n");

    for(i = 0; i < (int)strlen(algorithm); i++)
         lowerAlgorithm[i] = tolower(algorithm[i]); /* Send to lower*/
    
    lowerAlgorithm[i] = '\0'; /* Null terminate*/
    
    /* Check if user input matches existing algorithm*/
    for(i = 0; i < num_algorithms; i++) /* While not null*/
        if(strcmp(lowerAlgorithm, a_map[i].algorithm_name) == 0)
            ret = a_map[i].algorithm;
    
    if(ret == INVALID){ /* Choose not to error. Use default on input error*/
        printf("Algorithm defaulted to FIFO.\n");
        return DEFAULT_ALGORITHM_;
    }

    return ret;
}

unsigned int setQuantum(char *quantum){
    unsigned int ret = 0;
    if(!quantum) /* Somehow quantum string is null*/
        errorout("Set Quantum: Empty quantum string.\n");
    
    ret = safeStrtol(quantum);

    if(ret == 0){/* Choose not to error. Use default on input error*/
        printf("Quantum defaulted to 1\n");
        return DEFAULT_QUANTUM_;
    } /* No valid quantum found*/
    
    return ret;
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

void usage(char *argv){
    fprintf(stderr,
            "Usage: %s <job-file.txt> -p <ALGORITHM> -q <QUANTUM> -v\n",
            argv);
    exit(EXIT_FAILURE);
}