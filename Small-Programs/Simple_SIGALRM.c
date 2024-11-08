/*Program: Simple_SIGALRM
 * This program will catch signal alarm.
 * Signals are sent by alarm() with a time of 1 second.
 * This program will catch each SIGALRM 5 times
 * This program is a preface to better signal timer alarm handliing 
 * This is not a good approach to create a time driven program.
 * The alarm() only has a granlarity of one second intervals and
 * the program is subject to time drift as it uses best effort signal 
 * catching. This means that the signal will generate every second but
 * the signal might have to wait on another process before the handler
 * is call for the SIGALRM.
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

  /* Alarm only has one second granularity */
  alarm(1); /* Send alarm signal. 1 = 1 Second.*/
  
  if(-1 == sigaction(SIGALRM, &sa, NULL)){
    perror("sigaction");
    exit(EXIT_FAILURE);
  }
  
  for(i = 0; i < 5; i++){
    pause();
    printf("That's %d\n", i);
    
  }
  
  return 0;
}


void handler(int num){
  alarm(1);/* Request another alarm in 1 second*/
  printf("Caught a signal(%d)\n", num);
}
