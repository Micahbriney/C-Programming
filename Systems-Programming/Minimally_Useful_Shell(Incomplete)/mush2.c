#include "mush2.h"

char *parse_input(int argc, char *argv[]){
  char *commands;
  FILE *in;
   
  if(argc == 2){
    if(NULL == (in = fopen(argv[1], "r")))
      errorout("fopen()");
    return commands = readLongString(in);
  }
  else if(argc > 2)
    usage();
  else{
    printf("8-P ");
    commands = readLongString(stdin); /* Reads stdin until \n found*/
    return commands;
  }
  return commands;
}

int create_pipes(int *pipefd[2], int numpipes){
  int i;  
  for(i = 0; i < numpipes; i++)
    if(-1 == pipe(pipefd[i])){
      perror("pipe()");
      return -1;
    }
  return 0;
}

void block_sigint(){
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  if(-1 == sigprocmask(SIG_BLOCK, &set, NULL))
    errorout("sigprocmask block");
  return;
}

void unblock_sigint(){
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  if(-1 == sigprocmask(SIG_UNBLOCK, &set, NULL))
    errorout("sigprocmask unblock");
  return;
}

int parent_process(){
  int status;
  int end;
  pid_t pidc;

  status = 0;
  /* Wait for all child processes to terminate*/
  
  child_wait();

  return status;
}

int child_process(pipeline crpl, int numpipes, int **pipefds){
  int status, i;
  int infd, outfd;
  unblock_sigint();
  i = 0;
  if(numpipes > 0){ /* First forked child. Close */
    if(crpl->stage->outname == NULL){ /* No redirection*/
      if(-1 == (dup2(pipefds[i][WRITE_END_], STDOUT_FILENO)))
	errorout("Dup2 first forked child. No redirection");
    }
    else if(crpl->stage->outname != NULL){ /* Redirection from pipe to file*/
      if(-1 == (outfd = open(crpl->stage->outname, O_WRONLY)))
	errorout("openopen first forked child's outfd");
      if(-1 == (dup2(outfd, STDOUT_FILENO)))
        errorout("Dup2 first forked child with redirection");
    }
    if(crpl->stage->inname != NULL){ /* Redirection from infile to pipe*/
      if(-1 == (infd = open(crpl->stage->inname, O_RDONLY)))
	errorout("open first forked child's infd");
      if(-1 == (dup2(infd, STDIN_FILENO)))
        errorout("Dup2 first forked child with redirection");
    }
  }

  /* Subsequent children outputs*/
  for(i = 1; i < numpipes; i++){
    if(crpl->stage->outname == NULL){ /* No redirection*/
      if(-1 == (dup2(pipefds[i][WRITE_END_], STDOUT_FILENO)))
	errorout("Dup2 subsequent forked child. No redirection");
    }
    else if(crpl->stage->outname != NULL){ /* Redirection from pipe to file*/
      outfd = open(crpl->stage->outname, O_WRONLY);
      if(-1 == (dup2(outfd, STDOUT_FILENO)))
        errorout("Dup2 subsequent forked child with redirection");
    }

    /* Subsequent children inputs*/
    if(crpl->stage->inname == NULL){ /* No redirection*/
      if(-1 == (dup2(pipefds[i][READ_END_], STDIN_FILENO)))
	errorout("Dup2 subsequent forked child. No redirection");
    }
    else if(crpl->stage->outname != NULL){ /* Redirection from pipe to file*/
      infd = open(crpl->stage->inname, O_RDONLY);
      if(-1 == (dup2(infd, STDOUT_FILENO)))
	errorout("Dup2 subsequent forked child with redirection");
    }
  }

  closefds(pipefds, numpipes); /* Close file descriptorsstdout*/

  
  
  /* Need to check if there are argc > 1*/
  execl(crpl->cline, crpl->stage->argv[0]);
  return status;
}

/**
 *
 *@brief: 
**/
void sigint_handler(int signum){

  // DONT WAIT here.
  
}

int child_wait(){
  pid_t pidc;
  int status;
  status = 0;
  while (-1 != (pidc = waitpid(-1, &status, 0))) {
    /* Check for SIGINT*/
    /* For testing*/
    printf("Process %d exited with status %d.\n", pidc, WEXITSTATUS(status));
  }
  if(errno == ECHILD)
    errorout("waitpid()");
  return 0;
}

void closefds(int **pipefds, int numpipes){
  int i;

  for(i = 0; i < numpipes; i++){
      close(pipefds[i][READ_END_]);
      close(pipefds[i][WRITE_END_]);
  }
  return;
}

void errorout(char *error){
  perror(error);
  exit(EXIT_FAILURE);
}

void usage(){
  fprintf(stderr ,"Usage: mush2 [file]\n");
  exit(EXIT_FAILURE);
}
