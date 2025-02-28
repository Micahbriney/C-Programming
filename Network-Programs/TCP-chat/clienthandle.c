#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>

#include "clienthandle.h"
#include "safeUtil.h"
#include "networks.h"
#include "transport.h"

/* Global variables*/
static handlePtr handleSet;
static int maxHandles = 0;
static int currHandleSetSize = 0;
static void growHandleSet();

uint16_t getHandleLen(char *handleName){
  uint16_t nameLen = 0;
  
  checkHandle(handleName);

  /* Length Check 100 chars max*/
  if(NULL == handleName)/* Check for a mysterious NULL*/
    errorOut("getHandleLen(): Can't have a NULL handleName");
  if(0 == (nameLen = strlen(handleName)))/* Check for another edge case of 0? length*/
    errorOut("Invalid handle, handle needs to be longer than nothing\n");
  
  else if(nameLen > 100){
    fprintf(stderr, "Invalid handle, handle longer than 100 characters: <%s>\n",
	    handleName);
    exit(EXIT_FAILURE);
  }
  return nameLen;
}

/* Check that client handle starts with a-z or A-Z and only contains alphanumerics*/
void checkHandle(char *handleName){

  if(NULL == handleName)/* Check for mysterious NULL name*/
    errorOut("CheckHandle(): Can't have a NULL handleName");
  if(!isalpha(handleName[0])){
    if(isdigit(handleName[0]))
      errorOut("Invalid handle, handle starts with a number\n");
    errorOut("Invalid handle, handle should start with alphabetical character\n");
  }
  while(*handleName){
    if(!isalnum(*handleName))
      errorOut("Invalid handle, handle can only contain alphalnumeric characters\n");
    handleName++;
  }
  return;
}

void setupHandleSet(){
  currHandleSetSize = HANDLE_SET_SIZE;
  handleSet = (handlePtr) sCalloc(HANDLE_SET_SIZE, sizeof(Handle));
  if(verbosity)
    printf("Handle Setup function call worked\n");
  return;
}

int addHandleToSet(int socketNumber, uint8_t handleLen, char *handleName){
  /* Check handleName for NULL / zero length*/
  if(handleLen == 0 || handleName == NULL || handleName[0] == '\0')
    errorOut("addHandleToSet() handleLen = 0 or handleName is NULL or empty\n");

  /* Check for existing handleName*/
  if(-1 == checkExistingHandles(handleName))
    return 0; /* Duplicate found*/
  
  /* Grow handle set if more space is needed*/
  if(socketNumber >= currHandleSetSize)
    growHandleSet(socketNumber + HANDLE_SET_SIZE);

  /* Increase max handle count if socketNumber is larger than max*/
  if(socketNumber + 1 >= maxHandles)
    maxHandles = socketNumber + 1;

  handleSet[socketNumber].socketNumber  = socketNumber;
  handleSet[socketNumber].handleNameLen = handleLen;
  //handleSet[socketNumber].handleName    = sCalloc(handleLen,
  //						  sizeof(char));
  //strncpy(handleSet[socketNumber].handleName, handleName, handleLen);
  memcpy(handleSet[socketNumber].handleName, handleName, handleLen);  
  if(verbosity)
    printf("Added Handle: %s (Length: %d) with SocketNumber: %d\n",
	   handleName, handleLen, socketNumber);

  return 1;
}

/* Return -1 if an existing handle was found, otherwise return 0*/
int checkExistingHandles(char *handleName){
  int ret = 0;
  int socket;

  for(socket = 0; socket < maxHandles; socket++){
    if(0 == (ret = strcmp(handleSet[socket].handleName, handleName))){
      if(verbosity)
	printf("Ducplicate handle found with socket: %d and handle: %s",
	       socket, handleName);
      return -1;
    }
  }
  return ret;
}

handlePtr getHandleSet(){
  return handleSet;
}

char *getHandleName(int socketNumber){
  return handleSet[socketNumber].handleName;
}

int getHandleNameLen(int socketNumber){
  return handleSet[socketNumber].handleNameLen;
}

int deactivateHandle(int socketNumber){
  handleSet[socketNumber].socketNumber  = 0;
  handleSet[socketNumber].handleNameLen = 0;
  memset(handleSet[socketNumber].handleName, 0, MAX_HANDLE_SIZE);
  return 1;
}

static void growHandleSet(int newSetSize){
  int i = 0;
  
  /* Sanity check handle set size*/
  if(newSetSize <= currHandleSetSize){
    fprintf(stderr,
	    "Error - current handle set size: %d newSetSize is not greater: %d\n",
	    currHandleSetSize, newSetSize);
    exit(EXIT_FAILURE);
  }

  if(verbosity)/* For testing*/
    printf("Increasing Handle Set from: %d to %d\n", currHandleSetSize, newSetSize);

  /* Expand set to hold more handle structs*/
  handleSet = srealloc(handleSet, newSetSize * sizeof(Handle));

  /* Clear the poll*/
  for(i = currHandleSetSize; i < newSetSize; i++){
    handleSet[i].socketNumber  = 0;
    handleSet[i].handleNameLen = 0;
    memset(handleSet[i].handleName, 0, MAX_HANDLE_SIZE);
  }
  
  currHandleSetSize = newSetSize;
}
