#include "libTinyFS.h"
#include "libDisk.h"
#include "safeutil.h"
#include "doublyLinkedList.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

DiskInfo DiskList[MAX_NUM_DISKS_];
FileSystem fs;

int openDisk(char *filename, int nBytes)
{
    static uint8_t openFiles = 0;
    char blockBuffer[10];
    int flags = 0;
    int modes = 0;
    fileDescriptor fd;

    if(verbosity)
        printf("#################################### OPENING DISK ######################################\n\n");

    if(filename == NULL || strlen(filename) == 0)
        errorout("Filename is empty or NULL.\n");
    
    if(nBytes % BLOCKSIZE_)
    {
        sprintf(blockBuffer, "Number of Bytes (%d) is not divisible by BLOCKSIZE (%d).\n",nBytes, BLOCKSIZE_);
        errorout(blockBuffer);
    }

    if(nBytes > 0 && access(filename, F_OK) != 0)
    { /* File does not exist. So create it*/
        flags = O_RDWR | O_CREAT;  // Open for read/write, create if it doesnt't exist
        modes = S_IRUSR | S_IWUSR; // Give read write permissions to User
        fd = safeOpen(filename, flags, modes);
        if(verbosity){
            printf("Disk: %s created with user read and write permissions.\n",filename);
            printf("Disk: %s opened for reading and writing.\n", filename);
        }
    }
    else if(nBytes > 0)
    { /* File should exist. Open for read/write*/
        flags = O_RDWR;   // Open for read/write
        modes = 0;        // Don't need modes when not creating a file
        fd = safeOpen(filename, flags, modes);
        if(verbosity)
            printf("Disk: %s opened for reading and writing.\n\n", filename);
    }
    else if(nBytes == 0)
    { /* Assumed file is open*/
        flags = O_RDONLY; // Open for Read only
        modes = 0;        // Don't need modes when not creating a file
        fd = safeOpen(filename, flags, modes);
        if(verbosity)
            printf("Disk: %s opened for reading only.\n", filename);
    }
    else if(nBytes < 0){
        sprintf(blockBuffer, "Number of bytes (%d) must be positive.\n", nBytes);
        errorout(blockBuffer);
    }

    if(openFiles + 1 >= MAX_NUM_DISKS_){
        sprintf(blockBuffer, "%d disks open. Can't open anymore.\n", MAX_NUM_DISKS_);
        errorout(blockBuffer);
    }

    setDiskInfo(&DiskList[openFiles], filename, fd, nBytes, OPEN); /* Needs testing*/

    // if(strlen(filename) < MAX_DISKNAME_SIZE_)
    //     strcpy(DiskList[openFiles].disk_name, filename);
    // DiskList[openFiles].disk_size = nBytes;
    // DiskList[openFiles].fd = fd;
    // DiskList[openFiles].status = OPEN; /* Open, not mounted */

    /* Shallow copy should be ok since both are global*/
    // fs.disk_info = DiskList[openFiles]; /* Copy disk info to active filesystem*/

    if(verbosity){
        printf("---------------------------------- Opened Disk Info -----------------------------------\n");
        // printf("---------------- Disk List Info ----------------\n\n");
        // printDiskList();
        // printf("DiskList index = %d\n",openFiles);
        printDiskInfo(&(DiskList[openFiles]));
        // printf("Disk Name: %s\n", DiskList[openFiles].disk_name);
        // printf("File Descriptor: %d\n", DiskList[openFiles].fd);
        // printf("Disk Size: %zu\n", DiskList[openFiles].disk_size);
        // printf("Disk Status: %d\n", DiskList[openFiles].status);
        // printf("------------ File System Disk Info -------------\n");
        // printf("Disk Name: %s\n", fs.disk_info.disk_name);
        // printf("File Descriptor: %d\n", fs.disk_info.fd);
        // printf("Disk Size: %zu\n", fs.disk_info.disk_size);
        // printf("Disk Status: %d\n", fs.disk_info.status);
        // printf("---------------------------------------------------------------------\n");
    }
    /* Make a global open disk count?*/
    openFiles++; /* Increament total number of open disks*/

    if(verbosity)
        printf("########################################################################################\n\n");

    return fd;
}

