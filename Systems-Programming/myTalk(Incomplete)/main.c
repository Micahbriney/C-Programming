#include "options.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "modes.h"
#include "mytalk.h"

int main(int argc, char *argv[]){
  unsigned int modes, argindex;
  unsigned int aflag, vflag, Nflag, clientmode;
  unsigned int portnum;
  char *hostname;
  int fd;
  
  aflag = vflag = Nflag = clientmode = 0;
  modes = parse_options(argc, argv);

  aflag = modes & aMODE_;
  vflag = modes & vMODE_;
  Nflag = modes & NMODE_;
  clientmode = modes & CLIENTMODE_;
  argindex = modes >> HINIBBLE_;
  portnum = get_port(argv[argindex]);

  if(vflag)
    printf("Portnumber network order: %d\n", portnum);
    
  /* Load into client or server mode*/
  if(clientmode){
    hostname = argv[argindex++];
    if(vflag)
      printf("Hostname: %s.\n", hostname);
    fd = client_mode(hostname, portnum, vflag);
  }
  else
    fd = server_mode(portnum, vflag, aflag);

  mytalk(fd, vflag, aflag, Nflag);
    

  return 0;
}
