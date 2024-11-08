/**
 ***************************************************************************
 * @author	: Micah Briney
 * @date	: 11/14/2022
 * @version     : 2
 * @file	: trykit.c
 * @brief	: This program utlizes fork() and execl(). To demonstrate, a 
			  command-line argument of the path to a program (absolute or 
			  relative) is entered, a child is created via fork() that tries
			  to execl() the given program, and reports on its success or 
			  failure. A child that exits with a status of 0 is assumed to 
			  be a success, non-zero is a failure. If the execl() fails, the
			  child should print why (via perror()), and exit with a 
			  non-zero status.
		   
 * @compiler: gcc
 ***************************************************************************
 */
 
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void errorout(char *error);

int main(int argc, char *argv[]){
  char *path;
  pid_t pid;

  
  if(argc != 2){ /* Only one command should exist*/
    fprintf(stderr, "usage: tryit command\n");
    exit(EXIT_FAILURE);
  }
  
  path = argv[1];

  if((pid = fork()) < 0)     /* Create a child process*/
    errorout("fork");
  else if(pid == 0){         /* Child process*/
    execl(path, path, NULL);
    perror(path);            /* exec() family only returns on failure*/
    return EXIT_FAILURE;     /* Return non success to parent*/
  }
  else{                      /* Parent Process*/
    pid_t w;
    int wstatus;
    
    w = wait(&wstatus);      /* Store status info into wstatus*/

    while(w != pid){         /* Keep waiting till child process is closed or..*/
      w = wait(&wstatus);
      if(w == -1)            /* ...wait() returns an error*/
	errorout("wait");   
    }

    if(WIFEXITED(wstatus))
      if(!WEXITSTATUS(wstatus)) /* Returns exit status of child.*/
	printf("Process %ld succeeded.\n", (long)pid);
      else /* Child did not terminate normally*/
	printf("Process %ld exited with an error value.\n", (long)pid);
    else{
      fprintf(stderr, "Process %ld terminated abnormally.\n", (long)pid);
      errorout("WIFEXITED");
    }
  }
  return 0;
}

void errorout(char *error){
  perror(error);
  exit(EXIT_FAILURE);
}
