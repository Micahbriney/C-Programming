/*
 *  ******************************************************************************
 * @author	: Micah Briney
 * @date	: 2/10/2023
 * @version : 1
 * @file	: transport.c
 * @brief	: Transport file handles the transport layer for communication in the 
                chat system. It includes functions for sending and receiving 
                protocol data units (PDUs), handling various message flags, and 
                managing client connections.

 ******************************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "transport.h"
#include "clienthandle.h"
#include "safeUtil.h"

enum Flags {
    INIT = 1, ACK, INITERR, BROADCAST, UNICAST, MULTICAST, 
    MISSING_HANDLE, EXIT, EXIT_ACK, C_LIST, LIST_COUNT, SEND_LIST, EXIT_LIST
};

/* Global variable*/
static chatHeaderPtr clientChatHeader;

int sendPDU(int clientSocket, uint8_t *dataBuffer, int lengthOfData){
  chatHdr chatHdr;
  pduPtr pduBuf;
  int pduLen, sent;

  if(dataBuffer[FLAG_POS - 1] == C_INIT_FLAG)
    return sent = safeSend(clientSocket, dataBuffer, lengthOfData, MSG_CONFIRM);

  /* Init doesn't use the following*/
  if(NULL == (pduBuf = (pduPtr)(malloc(sizeof(pduPayload)))))
    errorOut("Malloc pduBuf sendPDU");

  pduLen = lengthOfData + CHAT_HEADER_BYTE_LEN;

  /* Check is buffer & Data field will be larger than MAXBUF plus null*/
  if(pduLen > MAXBUF + 1) 
    errorOut("Data buffer is to large"); 
  else{ /* Dont use for init*/
    buildPDU(chatHdr, dataBuffer, lengthOfData, pduBuf);
    sent = safeSend(clientSocket, (uint8_t *)pduBuf, pduLen, MSG_CONFIRM);
  }
  
  free(pduBuf);
  return sent;
}

//void buildPDU(uint8_t *dataBuffer, int lengthOfData, pduPtr pduBuf){
void buildPDU(chatHdr chatHdr, uint8_t *dataBuffer, int handleLen, pduPtr pduBuf){
//  pduBuf->dataLen = htons((uint16_t)lengthOfData);
  pduBuf->header = chatHdr;
  pduBuf->handleLen = handleLen;
  memcpy(pduBuf->data, dataBuffer, handleLen); /* Put data after length*/
  pduBuf->data[handleLen] = 0x00;                  /* Null Terminate*/
  return;
}

/* Can probably drop bufsize and replace the check with a macro.*/
int recvPDU(int scktNum, uint8_t *dataBuf, int bufSize){
  int msgLen = 0;
  pduPtr pduBuf;

  if(NULL == (pduBuf = (pduPtr)(sCalloc(MAXBUF, sizeof(uint8_t))))) /* Get memory*/
    errorOut("pduBuf recvPDU"); /* Something went wrong*/

  /* Get data from client socket*/
  if((msgLen = safeRecv(scktNum, dataBuf, CHAT_HEADER_BYTE_LEN, MSG_WAITALL)) < 0)
    errorOut("recv call data field"); /* Something went wrong*/
  
  /* Need to get the pduLength and that is the memcpy size*/
  memcpy(pduBuf, dataBuf, CHAT_HEADER_BYTE_LEN); /* Put C_hdr bytes in pduBuf struct*/
  pduBuf->header.PDULen = ntohs(pduBuf->header.PDULen); /* Change to host byte order*/

  if(pduBuf->header.PDULen > bufSize)
    errorOut("Message larger than buffer");

  /* Clear dataBuf. Might not need it but better to be safe*/
  //memset(dataBuf, 0, bufSize);
  if(pduBuf->header.PDULen > 3) /* Check PDULen if there is more data in PDU */
    if((msgLen = safeRecv(scktNum,
			  dataBuf,
			  (int)pduBuf->header.PDULen - CHAT_HEADER_BYTE_LEN,
			  MSG_WAITALL)) < 0)
      errorOut("recv call data");
  /* Copy the rest of the handle length, handle and pdu into the PDU struct*/
  pduBuf->handleLen = dataBuf[0];
  memcpy(pduBuf->data, dataBuf + HANDLE_BYTE_LEN, msgLen - HANDLE_BYTE_LEN);
  /* Check flags? of after return?*/
  flagHandler(scktNum, pduBuf);
  
  free(pduBuf);
  
  return msgLen;
}

