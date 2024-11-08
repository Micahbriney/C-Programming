/**
 ***************************************************************************
 * @author	: Micah Briney
 * @date	: 11/14/2022
 * @version     : 2
 * @file	: main.c
 * @brief	: This program utlizes reliable signal handling to imitate
                   a clock ticking and tocking. The ticking and tocking 
		   happens at alternating half second intervals. There 
		   SIGALRM is used to interrupt the program and execute the
		   print "Tick..." or "Tock\n" to the screen. The time is
		   decided by user input. When the program is done it will
		   print "Time's op!\n".
 * @target	: 
 * @compiler: gcc
 ***************************************************************************
 */


#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#define BASE_ 10
#define USAGE_FAILURE_ -1
#define DIVISIONS_ 2
#define USECINTERVAL_ (1000000 / DIVISIONS_)

void usage(char *prog);
long int get_time(char *time);
void signal_init(struct sigaction *sa);
void timer_init(struct itimerval *itv);
void IRQ_handler(int sig);

/**
 *@main
 *@brief The application entry point.
 *@parms int, char *
 *@return int
**/
int main(int argc, char *argv[]){
  long int sec;
  long int i,totalsignals;
  int j;
  char *prog;
  struct sigaction sa;
  struct itimerval itv;
  sigset_t set;
  
  prog = argv[0];
  if(argc < 2)    /* To many arguments found*/
    usage(prog);
  
  /* Check to see if there were any usage issues*/
  if(-1 == (sec = get_time(argv[1]))) /* Parse time argument*/
    usage(prog);
  
  signal_init(&sa); /* Initialize signal action struct*/
  timer_init(&itv); /* Initilized timer struct. Block SIGARLM*/

  /* Good luck testing LOL.*/
  if(sec > (LONG_MAX / 2))  /* Dont want to overflow. Weird edge case*/
    for(j = 0; j < 2; j++)  /* Run twice instead of multiplying by two*/
      for(i = 0; i < sec; i++) /* Loop then wait*/
	sigsuspend(&set);   /* Wait for the signal interrupt*/
  
  
  totalsignals = sec * DIVISIONS_;   /* Total number of Ticks + Tocks*/
  for(i = 0; i < totalsignals; i++) /* Loop then wait*/
    sigsuspend(&set); /* Wait for the signal interrupt*/
  

  /* Stop blocking SIGALRM*/
  sigemptyset(&set);                    /* Clear/resuse set*/
  sigaddset(&set, SIGALRM);             /* Set SIGALRM*/
  sigprocmask(SIG_UNBLOCK, &set, NULL); /* Unblock SIGALRM*/
  
  
  printf("Time's up!\n"); /* Final output*/
  return 0;
}


/**
 *@fn    signal_init
 *@brief This function will initilize the signal action struct. It will set 
          the signnal handler, modication flags, action mask and set which
	  signal will trigger the handler. This function will also block out
	  signals from SIGALRM before there is a chance to call sigsupend.
 *@params struct sigaction *sa
 *@return none
**/
void signal_init(struct sigaction *sa){
  sigset_t block;
   
  sa->sa_handler = IRQ_handler;  /* Specify action associated with signal*/
  sa->sa_flags = 0;              /* No behavior modifications*/
  if(sigemptyset(&sa->sa_mask)){ /* Clear out interrupt signal*/
    perror("sigemptyset");
    exit(EXIT_FAILURE);
  }
  
  if(sigaction(SIGALRM, sa, NULL)){/* Set action for SIGALRM.*/
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  /* Prevent race conditions by blocking sigalarm*/
  sigemptyset(&block);                  /* Clear sigset_block*/
  sigaddset(&block, SIGALRM);           /* Add SIGALRM to the signal*/
  sigprocmask(SIG_BLOCK, &block, NULL); /* Block SIGARLM*/
  
  return;
}


/**
 *@fn    timer_init
 *@brief This function will setup the interval and timer value for the
          itimerval sturct. The setitimer call will arm the ITIMER_REAL
	  timer. 
 *@params struct itimerval *itv
 *@return none
**/
void timer_init(struct itimerval *itv){
  itv->it_interval.tv_sec = 0;              /* Seconds. Not used*/
  itv->it_value.tv_sec = 0;                 /* Seconds. Not used*/
  itv->it_interval.tv_usec = USECINTERVAL_; /* Reload value*/
  itv->it_value.tv_usec = USECINTERVAL_;    /* Current timer value*/
  
  if(-1 == setitimer(ITIMER_REAL, itv, NULL)){ /* Set REAL timer*/
    perror("setitimer"); /* It's dead Jim.*/
    exit(EXIT_FAILURE);
  }
  return;
}


/**
 *@fn get_time
 *@brief Parse the command line time option. This will check for valid time
          inputs. If the time is negative or causes an underflow or 
          overflow then this function will print the error and return -1.
	  Otherwise the function will return an integer value.
**/
long int get_time(char *time){
  char *endptr;
  long int res;

  errno = 0; /* Clear for strol error checking*/
  res = strtol(time, &endptr, BASE_); /* Convert string to long*/
  /* Check integer value for valid time*/
  if(errno){
    if(res == LONG_MIN){ /* Underflow*/
      perror("strtol");
      return res = USAGE_FAILURE_;
    }
    else if(res == LONG_MAX){ /* Overflow*/
      perror("strtol");
      return res = USAGE_FAILURE_;
    }
    else if(res == 0){ /* No number found. Strtol "may also set errno..."*/
      perror("strtol");
      return res = USAGE_FAILURE_;
    }
  }
  else if(res == 0 && !strcmp(endptr, time)){ /* No number found*/
    fprintf(stderr, "No number found in \"%s\".\n", time);
    return res = USAGE_FAILURE_;
  }
  else if(res < 0){ /* Negative number*/
    fprintf(stderr, "Can not count negative seconds (%ld).\n", res);
    return res = USAGE_FAILURE_;
  }
  return res;
}


/**
 *@fn    IRQ_handler
 *@brief The interrupt handler will toggle the ticks and tocks of the 
          signal interrupt.
 *@params int
 *@return none
**/
void IRQ_handler(int sig){
  static int num = 0;
  if(num ^= 1) /* Toggle bit*/
    printf("Tick...");
  else
    printf("Tock\n");
  fflush(stdout);
  return;
}


/**
 *@fn    usage
 *@brief The function handles issues that arrise from usage problems. 
          It will print a usage message to stderr and exit the program.
 *@params char *
 *@return none
**/
void usage(char *prog){
  fprintf(stderr, "Usage: %s <seconds>\n", prog);
  exit(EXIT_FAILURE);
}

