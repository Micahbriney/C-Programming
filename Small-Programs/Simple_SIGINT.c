/*Program: Simple_SIGINT
 * This program will catch signal interrupt via stdin.
 * Signals include: Ctrl+C (^C)
 * This program will catch each SIGINT 5 times
 * This program will all you to control how your program is killed
 * with ^C. So if your program changes the terminal then you can
 * execute code to return the terminal back to normal.
 **/

#include <sys/time.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int num);

int main(int argc, char* argv[]){
  /* Sturct sigaction required for handler functions
  *  this allows for a reliable use of the unreliable handler function
  *  sa_sigaction is also avaible but is not used in this program.
  **/
  struct sigaction sa; /*Sigaction struct to hold handler func*/

  /* What do we want sigaction to do?
  *  We want it to use the handler function*/
  sa.sa_handler = handler; /* Set sa_handler points at handler funct*/
  /* What signals do we want to block?
   * We want to block no signals for this program 
   * Sig empty set takes the address of a sigset
   **/
  sigemptyset(&sa.sa_mask); /* Removes all signals from set sa_mask*/
  /* What actions do you want to take
   * Setting flags to 0 makes it do the default action.
   * 99 out of 100 times the typical setting is 0.
   **/
  sa.sa_flags = 0; /* No flags set*/
  int i;

  /* Catch SIGINT, pass it the address of sa, oldaction is ignored*/
  if(-1 == sigaction(SIGINT, &sa, NULL)){ /* Sigaction fails*/
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  
  for(i = 0; i < 5; i++){
    pause();                  /* Wait here for a signal*/
    printf("That's %d\n", i); /* Print after signal handler is called*/
    
  }
  
  return 0;
}

/* Signal handler function that was added to struct sigaction sa.handler*/
void handler(int num){ /* num is the SIGXXX number. Here it is ^C (14)*/
  printf("Caught a signal(%d)\n", num);
}/* When handler is done. pause function completed */
