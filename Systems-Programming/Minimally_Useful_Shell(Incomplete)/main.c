#include "mush2.h"
#include <stdint.h>


int main(int argc, char *argv[]){
  pid_t pid;
  struct sigaction sa;
  int      **pipefds;
  sigset_t  set;
  char     *commands;
  pipeline  crpl;
  int8_t    terminal;
  int       numpipes, i;
  
  terminal = 1;

  /* Install SIGINT handler before reading the pipeline*/
  sigemptyset(&set);
  sa.sa_handler = &sigint_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if(-1 == sigaction(SIGINT, &sa, NULL))
    errorout("sigaction");
  
  while(terminal != EOF){/* Wait for EOF Ctrl D signal.*/
    /* Block SIGINT*/
    block_sigint();
    commands = parse_input(argc,argv);
    
    if(commands == NULL)
      errorout("readLongString()");
    else if(*commands == EOF){
      terminal = EOF;
      break;
    }
    
    if(NULL == (crpl = crack_pipeline(commands)))
      errorout("crack_pipeline()");

    /* For testing*/
    /* print_pipeline(stdout, crpl);*/
    /*printf("\nNum of pipes?: %d\n", numpipes); How many pipes?*/

    numpipes = crpl->length;

    /* Make room for the pipefds*/
    if(numpipes > 0){
      if(!(pipefds = (int**)malloc(numpipes * sizeof(int*))))
	errorout("malloc **fd_pair"); /*Malloc returned NULL*/
      for(i = 0; i < numpipes; i++)
	if(!(pipefds[i] = (int*)malloc(PIPEFD_MAX_ * sizeof(int))))
	  errorout("malloc fd_pair[i]"); /*Malloc returned NULL*/
	
      if(-1 == create_pipes(pipefds, numpipes))
	continue; /* Something wrong with pipe()*/
    }
    
    
    /* Fork a child for each pipe*/
    for (i = 0; i < numpipes; i++) {
      if ((pid = fork()) < 0)
	perror("fork()");
      else if(pid == 0) /* Child process. Pid will hold parent pid*/
        child_process(crpl, i, pipefds);
      else /* Parent process.*/
	parent_process();
    }
    
    free_pipeline(crpl);
  }/* while(terminal)*/  
  return 0;
}