// bNum = block index
// seek to bNum in disk & read BLOCKSIZE_ bytes
int readBlock(int disk, int bNum, void *block){
    ssize_t count;
    int offset;
    char* block_type;

    block_type = getBlockType(bNum);
    
    if(verbosity)
        printf("############################## READING %s ##################################\n", block_type);

    offset = bNum * BLOCKSIZE_;
    lseek(disk, offset, SEEK_SET);
    count = read(disk, block, BLOCKSIZE_);

    // INO_NAME_BLK_INDEX_  1        /* Block number for seek when read/write*/
    // INODE_BLOCK_INDEX_   2 
    if(verbosity && count > 0){
        /* Below assumes fs has been populated with disk_name during openDisk*/
        printf("Read %zd byte(s) of block %d from disk: %s.\n" , count, bNum, fs.disk_info.disk_name);
        // printf("Read %zd byte(s) from block %d from disk: %s.\n" , count, bNum, DiskList[disk].disk_name);
        printf("[DATA]: ");
        if(strcmp(block_type, "INODE NAME BLOCK") == 0){
            printHexDump(block, BLOCKSIZE_);                /* Print block*/
        }
        else if(strcmp(block_type, "INODE BLOCK") == 0){  /* If reading a INODE BLOCK*/
            if(((uint8_t *)block)[sizeof(dev_t)] != UNUSED_INODE_){ /* And the st_ino block is used*/
                printf("Inode Block\n");
                printHexDump(block, BLOCKSIZE_);                /* Print block*/
            }
            else
                printf("Block reads as an unused inode block.\n");
        }
        else if(strcmp(block_type, "DATA BLOCK") == 0){
            if(isBlockEmpty((uint8_t *)block) == 0){ /* If block is not empty*/
                printf("Data Block\n");
                printHexDump(block, BLOCKSIZE_); /* Print block*/
            }
            else
                printf("Block reads as an unused data block.\n");
        }
        else if(strcmp(block_type, "FREE BLOCK") == 0){
            if(fs.superblock.free_block_type[bNum - FREE_BLOCK_INDEX_] == INODE){
                printf("Allocated Inode Block\n");
                printHexDump(block, BLOCKSIZE_);
            }
            else if(fs.superblock.free_block_type[bNum - FREE_BLOCK_INDEX_] == DATA){
                printf("Allocated Data Block\n");
                printHexDump(block, BLOCKSIZE_);
            }
            else{
                printf("Block reads as an unallocated free block.\n");
            }

        }
        printf("#######################################################################################\n\n");
    }

    if(count > 0)
        return 0; /* 0 on Success*/
    else if(count == -1)
        errorout("#ERROR: readBlock -1.\n");
    else if(count == 0)
        errorout("#ERROR: readBlock EOF - not a valid block number.\n");
    else
        errorout("#ERROR: readBlock failed all cases.\n");

    errorout("#ERROR: readBlock reached this and should not have.\n");
    return -1;
}

int writeBlock(int disk, int bNum, void *block){
    ssize_t count;
    int offset;
    char *block_type;
    int disk_index = getDiskListIndex(disk, NULL);;
    
    block_type = getBlockType(bNum);

    if(verbosity)
        printf("############################## WRITING %s ####################################\n", block_type);

    offset = bNum * BLOCKSIZE_;
    lseek(disk, offset, SEEK_SET);
    count = write(disk, block, BLOCKSIZE_);

    if(verbosity){
        if(count > 0) /* Below assumes fs has been populated with disk_name during openDisk*/
            printf("Wrote %zd byte(s) from block %d to disk: %s.\n" , count, bNum, DiskList[disk_index].disk_name);
        // printf("[DATA]:\n"); // For Testing
        // printHexDump(block, BLOCKSIZE_); // For Testing
        printf("#########################################################################################\n\n");
    }

    if(count > 0)
        return 0; /* 0 on Success*/
    else if(count == -1)
        errorout("#ERROR: writeBlock -1.\n");
    else if(count == 0)
        errorout("#ERROR: writeBlock 0.\n");
    else
        errorout("#ERROR: writeBlock failed all cases.\n");

    errorout("#ERROR: writeBlock reached this and should not have.\n");
    return -1;
}


/* closeDisk() takes a disk number ‘disk’ and makes the disk closed to further I/O; 
i.e. any subsequent reads or writes to a closed disk should return an error. 
Closing a disk should also close the underlying file, 
committing any writes being buffered by the real OS. */

/* Still need to figure out what he means by committing writes. Is there a global
   buffer we need to flush?*/
