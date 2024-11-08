#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "modes.h"
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <pwd.h>
/* For local use*/
/*#include "talk.h"*/



#define MAXBUFF_ 1024
#define LISTEN_BACKLOG_ 50
#define LISTEN_VERBOSE_MSG_ "Searching for connection..."
#define CLIENT_CONNECT_V_MSG_ "Client connection from "
#define SERVER_CONNECT_V_MSG_ "Server connected to  "
#define ACCEPT_MSG_ "Connect"
#define REJECT_MSG_ "Reject"

/**
 *@fn server_mode
 *@brief This function will open a socket and and start listening for
          incomming communcations from a IPV4 client. If a request is found
	  then the server will need to make a choise if they wish to 
	  establish a connection with the requesting client. Unless the
	  aflag is set which will auto accept the connection. Vflag adds
	  verbosity during the setup and connection process.
 *@params int port, int vflag, int aflag
 *@return int
 **/
int server_mode(int port, int vflag, int aflag){
  int sockfd, acceptfd;
  int err;
  struct sockaddr_in saddr, caddr;
  socklen_t alen;
  ssize_t read;
  char serveraddr[INET_ADDRSTRLEN], clientaddr[INET_ADDRSTRLEN];
  char caddrname[NI_MAXHOST], cname[MAXBUFF_], buff[MAXBUFF_];
  
  /* Populate socket address info*/
  saddr.sin_family = AF_INET;     /* IPV4*/
  saddr.sin_port   = htons(port); /* User port*/
  saddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incomming address*/
  alen = sizeof(saddr); /* Get size of struct*/
  
  /* Open a socket with IPV4*/
  if(-1 == (sockfd = socket(saddr.sin_family, SOCK_STREAM, 0)))
    errorout(0, "Server socket()");
  
  /* Assign address to fdsock*/
  if(-1 == (bind(sockfd, (struct sockaddr *)&saddr, alen)))
    errorout(sockfd, "Server bind()");
  
  /* Set socket as passive to accept incomming comms*/
  if(-1 == listen(sockfd, LISTEN_BACKLOG_))
    errorout(sockfd, "Server listen");
  
  if(vflag)/* Print the listening message*/
    printf(LISTEN_VERBOSE_MSG_);

  if(-1 == (acceptfd = accept(sockfd, (struct sockaddr *)&caddr, &alen)))
    errorout(sockfd, "Server accept()");

  close(sockfd); /* No longer listening. Close the fd*/
  
  /* Convert IPV4 from bin to text*/
  if(NULL == (inet_ntop(AF_INET, &saddr, serveraddr, INET_ADDRSTRLEN)))
    errorout(acceptfd, "Server inet_ntop");
  if(NULL == (inet_ntop(AF_INET, &caddr, clientaddr, INET_ADDRSTRLEN)))
    errorout(acceptfd, "Server inet_ntop");
  
  /* More verbose client address connection*/
  if(vflag){
    printf(CLIENT_CONNECT_V_MSG_"%s", clientaddr);
    printf(SERVER_CONNECT_V_MSG_"%s", serveraddr);  
  }
  
  /* Get client name. Send client address & len. Don't care about server*/
  if((err = getnameinfo((struct sockaddr *) &caddr, alen,
		       caddrname,  sizeof(caddrname),
			NULL, 0 ,0)))
    errorout(acceptfd, gai_strerror(err));

  /* Client will first send its name*/
  if(-1 == (read = recv(acceptfd, cname, sizeof(cname) - 1, 0)))
    errorout(acceptfd, "Server recv");
  cname[read] = '\0';
  
  /* Requesting connection*/
  printf("My talk request from %s@%s. Accept (y/n)?", cname, caddrname);

  /* Will server accept*/
  if(!aflag){/* Auto Accept flag?*/
    /* Check for accept*/
    if(NULL == fgets(buff, MAXBUFF_, stdin))
      errorout(acceptfd, "Server fgets");
    if(tolower(buff[0]) == 'y') /* Yes|YES|yes|Y|y found. Good enough*/
      send(acceptfd, ACCEPT_MSG_, strlen(ACCEPT_MSG_), 0);/* Connect msg*/
    else{
      send(acceptfd, REJECT_MSG_, strlen(REJECT_MSG_), 0);/* Reject msg*/
      fprintf(stderr, "Rejecting connection. Terminating server.\n");
      if(errno)   /* Send broke*/
	errorout(acceptfd, "Server send reject.");
      else{ /* Close due to reject connection*/
	close(acceptfd); /* Skip error checking*/
	exit(EXIT_FAILURE);
      }
    }/* End Reject else*/
  }/* End !auto accept*/
  else /* Auto accept*/
    send(acceptfd, ACCEPT_MSG_, strlen(ACCEPT_MSG_), 0); /* Connect msg*/

  if(errno) /* If an error occured during sending*/
    errorout(acceptfd, "Server send accept");

  if(vflag)
    printf("Accepting connection...\n");
  
  return acceptfd;
}


int client_mode(char *sname, int port, int vflag){	
  int sockfd;
  int err;
  socklen_t alen;
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  struct sockaddr_in saddr;
  struct passwd *pwent;
  ssize_t read;
  
  /* Open a socket with IPV4*/
  if(-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
    errorout(0, "Client socket");

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET; /* IPV4 or IPV6 family*/
  hints.ai_socktype = SOCK_STREAM; /* TCP IP*/
  hints.ai_protocol = 0; /* Any*/
  
  if((err = getaddrinfo(sname, NULL, &hints, &result)))
    errorout(sockfd, gai_strerror(err));
  
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr =
    ((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr;
  alen = sizeof(saddr);

  /* Start connection attempt*/
  for(rp = result; rp != NULL; rp = rp->ai_next){
    
  if(-1 == (connect(sockfd, (struct sockaddr *)&saddr, alen)))
    errorout(sockfd, "Client connect");

  if(vflag)
    printf("Connecting to %u via port %u.\n",
	   saddr.sin_addr.s_addr, saddr.sin_port);
  
  if(NULL == (pwent = getpwuid(getuid()))) /* If something weird happened*/
    errorout(sockfd, "Client get user id");

  if(vflag)
    printf("pw_name: %s.\n", pwent->pw_name);
  
  /* Send Client name to server*/
  if(-1 == send(sockfd, pwent->pw_name, strlen(pwent->pw_name), 0))
    errorout(sockfd, "Client send");
  
  /* Check if the request is accepted*/
  printf("Wainting for a responce from %s.\n", sname);

  /* Client will first send its name*/
  if(-1 == (read = recv(sockfd, sname, sizeof(sname) - 1, 0)))
    errorout(sockfd, "Client recv");

  sname[read] = '\0';

  if(strcmp(sname, ACCEPT_MSG_)){/* If no match then request was rejected*/
    fprintf(stderr, "Request rejected.");
    close(sockfd);
  }
  
  freeaddrinfo(result); /* Done with result*/
  return sockfd;
}

void errorout(int fd, const char *error){
  close(fd);
  perror(error);
  exit(EXIT_FAILURE);
}
