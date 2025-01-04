#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "doublyLinkedList.h"
#include "safeutil.h"
#include "libDisk.h"

FreeBlock *head = NULL; /* Point to null*/

void initFreeBlocks(void){
    int i;
    FreeBlock* newNode;
    FreeBlock* currNode = NULL;

    for(i = 0; i < NUM_FREE_BLOCKS_; i++){
        newNode = createNode();
        newNode->position = i;
        if(newNode != NULL){
            if(currNode == NULL){
                head = newNode;   /* Set head to the first node */
            } 
            else{
                currNode->next = newNode; /* Connect current node to new node */
                newNode->prev = currNode; /* Set previous pointer of new node */
            }
            currNode = newNode; /* Update the current node */
        }
        else{
            errorout("createNode returned NULL");
            break;
        }
    }
    fs.free_blocks_head = (FreeBlock *)safeMalloc(sizeof(FreeBlock));
    fs.free_blocks_head = head;
    
    return;
}


FreeBlock* createNode(void){
    FreeBlock* newNode = (FreeBlock*)safeMalloc(sizeof(FreeBlock)); /* Get space for new node*/
    
    if(newNode != NULL){
       newNode->next        = NULL;
       newNode->prev        = NULL;
       newNode->inode_block = NULL;
       newNode->data_block  = NULL;
       newNode->type        = UNALLOCATED;
       newNode->position    = UNUSED_NODE_;
    }
    return newNode;
}

/* Returns the first unallocated Free Block node in the list*/
FreeBlock* allocateNode(void){
    FreeBlock* currNode = head;

    while(currNode != NULL){
        if(currNode->type == UNALLOCATED)
            return currNode;
        currNode = currNode->next;
    }

    return NULL; // No unallocated nodes available
}


void printList(void){
    FreeBlock* currNode = head;

    while(currNode != NULL){
        printf("Node Position: .%d\n", currNode->position);
        printf("Node Type: .%d\n", currNode->type);

        printFreeBlock(currNode->position);

        // if(currNode->type == INODE)
        //     printInodeBlock(int bNum, iNode block);
        // else if(currNode->type == DATA)
            // printHexDump(currNode->data_block, BLOCKSIZE_);
        // else
        //     printf("Unallocated Free Block.\n");

        currNode = currNode->next; // Move to the next node
    }

    return;
}

