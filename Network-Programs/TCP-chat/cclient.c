/******************************************************************************
* myClient.c changed to cclient.c
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

#include "pollLib.h"
#include "networks.h"
#include "safeUtil.h"
#include "transport.h"

/* Define macros*/
#define DEBUG_FLAG            1
#define ARGV_HANDLE_POS       1
#define ARGV_SERVER_NAME_POS  2
#define ARGV_SERVER_PORT_POS  3
#define ARGV_VERBOSE_POS      4
#define COMMAND_OP_SYMBOL_POS 0
#define COMMAND_POS           1

/* Global verbose flag*/
int verbosity = 0;
int clientGlobalSocket = 0;

/* Function prototypes*/
void clientControl(int clientSocket, char *handleName);
void initConnection(int clientSocket, char *handleName);
void processEvent(int socketNum);
void recvFromServer(int serverSocket);
void recvHandler(uint8_t *dataBuffer, int socket);
void flagCheck();
void sendToServer(int socketNum);
int readFromStdin(uint8_t * buffer);
void PDUHandler(int socketNum, uint8_t *buffer, uint8_t flag);
uint8_t commandCheck(uint8_t *buffer, int bufferLen);
uint8_t getFlag(char command);
void checkArgs(int argc, char * argv[]);


int main(int argc, char * argv[]){
  int   clientSocket = 0;         //socket descriptor
  char *handleName;
  char *serverName;
  char *serverPort;

  checkArgs(argc, argv);

  handleName = argv[ARGV_HANDLE_POS];
  serverName = argv[ARGV_SERVER_NAME_POS];
  serverPort = argv[ARGV_SERVER_PORT_POS];
  
  /* set up the TCP Client socket  */
  clientGlobalSocket = clientSocket = tcpClientSetup(serverName,
						     serverPort,
						     verbosity);

  clientControl(clientSocket, handleName);
  	
  close(clientSocket);
	
  return 0;
}

void clientControl(int clientSocket, char *handleName){
  int pollValue = 0;
  int flag = 0;

  initConnection(clientSocket, handleName);
  
  printf("$: "); /* Initial prompt*/
  fflush(stdout);
  /* Check for flag*/
  while(flag != S_EXIT_ACK_FLAG){
    if(-1 == (pollValue = pollCall(POLL_WAIT_FOREVER))) /* For gdb testing*/
  //  if(-1 == (pollValue = pollCall(POLL_NO_BLOCK))) /* Poll and return immeditely*/
      continue;/* Nothing occured*/
    else{
      processEvent(pollValue);
      flag = getChatHeaderFlag();
    }
    
  }
}

void initConnection(int clientSocket, char *handleName){
  pduPtr initPDU;
  uint8_t handleLen;
  uint8_t sendBuf[MAXBUF];
  
  handleLen = getHandleLen(handleName);
  /* Initialize poll sets*/
  setupPollSet();
  addToPollSet(STDIN_FILENO);
  addToPollSet(clientSocket);
  /* Initialize Chat header*/
  initChatHeader();
  buildChatHeader(handleName, C_INIT_FLAG, 0); /* Initial server contact pdu*/ 
  /* Initialize handle*/
  setupHandleSet();
  addHandleToSet(clientSocket, handleLen, handleName);

  /* Build PDU*/
  initPDU = (pduPtr)sCalloc(HANDLE_MAX_LEN, sizeof(uint8_t));
  initPDU->header.PDULen = htons(CHAT_HEADER_BYTE_LEN + HANDLE_BYTE_LEN + handleLen);
  initPDU->header.flag = getChatHeaderFlag();
  initPDU->handleLen = handleLen;
  //memcpy(&initPDU[HANDLE_POS], handleName, handleLen);
  memcpy(initPDU->data, handleName, handleLen);
  memcpy(sendBuf, initPDU, MAXBUF);
  
  if(verbosity)
    printf("Init Chat header values. PDU Length: %d. Flag: %d\n",
	   ntohs(getChatHeaderPDULength()),
	   getChatHeaderFlag());
  
  /* Send Init connection flag */
  if(sendPDU(clientSocket, sendBuf, ntohs(getChatHeaderPDULength())) < 0)
    errorOut("Init send call");
  
  recvFromServer(clientSocket); /* Wait for flag 2 or 3*/
  return;
}

void processEvent(int socketNum){
  if(socketNum == 0){ /* Stdin event*/
    sendToServer(socketNum);
    printf("$: ");
    fflush(stdout);
  }
  else{
    recvFromServer(socketNum);
    //printf("$: ");
    }
}

void recvFromServer(int serverSocket){
  uint8_t dataBuffer[MAXBUF];
  int messageLen = 0;

  /* If I want to keep the flag check here*/
  if((messageLen = recvPDU(serverSocket, dataBuffer, MAXBUF)) < 0)
    errorOut("recv call");
  
  if(messageLen > 0)
    recvHandler(dataBuffer, serverSocket);
  else{
    if(verbosity) /* Turn back on when done testing*/
      printf("Message recieved on socket: %d, length: %d, Data: %s\n",
	     serverSocket, messageLen, dataBuffer);
  }
}
  
