#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

#include "clienthandle.h"

#define MAXBUF               1400
#define CHAT_HEADER_BYTE_LEN 3
#define HANDLE_BYTE_LEN      1
#define HANDLE_COUNT_LEN     4
#define HANDLE_MAX_LEN       100
#define RECIPIENTS_BYTE_LEN  1
#define UNICAST_RECIPIENTS   1

#define PDU_LEN_POS            0
#define FLAG_POS               3
#define HANDLE_LEN_POS         4
#define HANDLE_POS             5
#define NUM_HANDLES_POS        4
#define MISSING_HANDLE_POS     HANDLE_POS
#define MISSING_HANDLE_LEN_POS HANDLE_POS
#define NUM_RECIPIENTS_POS     4

#define INVALID_CMD_FLAG   0x00 /* Command was not reconized*/
#define C_INIT_FLAG        0x01 /* Client to server initial packet */
#define S_ACK_FLAG         0x02 /* Server to client acknowledge good handle packet*/
#define S_ACK_ERR_FLAG     0x03 /* Server to client handle error packet*/
#define BCAST_FLAG         0x04 /* Broadcast Client message to all clients on server*/
#define UNICAST_FLAG       0x05 /* Unicast cleint message to one client*/
#define MULTICAST_FLAG     0x06 /* Multicast message from Client to multiple clients*/
#define BAD_HANDLE_FLAG    0x07 /* Unicast\Multicast error message. Server to Client*/
#define C_EXIT_FLAG        0x08 /* Client to server termination initiation flag*/
#define S_EXIT_ACK_FLAG    0x09 /* Server to client termination acknowledgement flag*/
#define C_LIST_FLAG        0x0A /* Client to server requesting handle list flag*/
#define S_LIST_CNT_FLAG    0x0B /* Server to client return handle set count flag*/
#define S_SEND_HANDLE_FLAG 0x0C /* Server to client returning a single handle flag*/
#define S_END_LIST_FLAG    0x0D /* Server to client returning list is complete flag*/

extern int verbosity;

typedef struct chatHeader *chatHeaderPtr;
typedef struct dHandle *destHandlePtr;
typedef struct pdu *pduPtr;

typedef struct chatHeader{
  uint16_t PDULen;         /* 2 Bytes. Holds length of data*/
  uint8_t  flag;           /* 1 Byte. Holds pdu format flag*/
} __attribute__((packed)) chatHdr;

typedef struct destHandle{
  uint8_t destHandleLen;
  uint8_t destHandle[HANDLE_MAX_LEN];
} __attribute__((packed)) dHandle;

typedef struct pdu{
  chatHdr header;           /* 3 Bytes. Holds Chat Header*/
  uint8_t handleLen;        /* 1 Byte. Holds Handle Length*/
  uint8_t data[MAXBUF - HANDLE_BYTE_LEN - HANDLE_BYTE_LEN]; /* (1400 - 4) Bytes*/
} __attribute__((packed)) pduPayload;



int sendPDU(int clientSocket, uint8_t *dataBuffer, int lengthOfData);
void buildPDU(chatHdr chatHdr, uint8_t *dataBuffer, int handleLen, pduPtr pduBuf);
int recvPDU(int scktNum, uint8_t *dataBuf, int bufSize);

void flagHandler(int handleSocket, pduPtr pduBuf);
void handleFlag1(int handleSocket, uint8_t handleLen, char *handle);

void initChatHeader();
void buildChatHeader(char *handleName, uint8_t flag, uint8_t *buffer);
void unicastHandler(char *handleName, uint8_t *buffer, uint8_t flag);
void tokenizeDestHandles(uint8_t *numRecipients,
			 dHandle *destHandle,
			 uint8_t *buffer,
			 uint8_t flag);
void setChatHeaderPDULength(char *handleName, uint8_t flag);
void calculateChatHeaderPDULength(pduPayload payload);
void setChatHeaderFlag(uint8_t flag);
uint16_t getChatHeaderPDULength();
uint8_t getChatHeaderFlag();
chatHeaderPtr getChatHeader();

void errorOut(char *error);

#endif