void closeDisk(int disk){
    int diskIndex;

    if((diskIndex = getDiskListIndex(disk, NULL)) < 0)
        errorout("No Disk found when trying to close.\n");

    if(verbosity)
        printf("############################## CLOSING DISK %s ##############################\n", 
               DiskList[diskIndex].disk_name);
    
    if(commitDisk(DiskList[diskIndex]) != 0)
        errorout("Flushing disk failed while closing Disk.\n");
    
    errno = 0;
    close(disk);

    if(errno)
        errorout("CloseDisk failed.\n");
    
    
    /* Clear file system if disk was mounted.*/
    if(DiskList[diskIndex].status == MOUNTED){
        initFileSystem();
        if(verbosity)
            printf("File system cleared.");
    }
    
    DiskList[diskIndex].fd = -1;         // Set to invalid FD
    DiskList[diskIndex].status = CLOSED; // Set Disk status to closed

    if(verbosity){
        printf("-------------------------- Closing Disk Info -------------------------\n");
        printf("DiskList index = %d\n",diskIndex);
        printf("---------------- Disk List Info ----------------\n");
        printf("Disk Name: %s\n", DiskList[diskIndex].disk_name);
        printf("File Descriptor: %d\n", DiskList[diskIndex].fd);
        printf("Disk Size: %zu\n", DiskList[diskIndex].disk_size);
        printf("Disk Status: %d\n", DiskList[diskIndex].status);
        printf("------------ File System Disk Info -------------\n");
        printf("Disk Name: %s\n", fs.disk_info.disk_name);
        printf("File Descriptor: %d\n", fs.disk_info.fd);
        printf("Disk Size: %zu\n", fs.disk_info.disk_size);
        printf("Disk Status: %d\n", fs.disk_info.status);
        printf("---------------------------------------------------------------------\n");
        printf("############################## CLOSING DISK %s ##############################\n", 
            DiskList[diskIndex].disk_name);
    }

    return;
}

// diskInfo = disk_info of disk being flushed.
// Returns 0 on when everything successfully writes to disk
int commitDisk(DiskInfo curr_disk_info){
    int i;
    uint8_t block_type;
    uint8_t buffer[BLOCKSIZE_] = {0}; /* Empty buffer*/
    // int curr_disk_size = curr_disk_info.disk_size;
    int curr_fd = curr_disk_info.fd;
    // int remaining_inodes = fs.superblock.inode_count;

    if(curr_disk_info.status != OPEN && curr_disk_info.status != MOUNTED)
        errorout("Comminting to disk failed. Disk already closed.\n");

    if(verbosity)
        printf("############################## COMMITING BLOCKS TO DISK ##############################\n");
        
    /* Write SuperBlock*/
    memcpy(buffer, &(fs.superblock), sizeof(fs.superblock));
    writeBlock(curr_fd, SUPER_BLOCK_INDEX_, buffer);
    if(verbosity)
        printHexDump(buffer, BLOCKSIZE_);
    memset(buffer, 0, BLOCKSIZE_); /* Clear buffer*/

    if(verbosity)
        printf("Superblock written to file: %s.\n", curr_disk_info.disk_name);

    /* Write Special Inode block*/
    memcpy(buffer, &(fs.special_inode_list), sizeof(fs.special_inode_list));
    writeBlock(curr_fd, INO_NAME_BLK_INDEX_, buffer);
    if(verbosity)
        printHexDump(buffer, BLOCKSIZE_);
    memset(buffer, 0, BLOCKSIZE_); /* Clear buffer*/

    if(verbosity)
        printf("Special Inode block written to file: %s.\n", curr_disk_info.disk_name);

    /* Write Inode blocks */
    for(i = 0; i < NUM_INODES_; i++){
        memcpy(buffer, &(fs.inode_blocks[i]), sizeof(fs.inode_blocks[i]));
        writeBlock(curr_fd, INODE_BLOCK_INDEX_ + i, buffer);
        if(verbosity){
            printf("Inode block %d written to file: %s.\n", i, curr_disk_info.disk_name);
            printf("[DATA]: ");
            if(fs.inode_blocks[i].st_ino != UNUSED_INODE_){
                printf("\n");   
                printHexDump(buffer, BLOCKSIZE_);
            }
            else
                printf("Appears Unused.\n\n");
        }
        memset(buffer, 0, BLOCKSIZE_); /* Clear buffer*/
    }

    /* Write Data blocks*/
    for(i = 0; i < NUM_DATA_BLOCKS_; i++){
        memcpy(buffer, &(fs.data_blocks[i]), sizeof(fs.data_blocks[i]));
        writeBlock(curr_fd, DATA_BLOCK_INDEX_ + i, buffer);
        if(verbosity){
            printf("Data block %d written to file: %s.\n", i, curr_disk_info.disk_name);
            printf("[DATA]: ");
            if(isBlockEmpty(buffer) == 0){
                printf("\n");
                printHexDump(&(buffer), BLOCKSIZE_);
            }
            else
                printf("Appears empty.\n\n");
        }
        memset(buffer, 0, BLOCKSIZE_); /* Clear buffer*/
    }

    /* Write Free blocks*/
    for(i = 0; i < NUM_FREE_BLOCKS_; i++){
        FreeBlock *currHead = fs.free_blocks_head;
        if(currHead == NULL)
            errorout("CurrHead is null while writing Free blocks.\n");
        block_type = fs.superblock.free_block_type[i];
        
        if(block_type == INODE)
            memcpy(buffer, currHead->inode_block, BLOCKSIZE_);
        else if(block_type == DATA)
            memcpy(buffer, currHead->data_block, BLOCKSIZE_);
        else
            memset(buffer, 0, BLOCKSIZE_); /* Copy over empty data*/

        currHead = currHead->next;

        writeBlock(curr_fd, FREE_BLOCK_INDEX_ + i, buffer);
        
        if(verbosity){
            printf("Free block %d written to file: %s.\n", i, curr_disk_info.disk_name);
            printf("[DATA]: ");
            if(block_type == INODE){
                if(currHead->inode_block->st_ino != UNUSED_INODE_){
                    printf("\n");   
                    printHexDump(buffer, BLOCKSIZE_);
                }
                else
                    printf("Free Inode Block Appears Unused.\n\n");
            }
            else if(block_type == DATA){
                if(isBlockEmpty(buffer) == 0){
                    printf("\n");
                    printHexDump(&(buffer), BLOCKSIZE_);
                }
                else
                    printf("Free Data Block Appears empty.\n\n");
            }
            else
                printf("Free Block Appears Unallocated.\n\n");
        }
        memset(buffer, 0, BLOCKSIZE_); /* Clear buffer*/
    }

    if(verbosity)
        printf("#########################################################################################\n\n");

    return 0;
}

