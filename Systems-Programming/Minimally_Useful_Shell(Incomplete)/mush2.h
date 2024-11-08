#ifndef MUSH2_
#define MUSH2_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <mush.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


#define PIPEFD_MAX_ 2
#define READ_END_  0
#define WRITE_END_ 1

char *parse_input(int argc, char *argv[]);
int create_pipes(int *pipefd[2], int numpipes);

void block_sigint(void);
void unblock_sigint(void);

int parent_process();
int child_process(pipeline crpl, int numpipes, int **pipefds);

void sigint_handler(int signum);
int child_wait();

void closefds(int ** pipefds, int numpipes);

void errorout(char *error);
void usage();


#endif