void recvHandler(uint8_t *dataBuffer, int socket){
  uint8_t  flag;
  //uint16_t PDULen;
  uint32_t handleCnt;
  char    *handle;
  //PDULen = dataBuffer[PDU_LEN_POS];
  flag   = dataBuffer[FLAG_POS];

  /* Server flags 2, 3, 7, 9, 11, 12, 13 */
  switch(flag){
  case 2:  /* Server to Client ACK init connection*/
    if(verbosity)
      printf("Server connection acknowledged. Begin communication.");
    printf("$: ");
    break;
  case 3:  /* Server to Client Error init connection*/
    printf("Handle already in use: %s\n", getHandleName(socket));
    exit(EXIT_SUCCESS);
  case 7:  /* Missing Handle*/
    handle = (char *)sCalloc(dataBuffer[MISSING_HANDLE_LEN_POS], sizeof(char));
    memcpy(handle,
	   dataBuffer + MISSING_HANDLE_POS,
	   dataBuffer[MISSING_HANDLE_LEN_POS]);
    printf("Client with handle %s does not exist.\n", handle);
    free(handle);
    printf("$: ");
    fflush(stdout);
    break;
  case 9:  /* Exit Ack*/
    if(verbosity)
      printf("Exit acknowledged. Shutting down.\n");
    break;
  case 11: /* List Count return*/
    memcpy(&handleCnt, dataBuffer + HANDLE_COUNT_LEN, HANDLE_COUNT_LEN);
    printf("Number of clients: %d", ntohl(handleCnt));
    break;
  case 12: /* Send list of handles one at time*/
    printf("\t%s", getHandleName(socket)); /* Get handle and print them*/
    break;
  case 13: /* End list return*/
    if(verbosity)
      printf("End of list flag encountered.\n");
    printf("$: ");
    break;
  default:
    break;
  }
  return;
  
}

void flagCheck(int flag){


}

void sendToServer(int socketNum){
  uint8_t sendBuf[MAXBUF];//data buffer
  int sendLen = 0;      //amount of data to send
  int sent = 0;         //actual amount of data sent/* get the data and send it   */
  uint8_t flag= 0;

  sendLen = readFromStdin(sendBuf);
  
  /* Check for valid commands*/
  if((flag = commandCheck(sendBuf, sendLen)) == 0)
    return;
  
  /* Build PDU*/   
  PDUHandler(socketNum, sendBuf, flag);
  
  if(verbosity)
    printf("read: %s string len: %d (including null)\n", sendBuf, sendLen);
  sent = sendPDU(socketNum, sendBuf, sendLen); /* Convert readFromStdin to a PDU*/

  if (sent < 0)
    {
      perror("send call");
      exit(-1);
    }

  if(verbosity)
    printf("Amount of data sent is: %d\n", sent);
}

int readFromStdin(uint8_t * buffer){
  char aChar    = 0;
  int  inputLen = 0;        
	
  // Important you don't input more characters than you have space 
  buffer[0] = '\0';
  /*printf("Enter data: ");*/
  while (inputLen < (MAXBUF - 1) && aChar != '\n')
    {
      aChar = getchar();
      if (aChar != '\n')
	{
	  buffer[inputLen] = aChar;
	  inputLen++;
	}
    }
	
  // Null terminate the string
  buffer[inputLen] = '\0';
  inputLen++;
	
  return inputLen;
}

void PDUHandler(int socketNum, uint8_t *buffer, uint8_t flag){
  buildChatHeader(getHandleName(clientGlobalSocket), flag, buffer);
}

/* Checks for valid command and format. Returns the flag*/
uint8_t commandCheck(uint8_t *buffer, int bufferLen){
  char command;

  if(buffer[COMMAND_OP_SYMBOL_POS] == '0' ||
     buffer[COMMAND_POS] == '0') /* Somehow a NULL by got in here*/
    errorOut("CommandCheck somehow got a Null byte from readFromStdin()");

  if(buffer[COMMAND_OP_SYMBOL_POS] != '%'){
    printf("Invalid command format\n");
    return 0;
  }

  command = tolower(buffer[COMMAND_POS]);
  if(verbosity)
    printf("The command op symbol %% was found and the command is: %c", command);

  return getFlag(command);
}

/* Find the right flag*/
uint8_t getFlag(char command){
  switch(command){
  case 'm':
    return UNICAST_FLAG;
  case 'b':
    return BCAST_FLAG;
  case 'c':
    return MULTICAST_FLAG;
  case 'l':
    return C_LIST_FLAG;
  case 'e':
    return C_EXIT_FLAG;
  default:
    printf("Invalid command\n");
    return 0;
  }
  return 0; /* No sure how we got here*/
}


void checkArgs(int argc, char * argv[]){
  /* check command line arguments  */
  /* Add check for handle*/
  char *handleName = argv[ARGV_HANDLE_POS];
  
  checkHandle(handleName);  /* Check handle syntax*/
  getHandleLen(handleName); /* Check handle length*/
  
  if(argc == 5){ /* Check for verbosity argument*/
    if((argv[ARGV_VERBOSE_POS][1] && argv[ARGV_VERBOSE_POS][0]) &&
       tolower(argv[ARGV_VERBOSE_POS][1]) == 'v' &&
       argv[ARGV_VERBOSE_POS][0] == '-'){
      verbosity = 1; /* Set global verbosity flag*/
      return;
    }
  }
  
  if (argc != 4)
    {
      printf("usage: %s handle host-name port-number [-v] \n", argv[0]);
      exit(1);
    }
}
