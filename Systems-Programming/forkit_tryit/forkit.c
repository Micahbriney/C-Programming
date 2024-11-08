/**
 ***************************************************************************
 * @author	: Micah Briney
 * @date	: 11/14/2022
 * @version     : 2
 * @file	: forkit.c
 * @brief	: This program utlizes fork() and waitpid()to demonstrate 
              that the order of execution of parent and child in 
			  nondeterministic. This forkit program starts up, announces 
			  itself, then splits into two processes. The child announces 
			  itself and exits. The parent identifies itself, waits for the 
			  child, then signs off.
		
		   
 * @compiler: gcc
 ***************************************************************************
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define WAIT_FOR_ANY_CHILD_ -1

int main(int argc, char *argv[]){
  pid_t pid;
  int wstatus;
  printf("Hello, world!\n");
  if((pid = fork()) < 0){ /*  on success.*/
    perror("fork error");
  }
  else if(pid == 0) /* Child process. Pid will be hold parent pid*/
    printf("This is the child, pid %ld.\n", (long)getpid()); /* Child code*/
  else{ /* Parent process*/
    pid_t parentpid;
    int w;
    parentpid = getpid();
    printf("This is the parent, pid %ld.\n", (long)parentpid); /* Parent codes*/

    w = waitpid(WAIT_FOR_ANY_CHILD_, &wstatus, 0);
    while(w != pid)    /* Wait for child process to end*/
      w = waitpid(WAIT_FOR_ANY_CHILD_, &wstatus, 0);
    printf("This is the parent. pid %ld, signing off.\n", (long)parentpid);
  }

  return 0;
}