/* Find the DiskList index for provided filename or disk (disk file descriptor)
   When disk is < 0 only search using filename
   When filename is empty or NULL only search using disk
   Returns DiskList index when a filename or disk match occurs
   Returns -1 if neither filename nor disk was not found in DiskList*/
int getDiskListIndex(int disk, char *filename){
    int index;

    if(disk > 0){ // Check for matching file descriptor
        for(index = 0; index < MAX_NUM_DISKS_; index++)
            if(DiskList[index].fd == disk)
                return index;
    }

    if(filename != NULL && filename[0] != '\0'){ // Check for matching filename
        for(index = 0; index < MAX_NUM_DISKS_; index++)
            if(strcmp(DiskList[index].disk_name, filename) == 0)
                return index;
    }
    
    return -1; // Nothing found
}

/* Returns DiskList index for the mounted disk*/
/* Else returns -1 if no disks are mounted*/
int getMountedDisk(void){
    int diskListIndex;

    for(diskListIndex = 0; diskListIndex < MAX_NUM_DISKS_; diskListIndex++) /* Check if any disks are already mounted*/
        if(DiskList[diskListIndex].status == MOUNTED)
            return diskListIndex;

    if(verbosity)
        printf("No Mounted Disk Found.\n");

    return -1; /* No mounted disk found*/
}

char* getBlockType(int bNum){
    char* block_type; 
    block_type = (bNum == SUPER_BLOCK_INDEX_) ? "SUPER BLOCK" :
                (bNum == INO_NAME_BLK_INDEX_) ? "INODE NAME BLOCK" :
                (bNum >= INODE_BLOCK_INDEX_ && bNum < DATA_BLOCK_INDEX_) ? "INODE BLOCK" :
                (bNum >= DATA_BLOCK_INDEX_ && bNum < FREE_BLOCK_INDEX_) ? "DATA BLOCK" :
                (bNum >= FREE_BLOCK_INDEX_ && bNum < NUM_BLOCKS_) ? "FREE BLOCK" :
                "OUT OF SCOPE";
    return block_type;
}

void printHexDump(const void* data, size_t size){
    const unsigned char* dataPtr = (const unsigned char*)data;
    size_t i, j;
    
    printf("################################## HEX DUMP ##################################\n");

    for(i = 0; i < size; i += 16){ /* Print offset*/
        printf("%08zx ", i);
        for(j = 0; j < 16; j++){   /* Print the data hex values*/
            if(i + j < size)
                printf("%02x ", dataPtr[i + j]);
            else
                printf("-- ");     /* Empty data space*/

            if(j == 7)  /* Insert extra space after 8 bytes*/
                printf(" ");
        }

        printf("    "); /* Space between hex and ASCII*/

        for(j = 0; j < 16; j++){ /* Print ASCII representation*/
            if(i + j < size)
                printf("%c", (dataPtr[i + j] >= 32 && 
                              dataPtr[i + j] <= 126)  ? dataPtr[i + j] : '.');
            else
                printf(" ");
        }

        printf("\n");
    }
    
    printf("##############################################################################\n\n");

    return;
}