#include "options.h"

int getopt(int argc, char *const argv[], const char *optstring);
extern int optind;

unsigned int parse_options(int argc, char* argv[]){
  char opt;
  char *prog;
  unsigned int mode, argsleftmax, argsleft, vcount = 0;

  mode = 0;  
  prog = argv[0];
  if(argc < 2)
    usage(prog);
  while((opt = getopt(argc, argv,"vaN")) != -1){
    switch(opt){
    case 'v':         /* Verbose mode. Repeated verbosity treated as 1 opt*/
      vcount++;
      mode |= vMODE_;
      break;
    case 'a':         /* Accept all connections mode*/
      mode |= aMODE_;
      break;
    case 'N':         /* No ncurses mode*/
      mode |= NMODE_;
      break;
    default: /* '?' invalid option found*/
      usage(prog);
    }
  }
  
  
  /* Decrement vcount if it was found. This is for multiple verbosity*/
  (vcount > 1) ? vcount = 1: 0;
  argsleftmax = CLIENT_;
  argsleft = argc - optind;

  if(argsleft == CLIENT_){ /* If 2 arguments exist after opts*/
    mode |= CLIENTMODE_;       /* Client mode*/
    mode |= ((optind + 1) << HINIBBLE_); /* modes high nibble = Optind + 1*/
  }
  else if(argsleft > argsleftmax){ /* To many args after opts*/
    fprintf(stderr, "To many arguments for %s.\n", prog);
    usage(prog);
  }
  else if(argsleft < MINARGS_){ /* To few args after opts*/
    fprintf(stderr, "To few arguments for %s.\n", prog);
    usage(prog);
  }
  else
    mode |= (optind << HINIBBLE_); /* Store Optind in mode's high nibble*/
  
 if(mode & vMODE_)
    print_modes(mode);
  
  return mode;
}

void usage(char *argv){
  fprintf(stderr,
	  "Usage: %s [ -v ] [ -a ] [ -N ] [ hostname ] port\n",
	  argv);
  exit(EXIT_FAILURE);
}

/* For debugging and verbosity*/
void print_modes(int mode){
  printf("vMode: %d\n", (mode & vMODE_) >> vMODEPOS_);
  printf("aMode: %d\n", (mode & aMODE_) >> aMODEPOS_);
  printf("NMode: %d\n", (mode & NMODE_) >> NMODEPOS_);
  printf("CLIENTMODE: %d\n", (mode & CLIENTMODE_) >> CLIENTMODEPOS_);
  return;
}


unsigned int get_port(const char *portstr){
  /* Convert to long and check for validity*/
  /* 1 through 65535. However, some of the ports in the
   * range 1 through 1023 are used by system-supplied 
   * TCP/IP applications
   */
  long int portnum;
  char *endptr;
  
  /* If no number in portStr then it will return 0*/
  errno = 0;
  portnum = (unsigned int)strtol(portstr, &endptr, BASE_);

  if(errno){
    if(portnum == LONG_MIN) /* Underflow. Exit taken care of below.*/
      perror("strtol");
    else if(portnum == LONG_MAX) /* Overflow. Exit taken care of below.*/
      perror("strtol");
    else if(portnum == 0){/* No Number with error.*/
      perror("strtol");
      exit(EXIT_FAILURE);
    }
  }
  if(portnum == 0 && !strcmp(endptr, portstr)){
    fprintf(stderr, "No number found.\n");
    exit(EXIT_FAILURE);
  }
  
  /* Invalid portnumber or Extra text found after portnumber*/
  if(portnum < 1024 || portnum > 65635){
    fprintf(stderr,"Port numbers should be 1024 and 65635.\n");
    exit(EXIT_FAILURE);
  }
  
  return portnum;
}