void flagHandler(int handleSocket, pduPtr pduBuf){
  uint8_t  flag   = pduBuf->header.flag;
  uint16_t PDULen = pduBuf->header.PDULen;
  uint8_t handleLen;
  char handle[HANDLE_MAX_LEN] = {0};
  
  if(PDULen > CHAT_HEADER_BYTE_LEN && flag != S_LIST_CNT_FLAG){
    handleLen = pduBuf->handleLen;
    memcpy(handle, pduBuf->data, handleLen);
  }
  
  switch(flag){
  case INIT:  /* Client to Server Init connnection*/
      handleFlag1(handleSocket, handleLen, handle);
      break;
  case ACK:  /* Server to Client ACK init connection*/
      if (verbosity)
          printf("Server ACK with flag: %d & %s was added to handleSet\n", flag, handle);
      break;
  case INITERR:  /* Server to Client Error init connection*/
      printf("Handle already in use: %s", handle);
      exit(EXIT_FAILURE);
      return;
  case BROADCAST:  /* Broadcast adds message */
    //PDULen += 
      break;
  case UNICAST:  /* Unicast*/
      break;
  case MULTICAST:  /* Multicast*/
      break;
  case MISSING_HANDLE:  /* Missing Handle*/
      break;
  case EXIT:  /* Client Exit*/
  case EXIT_ACK:  /* Exit Ack*/
  case C_LIST: /* Client List Request*/
      break;
  case LIST_COUNT: /* List Count return*/
      break;
  case SEND_LIST: /* Send list of handles one at time*/
      break;
  case EXIT_LIST: /* End list return*/
  default:
    break;
  }

  return;
}


void handleFlag1(int handleSocket, uint8_t handleLen, char *handle){
  chatHdr chatHeaderOnly;
  int sent;
  uint8_t PDUBuf[CHAT_HEADER_BYTE_LEN];
  /* Prepare flag 2 or 3*/
  chatHeaderOnly.PDULen = htons(CHAT_HEADER_BYTE_LEN);

  /* 0 Means the handle already exists*/
  if(0 == addHandleToSet(handleSocket, handleLen, handle))
    chatHeaderOnly.flag = S_ACK_ERR_FLAG;    /* Set flag 3 for cclient*/
  else
    chatHeaderOnly.flag = S_ACK_FLAG;        /* Set flag 2 for cclient*/
  memcpy(PDUBuf, &chatHeaderOnly, CHAT_HEADER_BYTE_LEN);
  sent = safeSend(handleSocket,
		  PDUBuf,
		  CHAT_HEADER_BYTE_LEN,
		  MSG_CONFIRM);

  /* TODO figureout what to put here*/
  if(sent == 0)
    errorOut("handleFla1() send zero bytes");
  
  return;
}

void initChatHeader(){
  clientChatHeader = (chatHeaderPtr)sCalloc(CHAT_HEADER_BYTE_LEN, sizeof(chatHdr)); 
}

/* Set up Chat Header length and flag*/
void buildChatHeader(char *handleName, uint8_t flag, uint8_t *buffer){
  
  /* TODO CONTINUED. Things to consider:
      unicast flag = 5. the command is %M. Format %M <handlename> [message]*/    
  switch(flag){
  case 1:  /* Client to Server Init connnection*/
    setChatHeaderPDULength(handleName, flag);
    setChatHeaderFlag(flag);
    break;
  case 2:  /* Server to Client ACK init connection*/
  case 3:  /* Server to Client Error init connection*/
    return;
  case 4:  /* Broadcast adds message */
    /* Start building here. Dont need a length check as the buffer cant go that long
        need to check if the length of the buffer is > 200 for mutliple sends though
        This should just build the header.*/
    break;
  case 5:  /* Unicast*/
    unicastHandler(handleName, buffer, flag);
    break;
  case 6:  /* Multicast*/
    break;
  case 7:  /* Missing Handle*/
    break;
  case 8:  /* Client Exit*/
  case 9:  /* Exit Ack*/
  case 10: /* Client List Request*/
    break;
  case 11: /* List Count return*/
    break;
  case 12: /* Send list of handles one at time*/
    break;
  case 13: /* End list return*/
    break;
  default:
    break;
    
  }
  //  setChatHeaderLength(PDULen);
  return;
}

