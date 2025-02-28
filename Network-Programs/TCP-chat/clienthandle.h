#ifndef CLIENTHANDLE_H
#define CLIENTHANDLE_H

#define HANDLE_SET_SIZE 10
#define MAX_HANDLE_SIZE 100

typedef struct handle *handlePtr;

typedef struct handle{
  int     socketNumber;
  uint8_t handleNameLen;
  char    handleName[MAX_HANDLE_SIZE];
} __attribute__((packed)) Handle;


uint16_t getHandleLen(char *handleName);
void checkHandle(char *handleName);
void setupHandleSet(void);
int addHandleToSet(int socketNumber, uint8_t handleLen, char *handleName);
int checkExistingHandles(char *handleName);
handlePtr getHandleSet(void);
char *getHandleName(int socketNumber);
int getHandleNameLen(int socketNumber);
int deactivateHandle(int socketNumber);


#endif
