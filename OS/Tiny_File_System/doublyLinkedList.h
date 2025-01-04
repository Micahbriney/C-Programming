#ifndef MDLLH
#define MDLLH

#include "libTinyFS.h"

typedef struct inode iNode;           /* Forward declaration */
typedef struct data_block DataBlock;  /* Forward declaration */

#define UNUSED_NODE_ 255 

typedef struct free_block FreeBlock;

//Element structure
struct free_block{
    FreeBlock *next;       /* Navigate towards end*/
    FreeBlock *prev;       /* Navigate towards front*/
    iNode *inode_block;    /* Store inode block data*/
    DataBlock *data_block; /* Store data block data*/
    uint8_t type;          /* UNALLOCATED, INODE, DATA*/
    uint8_t position;      /* For indexing*/
}__attribute__((packed));


void initFreeBlocks(void);
FreeBlock* createNode(void);
FreeBlock* allocateNode(void);
void printList(void);

extern FreeBlock *head;

#endif