/* Building to handle both unicast and multicast*/
void unicastHandler(char *handleName, uint8_t *buffer, uint8_t flag){
  chatHdr    chatHeader;
  pduPayload payload;
  //uint16_t   PDULen;
  uint8_t    numRecipients;
  //  uint8_t   *msgPtr;
  //uint8_t   *message;
  dHandle    destHandle[HANDLE_MAX_LEN];

  tokenizeDestHandles(&numRecipients, destHandle, buffer, flag);

  memset(&chatHeader, 0, sizeof(chatHeader)); /* Initilize chatHeader's fields to 0*/
  payload.header = chatHeader;

    /* Note get client handle from *getHandleName(int socketNumber)*/
  /* Set flag*/
  chatHeader.flag   = flag;
  /* Set Client Handle Len*/
  payload.handleLen = getHandleLen(handleName); /* Get from client global handleSet*/
  /* Set Client/Source Handle*/
  memcpy(payload.data, handleName, payload.handleLen); 
  /* Set number of recipients*/
  payload.data[payload.handleLen] = numRecipients;
  /* Set destination handle(s) and length(s)*/
  memcpy(payload.data + payload.handleLen + HANDLE_BYTE_LEN,
	 destHandle,
	 sizeof(dHandle));
  calculateChatHeaderPDULength(payload);
  if(verbosity){

  }

  /* Put payload into buffer*/
  memcpy(buffer, &payload, sizeof(payload));
  //memcpy(buffer, payload.header, CHAT_HEADER_BYTE_LEN * sizeof(uint8_t));
  //memcpy(buffer + CHAT_HEADER_BYTE_LEN, payload.handleLen, HANDLE_BYTE_LEN);
  //memcpy(buffer + CHAT_HEADER_BYTE_LEN + HANDLE_BYTE_LEN,
  //	 payload.data, payload.PDULen - CHAT_HEADER_BYTE_LEN + HANDLE_BYTE_LEN)
  //getHandleLen(handleName)
  
  
}

void tokenizeDestHandles(uint8_t *numRecipients,
			 dHandle *destHandle,
			 uint8_t *buffer,
			 uint8_t flag){
  char    *token;
  uint8_t  destHandleLen;
  int i;
  
  token = strtok((char *)buffer, " "); /* Get 1st token %C or %M. Will discard*/

  if(flag == MULTICAST_FLAG)
    *numRecipients = atoi(strtok(NULL, " ")); /* Get 2nd token and convert to int*/
  else
    *numRecipients = UNICAST_RECIPIENTS;

  /* Multicast starts after 2nd space. Unicast starts at 1st space*/
  for(i = 0; i < *numRecipients; i++){
    token = strtok(NULL, " ");
    if(NULL == token)
      errorOut("unicastHandler(): A Null Byte was found in the user input buffer");
    destHandleLen = strlen(token);
    destHandle[i].destHandleLen = destHandleLen;

    if(verbosity)
      printf("DestHandle: %s with len: %d was found in buffer.\n",
	     token, destHandleLen);

    memcpy(destHandle[i].destHandle, token, destHandleLen);
    //strncpy(destHandle[i].destHandle, token, destHandleLen);

    if(verbosity)
      printf("DestHandle: %s with len: %d was added to handle arr at position: %d\n",
	     destHandle[i].destHandle, destHandle[i].destHandleLen, i);
  }
  return;
}

/* Probably change the parameter name from handleName to buffer?*/
void setChatHeaderPDULength(char *handleName, uint8_t flag){
  uint16_t PDULen = 0;
  PDULen = CHAT_HEADER_BYTE_LEN + HANDLE_BYTE_LEN + getHandleLen(handleName);
   
  clientChatHeader->PDULen = htons(PDULen);
  return;
}

void calculateChatHeaderPDULength(pduPayload payload){
  uint16_t PDULen = 0;
  uint8_t flag = payload.header.flag;
  PDULen = CHAT_HEADER_BYTE_LEN; /* All flags are atleast this long*/
  
   /* For flags 2, 3, 8, 9, 10, 13*/
  if(flag == 2 || flag == 3 || flag == 8 || flag == 9 || flag == 10 || flag == 13)
    payload.header.PDULen = htons(PDULen);
  else if(flag == 11)
    payload.header.PDULen = htons(PDULen + HANDLE_COUNT_LEN);
  else if(flag == 1 || flag == 12 || (flag >= 4 && flag <= 7)){ /* Check 1, 12, 4-7*/
    PDULen += HANDLE_BYTE_LEN + payload.handleLen; /* 1, 7, 12 only handels*/
    if(flag == 5 || flag == 6){ /* Add destHandle lens. 1 Byte + variable name len*/
      //for(i = 0 ; i < payload.data[payload.handleLen], i++){
	PDULen += RECIPIENTS_BYTE_LEN +
	          strlen((char *)(payload.data +
		   	         (RECIPIENTS_BYTE_LEN +
			          payload.handleLen))); /* strlen Might be one off*/
    }
    payload.header.PDULen = htons(PDULen);
  }
  if(verbosity)
    printf("Payload flag: %d calculated PDULen is: %d\n", flag, PDULen);
}

void setChatHeaderFlag(uint8_t flag){
  clientChatHeader->flag = flag;
  return;
}

uint16_t getChatHeaderPDULength(){
  return clientChatHeader->PDULen;
}

uint8_t getChatHeaderFlag(){
  return clientChatHeader->flag;
}

chatHeaderPtr getChatHeader(){
  return clientChatHeader;
}


void errorOut(char *error){
  if(errno)
    perror(error);
  else
    fprintf(stderr, "%s", error);
  exit(EXIT_FAILURE);
}
