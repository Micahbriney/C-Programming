/******************************************************************************
* myServer.c changed to server.c
* 
* Writen by Prof. Smith, updated Jan 2023
* Use at your own risk.  
*
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdint.h>
#include <ctype.h>

#include "networks.h"
/* Start adding new code below*/
#include "transport.h"
#include "pollLib.h"
/* End adding new code*/

#include "safeUtil.h"

/* Start commenting out code*/
//#define MAXBUF 1024
/* End commenting out code*/
#define DEBUG_FLAG 1

int verbosity = 0; /* Global verbose flag*/

void serverControl(int mainServerSocket);
void initServer(int mainServerSocket);
void addNewClient(int mainServerSocket);
void processClient(int clientSocket);
void recvFromClient(int clientSocket);
void recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);

int main(int argc, char *argv[]){
  int mainServerSocket = 0; /* socket descriptor for the server socket*/
  int portNumber       = 0;
	
  portNumber = checkArgs(argc, argv);
	
  /* create the server socket */
  mainServerSocket = tcpServerSetup(portNumber);

  serverControl(mainServerSocket);

  // wait for client to connect
  //clientSocket = tcpAccept(mainServerSocket, DEBUG_FLAG);
    
  //recvFromClient(clientSocket);
    
  /* close the sockets */
  //close(clientSocket);
  
  close(mainServerSocket);

	
  return 0;
}

/* Start new code here*/
void serverControl(int mainServerSocket){
  int pollValue = 0;

  /* Run init functions*/
  initServer(mainServerSocket);
  
  while(1){
    //    if(mainServerSocket == (pollValue = pollCall(POLL_NO_BLOCK)))
    /* For gdb testing*/
    if(mainServerSocket == (pollValue = pollCall(POLL_WAIT_FOREVER)))
      addNewClient(mainServerSocket); /* New socket found*/
    else if(pollValue == -1)          /* Time out occured*/
      continue;
    else
      processClient(pollValue);       /* Client sent PDU*/
  }   
}

void initServer(int mainServerSocket){
  /* Initilize the poll set*/
  setupPollSet();                 /* Init polling sets*/
  addToPollSet(mainServerSocket); /* Add the main server socket to the set*/

  /* Initilize the handle set*/
  setupHandleSet();
  return;
}

void addNewClient(int mainServerSocket){
  int clientSocket = 0;
  //  clientSocket = tcpAccept(mainServerSocket, DEBUG_FLAG);
  clientSocket = tcpAccept(mainServerSocket, verbosity);
  addToPollSet(clientSocket);
}

void processClient(int clientSocket){
  recvFromClient(clientSocket);
}

void recvFromClient(int clientSocket){
  uint8_t dataBuffer[MAXBUF] = {0};
  int messageLen = 0;
  
  /* now get the data from the client_socket*/
  if((messageLen = recvPDU(clientSocket, dataBuffer, MAXBUF)) < 0)
    errorOut("recv call");
  
  
  /* Changes from provided code include: removal of {}'s and slight message change*/
  if (messageLen > 0 && verbosity)
    printf("Message received on socket %d, length: %d Data: %s\n",
	   clientSocket, messageLen, dataBuffer);
  else{
    if(verbosity)
      printf("Connection closed by other side\n");
    removeFromPollSet(clientSocket);
  }
}

int checkArgs(int argc, char *argv[]){
  // Checks args and returns port number
  int portNumber = 0;
  //extern int verbosity;

  if(argc == 2){
    if(tolower(argv[1][1]) == 'v' && argv[1][0] == '-')
      verbosity = 1; /* Set global verbosity flag*/
    else
      portNumber = atoi(argv[1]);
  }
  
  if(argc > 2){
    if((argv[2][1] && argv[2][0]) && 
       (tolower(argv[2][1]) == 'v' && argv[2][0] == '-')){/* 1st Check NULL bytes*/
      verbosity = 1;
      portNumber = atoi(argv[1]);
    }
    else{
      fprintf(stderr, "Usage %s [optional port number] [-v]\n", argv[0]);
      exit(-1);
    }
  }
  
  // if (argc == 2 && !verbosity)
  //portNumber = atoi(argv[1]);
  
  return portNumber;
}
