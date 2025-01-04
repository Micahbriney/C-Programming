#include "libTinyFS.h"
#include "libDisk.h"
#include "safeutil.h"

#include <unistd.h>
#include <string.h>
#include <time.h>

// iNode iNodeList[NUM_INODES_];
// Superblock sb;
FileSystem fs;
DiskInfo DiskList[MAX_NUM_DISKS_];

int tfs_mkfs(char *filename, int nBytes){
    DiskInfo curr_disk_info;
    fileDescriptor fd = 0;

    initFileSystem(); /* A bit overkill but is fine.*/

    /* Open Disk will add to DiskList*/
    if((fd = openDisk(filename, nBytes)) < 0)
        errorout("tfs_mkfs openDisk failed");
    
    /* Add disk info to local varaible. Probably dont need.*/
    curr_disk_info.fd = fd;
    curr_disk_info.disk_size = nBytes;
    strcpy(curr_disk_info.disk_name, filename);
    curr_disk_info.status = OPEN;

    /* Add superblock data*/
    fs.superblock.disk_size = nBytes;
    fs.superblock.magic_num = MAGIC_NUM_TYPE_DISK_;
   
    /* Unused data*/
    // fs.superblock.data_blocks_count;
    // fs.superblock.free_blocks_count;
    // fs.superblock.inode_count;
    // fs.superblock.special_bNum = 0;

    closeDisk(curr_disk_info.fd);  /* Write to disk and close*/
    
    return 0;
}

// open disk & do checks

/* tfs_mount(char *filename) “mounts” a TinyFS file system located within ‘filename’. 
 As part of the mount operation, tfs_mount should verify the file system is the correct type. 
 Only one file system may be mounted at a time. 
 Use tfs_unmount to cleanly unmount the currently mounted file system.
  Must return a specified success/error code. */
int tfs_mount(char *filename){
    int disk_index = -1;
    // int file_found = -1;
    char block_buffer[BLOCKSIZE_];

    if(verbosity){
        printf("############################## MOUNTING DISK %s ##############################\n\n", filename);
        printf("... Search for other mounted disks. Abort mounting %s if found ...\n", filename);
    }

    if(getMountedDisk() != -1) /* Check if any disks are already mounted*/
        errorout("A disk is already mounted.\n");

    // for(i = 0; i < MAX_NUM_DISKS_; i++) /* Check if any disks are already mounted*/
    //     if(DiskList[i].status == MOUNTED)
            // errorout("A disk is already mounted.\n");

    // FileSystem currFileSystem; // Change if/when we get a global Filesystem struct

    /* Check to see if the file has been opened before trying to mount.*/
    if((disk_index = getDiskListIndex(disk_index, filename)) == -1) /* Check if filename exists, get DiskList index*/
        errorout("!!!! Disk not found in DiskList.\n"); /* Might consider attempting to open from here*/
    

    initFileSystem(); /* Prep the file system global*/

    // printf("Before copy\n");
    // printDiskInfo(&fs.disk_info);

    /* Shallow copy should be ok since both are global*/
    if(verbosity)
        printf("... Populating Filesystem's Disk Info ....\n");
    fs.disk_info = DiskList[disk_index]; /* Copy disk info to active filesystem*/
    
    if(verbosity){
        printf("--------------------------- File System Disk Info ----------------------------\n");
        printDiskInfo(&fs.disk_info);
    }
    // printf("After copy\n");
    // printDiskInfo(&fs.disk_info);

    //strcpy(fs.disk_info.disk_name, filename); /* Save disk name into filesystem*/
    
    // printf("disk_index: %d\n", disk_index);
    // printf("Disk fd: %d\n", DiskList[disk_index].fd);
    // printf("Disk status: %d\n", DiskList[disk_index].status);
    /* Might be a redudent check. Not sure how we might added a disk to DiskList without opening it first
       and that was checked above.*/
    if(DiskList[disk_index].status != OPEN)  /* File is not open. */
        errorout("Disk Is Not Open.\n");      
    
    fs.disk_info.status = OPEN;               /* Set file system disk info status to OPEN*/

    if(verbosity)
        printf("--------- Copying Super Block From Disk ----------\n\n");

    /* Write the super block to the fs*/
    if(readBlock(fs.disk_info.fd, SUPER_BLOCK_INDEX_, (void *)block_buffer) != 0)
        errorout("readBlock failed in tfs_mount.\n");

    if(verbosity)
        printf("... Copying Super Block Info To File System Memory ...\n");
    memcpy(&(fs.superblock), block_buffer, BLOCKSIZE_); // Need to test

    if(verbosity){
        printSuperBlock();
        printf("... Checking Superblocks Magic Number ...\n");
    }
    
    if(fs.superblock.magic_num == MAGIC_NUM_TYPE_DISK_ || 
       fs.superblock.magic_num == MAGIC_NUM_TYPE_USB_){
        DiskList[disk_index].status = MOUNTED;            /* Update DiskList's disk status to MOUNTED*/
        fs.disk_info.status = MOUNTED;                    /* Update fs disk status to MOUNTED*/
        // fs.disk_info.disk_size = fs.superblock.disk_size; /* Set fs disk info's disk size using superblock*/
        // fs.disk_info.fd = DiskList[disk_index].fd;        /* Set fs disk info file descriptor*/
        if(verbosity){
            printf("... Updating Filesystem's Disk Info Status To MOUNTED ....\n");
            printf("[DISK STATUS UPDATED]: MOUNTED\n");
            printDiskInfo(&fs.disk_info);
        }
    }
    else{
        errorout("File type not recognized. (Magic Num check in tfs_mount).\n");
    }
   
    if(verbosity){
        printf("############################## %s MOUNTED ##############################\n", filename);
        printf("... Processing Meta Data ...\n");
    }
    

    /* Process meta data will load rest of the data into memory.*/
    processMetaData();

    return 0;
}

// close disk
// tfs_unmount(void) “unmounts” the currently mounted file system.
int tfs_unmount(void)
{
    int diskIndex;
    commitDisk(fs.disk_info); /* Write all data to disk before unmounting it*/

    diskIndex = getDiskListIndex(fs.disk_info.fd, fs.disk_info.disk_name);
    DiskList[diskIndex].status = UNMOUNTED; /* Allows for remounting while file is still open*/

    initFileSystem(); /* Clear file system*/

    return 0;
}

/* Opens a file for reading and writing on the currently mounted file system. 
Creates a dynamic resource table entry for the file (the structure that tracks open files, 
the internal file pointer, etc.), and returns a file descriptor (integer) that can be used to reference 
this file while the filesystem is mounted. */
fileDescriptor tfs_open(char *name){
    fileDescriptor fd;
    int i;
    int inode_name_pair_index;
    int new_inode, new_data_block;
    int inode_count, free_count, data_count;
    FreeBlock *currHead = NULL;
    FreeBlock *tempFree = NULL;
    iNode *new_inode_ptr;
    struct timespec current_time;


    inode_count = fs.superblock.inode_count;
    free_count = fs.superblock.free_blocks_count;
    data_count =  fs.superblock.free_blocks_count;
    
    if(verbosity)
        printf("######################### OPENING FILE #########################\n");

    /* Check if the file exists*/
    inode_name_pair_index = getInodeNamePairIndex(name);
    if(inode_name_pair_index == -1){ /* File not found*/
        if(inode_count < NUM_INODES_){        /* There is at least 1 inode block left*/
            new_inode = findAvailableInode();           /* Find a free Inode block.*/
            fs.superblock.available_inodes[new_inode] = ALLOCATED; /* Update bitmap*/
            new_data_block = findAvailableDataBlock(); /* Find a free data block. Won't need free block*/
            fs.superblock.data_blocks_count++;          /* Increment data block count in fs*/
            // inode_name_pair_index = addInodeNamePair(name, new_inode);  /* Add to inode_name_block*/
            fs.inode_blocks[new_inode].data_indexs[0] = new_data_block; /* Set first data block*/
            fs.inode_blocks[new_inode].st_blocks = 1; /* Increment number of data blocks used*/
            fs.inode_blocks[new_inode].st_uid =  USER_ID_; /* Set user id*/
            fs.inode_blocks[new_inode].st_gid =  GROUP_ID_; /* Set user id*/
            setTimestamps(&(fs.inode_blocks[new_inode]));
        } // TODO finish below
        else if(inode_count < MAX_INO_NAME_PAIRS_){ /* No Inodes blocks left*/
            if(free_count < (NUM_FREE_BLOCKS_ - MIN_FREE_BLK_INODE_)){ /* At least 2 free blocks left*/
                currHead = allocateNode();    /* Look for a free block*/
                if(currHead == NULL)          /* Null check*/
                    errorout("Allocating for open (currHead), free count suggests nodes available.\n");
                new_inode = NUM_INODES_ + currHead->position; /* Save the new inode number*/
                fs.superblock.free_blocks_count++;  /* Increment the free block counter.*/
                fs.superblock.inode_count++; /* Increment the inode block counter*/
                if(currHead->type == UNALLOCATED){ /* Redundent check*/
                    currHead->type = INODE;        /* Set the type to Inode*/
                    new_inode_ptr = (iNode *)safeMalloc(sizeof(iNode)); /* Make a inode pointer*/
                    initInode(new_inode_ptr);    /* Initilize the new inode pointer*/
                    new_inode_ptr->st_ino = inode_count; /* Set the inode number*/
                    // TODO test this at some point
                    new_data_block = findAvailableDataBlock(); /* Find a free data block. Might need free block*/
                    if(new_data_block == INVALID_DATA_INDEX_){ /* Get a free block for data*/
                        tempFree = allocateNode();             /* Find a free block*/
                        if(tempFree == NULL)                   /* NULL check for edge cases*/
                            errorout("Allocating for open (tempFree), free count suggests nodes available.\n");
                        fs.superblock.free_blocks_count++;     /* Increment free block count*/
                        fs.superblock.free_block_type[tempFree->position] = DATA; /* Update free block bitmap*/ 
                        tempFree->type = DATA;                      /* Set the free block type to data*/
                        tempFree->data_block = (DataBlock *)safeMalloc(sizeof(DataBlock));/*Get free data blck space*/
                        memset(tempFree->data_block, 0, BLOCKSIZE_);  /* Clear the data block*/
                        new_inode_ptr->data_indexs[0] = NUM_DATA_BLOCKS_ + tempFree->position; /* Set the linked list position*/
                    }
                    else
                        new_inode_ptr->data_indexs[0] = new_data_block; /* Set first data block*/
                    fs.superblock.data_blocks_count++;          /* Increment data block count in fs*/

                    new_inode_ptr->st_blocks = 1;       /* At least 1 block*/
                    new_inode_ptr->st_uid =  USER_ID_;  /* Set user id*/
                    new_inode_ptr->st_gid =  GROUP_ID_; /* Set user id*/
                    setTimestamps(new_inode_ptr);       /* Set time stamps*/
                    currHead->inode_block = (iNode *)safeMalloc(sizeof(iNode));
                    memcpy(currHead->inode_block, new_inode_ptr, BLOCKSIZE_);
                    free(new_inode_ptr);
                    fs.superblock.free_block_type[currHead->position] = INODE; /* Upate free block bitmap*/
                }
                ;
            }
            else if(free_count < (NUM_FREE_BLOCKS_ - 1) && 
                    data_count < NUM_DATA_BLOCKS_){ /* Can make a free block inode and use data blocks.*/
                ; /* Allocate one free inode and find the free data block*/
            }
        } 

        /* Add inode name pair*/
        inode_name_pair_index = addInodeNamePair(name, new_inode);
        if(verbosity)
            printf("... Updating Superblocks Inode Count ...\n");
        fs.superblock.inode_count++;

    }
    

        
    
    /* Check if the file is already open*/
    if(inode_name_pair_index > 0 && ((fd = getFileDescriptorIndex(name)) != -1)){ /* File exists*/
        printf("File already open!\n");
        return fd;
    }
    else{

    }
    
    // TODO Still need to set fs.file_descriptors info
    /* need to consider my code below. There is a difference between FD's and inodes
       There can be as many FDs as MAX_INO_NAME_PAIRS_. But only if there is also enough 
       data blocks (or free blocks for data). Remember that atleast one data block for one inode.
       fd can be released but inodes(number of files) can still exist. Also inodes(files) can be
       deleted. The file_descriptors[] index can be used as the fd*/

    /* Before creating a new file check if there exists any free inodes
       or if we need to allocate an inode from the free blocks.*/
    if((fd = getNextOpenFD() > NUM_INODES_))
        ;// TODO allocate inode from free blocks
    else if(fd == -1){
        commitDisk(fs.disk_info); /* Try saving all of the information before exiting program*/
        printf("!!! NO FILE DESCRIPTORS LEFT !!!\n--- Close files before opening a new one."); /* No fds left*/
    }

    /* If we are here we should have a working fd.*/
    addFileDescriptor(name, fs.special_inode_list[inode_name_pair_index].inode);

    /* Check if file exists in special inode
       if not then it will need to be "created"*/
    for(i = 0; i < MAX_INO_NAME_PAIRS_; i++){
        if(strcmp(fs.special_inode_list[i].name, name) == 0 &&
           fs.special_inode_list[i].inode != INVALID_INODE_){
            break; /* Found*/
        }
            // fs.file_descriptors[i].
    }
    /* write inode and name to special inode block*/
    /* Populate inode data. set number of data block count to 1. We atleast need one
       set time information realted to creating/opening the file (Bonus for demo)*/

    clock_gettime(CLOCK_REALTIME, &current_time);

    if(verbosity){
        printf("... Bonus Updating Status Change Time ...");
        print_timespec("Last Time of Status Change: ", 
        &(fs.inode_blocks[fs.special_inode_list[inode_name_pair_index].inode].st_ctim));
    }

    sleep(2); // Add a 2-second delay
    
    clock_gettime(CLOCK_REALTIME, &current_time);
    fs.inode_blocks[fs.special_inode_list[inode_name_pair_index].inode].st_ctim = current_time; /* Update access time*/

    if(verbosity){
        printf("... Modify Status Change Time ...");
        print_timespec("Last Time of Status Change: ", 
        &(fs.inode_blocks[fs.special_inode_list[inode_name_pair_index].inode].st_ctim));
    }

    return 0;
}

int tfs_close(fileDescriptor FD)
{
    setTimestamps(&(fs.inode_blocks[fs.file_descriptors[FD].inode])); /* Set time stamps*/
    fs.file_descriptors[FD].inode = UNUSED_INODE_;
    memset(fs.file_descriptors[FD].name, 0, MAX_FILE_SIZE_);
    fs.file_descriptors->seek_position = 0;
    fs.file_descriptor_count--;
    fs.special_inode_list->seek_position = 0;
    
    return 0;
}

/* How much are we writting? 
    Is the size bigger than remaining data?
      If yes can we add another data block? 
         are we at 5 blocks?
         If no are there any more data blocks free?
         if no are there any free blocks?
      If no then write the data to the already existing block*/
int tfs_write(fileDescriptor FD, char *buffer, int size){
    int i;
    int fd_inode;
    int pair_index, data_block_index;
    int file_size;
    int inode;
    FreeBlock *free_block_inode = NULL;
    int list_pos;
    uint8_t list_flag = 0;
    off_t curr_data_offset;    /* Where to start writting to on data blocks.*/
    int remaining_size = size; /* Will decrement when writting occurs*/
    int write_amount;          /* How much to write to data block*/
    int buffer_offset;
    int starting_block;
    int num_blocks;
    int new_data_block;
    FreeBlock *temp_free_data = NULL;
    struct timespec current_time;


    fd_inode = fs.file_descriptors[FD].inode;
    // if inode > NUM_INODES_ subtract NUM_INODES to find linked list pos
    pair_index = getInodeNamePairIndex(fs.file_descriptors[FD].name);
    if(fd_inode != fs.special_inode_list[pair_index].inode)
        errorout("File Descriptor and Special Inode have mismatched inodes.\n");
    
    /* When writting need to consider that an data_index[i] > NUM_DATA BLOCKS mean the the data is in a Free block
       so to find the free block position sub tract NUM_DATA_BLOCKS from the data_index[i] value*/
    /* Check if we are using a Free Block of Inode Block for the inode*/
    if(fd_inode > NUM_INODES_){ /* Using a Free Block Inode*/
        list_pos = fd_inode - NUM_INODES_; /* Get position of Free inode block*/
        free_block_inode = fs.free_blocks_head;  /* Set Free Block inode to head*/
        for(i = 0; i < list_pos; i++) /* Move Free Block inode to list position*/
            free_block_inode = free_block_inode->next;
        list_flag = 1;
    } 



    /* Check remaining data. And allocate necessary data blocks*/
    if(list_flag){ /* Need to use Free Blocks*/
        curr_data_offset = free_block_inode->inode_block->st_size;
        if((curr_data_offset + size) < MAX_FILE_SIZE_){ /* Enough space left*/
            free_block_inode->inode_block->st_size += size; /* Increase the size*/
            /* Recalc num blocks required*/
            num_blocks = (free_block_inode->inode_block->st_size + BLOCKSIZE_ - 1) / BLOCKSIZE_;
            if((free_block_inode->inode_block->st_blocks < DATA_BLKS_PER_INODE_) && 
               (num_blocks > free_block_inode->inode_block->st_blocks)){ /*Check if we need to allocate data blocks*/
                starting_block = free_block_inode->inode_block->st_blocks;
                for(i = starting_block; i < num_blocks; i++){
                    free_block_inode->inode_block->st_blocks++; /* Increment the block count*/
                    // TODO: Go though logic again. Power went out and not sure what was lost.
                    if((new_data_block = findAvailableDataBlock()) == INVALID_INODE_){/* if no Data Blocks remain*/
                        temp_free_data = allocateNode();  /* Allocate a data Block node*/
                        if(temp_free_data == NULL)
                            errorout("No Free Blocks left to allocate for Data.\n");
                        fs.superblock.free_blocks_count++; /* Increment the num of free blocks used*/
                        temp_free_data->type = DATA;       /* Set free block type to DATA*/
                        temp_free_data->data_block = (DataBlock *)safeMalloc(sizeof(DataBlock));/* Get space for Data*/
                        memset(temp_free_data->data_block, 0 ,BLOCKSIZE_); /* Clear the data block*/
                        /* Set data position*/
                        free_block_inode->inode_block->data_indexs[i] = NUM_DATA_BLOCKS_ + temp_free_data->position;
                        fs.superblock.free_block_type[temp_free_data->position] = DATA; /* Update Free Block bitmap*/
                        temp_free_data = NULL; /* Set back to NULL for llocateNode() in loop*/
                    }
                    else{
                        free_block_inode->inode_block->data_indexs[i] = new_data_block; /* Set data index to Data Block index*/
                        fs.superblock.available_data_blocks[new_data_block] = ALLOCATED; /* Update Data Block Bitmap*/
                        fs.superblock.data_blocks_count++; /* Increment the num of data blocks used*/
                    }
                }

            }
        }
        else
            errorout("Cannot write to file. Not enough space.\n");
    }
    else{ /* Data Blocks availiable*/
        curr_data_offset = fs.inode_blocks[fd_inode].st_size;
        if((curr_data_offset + size) < MAX_FILE_SIZE_){
            fs.inode_blocks[fd_inode].st_size += size; /* Increase the size*/
            /* Recalc num blocks required*/
            num_blocks = (fs.inode_blocks[fd_inode].st_size + BLOCKSIZE_ - 1) / BLOCKSIZE_;
            if((fs.inode_blocks[fd_inode].st_blocks < DATA_BLKS_PER_INODE_) && 
               (num_blocks > fs.inode_blocks[fd_inode].st_blocks)){ /* Check if we need to allocate data blocks*/
                starting_block = fs.inode_blocks[fd_inode].st_blocks;
                for(i = starting_block; i < num_blocks; i++){
                    fs.inode_blocks[fd_inode].st_blocks++; /* Increment the block count*/
                    if((new_data_block = findAvailableDataBlock()) == INVALID_INODE_){/* if no Data Blocks remain*/
                        ; //TODO /* Allocate a free block in rare case.*/
                    }
                    else{
                        fs.inode_blocks[fd_inode].data_indexs[i] = new_data_block; /* Set data index to Data Block index*/
                        fs.superblock.available_data_blocks[new_data_block] = ALLOCATED; /* Update Data Block Bitmap*/
                        fs.superblock.data_blocks_count++; /* Increment the num of data blocks used*/
                    }
                }

            }
        }
    }

    if(remaining_size > (curr_data_offset % BLOCKSIZE_)){ /* We need to write to more than one block*/
        if(list_flag){
            ; // TODO implement Free Block data writing
        }
        else{ /* Num blocks should already be set*/
            /* Initial write to data block to finish out the block*/
            // write_amount = BLOCKSIZE_ - (curr_data_offset % BLOCKSIZE_); /* Write to end of first Data Block*/
            write_amount = size; /* Need to fix this.*/
            buffer_offset = 0; /* Start buffer at beginning*/

            /* loop for each block that needs to be written to.*/
            for(i = starting_block; i < num_blocks; i++){
                data_block_index = fs.inode_blocks[fd_inode].data_indexs[i]; /* Get data block index*/
                
                memcpy(fs.data_blocks[data_block_index].data + (curr_data_offset % BLOCKSIZE_), 
                       buffer + buffer_offset, 
                       write_amount); /* First loop starts at data[x], subsequent start at data[0]*/
                
                buffer_offset = write_amount;      /* Move the buffer offset for subsequent memcpys*/
                remaining_size -= write_amount;    /* Decrement remaining data to write*/
                
                if(remaining_size > BLOCKSIZE_)    /* If writing more than a block*/
                    write_amount = BLOCKSIZE_;     /* write a block*/
                else
                    write_amount = remaining_size; /* else write remaining buffer under BLOCKSIZE_*/
                
                curr_data_offset += write_amount;  /* This should be 0 now*/
            }
        }
    }
    else{ /* Write only to last data block in data_index*/
        if(list_flag){
            ; // Todo: implement Free Block data writing
        }
        else{ /* Dont need offset for buffer, the write amount is the entire size of the buffer.*/
            data_block_index = fs.inode_blocks[fd_inode].data_indexs[i];
            memcpy(fs.data_blocks[data_block_index].data + (curr_data_offset % BLOCKSIZE_), buffer, size);
            remaining_size = 0;
        }
    }

    clock_gettime(CLOCK_REALTIME, &current_time);

    if(verbosity){
        printf("... Bonus Updating Modify Time ...");
        print_timespec("Last Time of Modification: ", &(fs.inode_blocks[fd_inode].st_mtim));
    }

    sleep(2); // Add a 5-second delay
    
    clock_gettime(CLOCK_REALTIME, &current_time);
    fs.inode_blocks[fd_inode].st_mtim = current_time; /* Update access time*/

    if(verbosity){
        printf("... Modify Time Updated ...");
        print_timespec("Last Time of Modification: ", &(fs.inode_blocks[fd_inode].st_mtim));
    }

    return 0;
}

int tfs_delete(fileDescriptor FD)
{
    /* Look to see if the fileDescriptor exists (signalling that file is open)
         If the fd exists then set the inode and data blocks to unused/invalid/empty/free*/
    return 0;
}

/* reads one byte from the file and copies it to ‘buffer’, 
using the current file pointer location and incrementing it by one upon success. 
If the file pointer is already at the end of the file then tfs_readByte() 
should return an error and not increment the file pointer. */
/* Read one byte starting at file seek position.
   return 0 for success and set char *buffer
   return -1 for EOF*/
int tfs_readByte(fileDescriptor FD, char *buffer)
{
    struct timespec current_time;
    int fd_inode;         /* Inode of the FD*/
    int curr_position;    /* Location in overall data blocks if they were lined up*/
    int block_position;   /* Loction in the specific data block*/
    int file_size;        /* How large is the file*/
    int curr_block;       /* Which block are we reading from*/
    int data_block_index; /* Which data block are we looking at*/

    if(getFileDescriptorIndex(fs.file_descriptors[FD].name)) /* Check for valid FD*/
        return -1; /* File not open*/

    fd_inode = fs.file_descriptors[FD].inode;
    curr_position = fs.file_descriptors[FD].seek_position;
    file_size = fs.inode_blocks[fd_inode].st_size;

    if(curr_position == file_size) /* We are at the end of the file*/
        return -1; /* Return EOF*/

    /* ex. if curr pos = 700  then 200 / 256 = 2. so data is in block index 2, (data block 3)*/
    curr_block = curr_position / BLOCKSIZE_;     /* Get the block index number of the inode.*/
    block_position = curr_position % BLOCKSIZE_; /* Get location of byte to read*/
    data_block_index = fs.inode_blocks[fd_inode].data_indexs[curr_block]; /* Get index of data block we want to read*/
    
    *buffer = fs.data_blocks[data_block_index].data[block_position];

    clock_gettime(CLOCK_REALTIME, &current_time);
    
    if(verbosity){
        printf("... Bonus Updating Access Time ...");
        print_timespec("Last Time of Access: ", &(fs.inode_blocks[fd_inode].st_atim));
    }

    sleep(3); // Add a 5-second delay
    
    clock_gettime(CLOCK_REALTIME, &current_time);
    fs.inode_blocks[fd_inode].st_atim = current_time; /* Update access time*/

    if(verbosity){
        printf("... Access Time Updated ...");
        print_timespec("Last Time of Access: ", &(fs.inode_blocks[fd_inode].st_atim));
    }

    
    return 0;
}

/* Change the file position offset (absolute). Returns success/error codes.*/
/* Returns 0 upon success
   Returns -1 when offset is to large*/
int tfs_seek(fileDescriptor FD, int offset){
    u_int8_t inode_index;
    
    if(offset > BLOCKSIZE_ * DATA_BLKS_PER_INODE_)
        return -1;

    inode_index = getInodeNamePairIndex(fs.file_descriptors[FD].name); /* Get the index for inode name pair*/
    
    fs.file_descriptors[FD].seek_position = offset; /* Set the seek position of FD to offset*/
    fs.special_inode_list[inode_index].seek_position = offset; /* Set the seek position of inode name pair*/

    return 0;
}


int processMetaData(void){
    char block_buffer[BLOCKSIZE_] = {'\0'};
    int i, j, data_index;
    int inode, block_type;
    fileDescriptor fd = fs.disk_info.fd;
    char *filename = fs.disk_info.disk_name;
    FreeBlock *currHead = NULL;


    if(verbosity){
        printf("-------------------- Processing Meta Data --------------------\n\n");
        printf("--------- Copying Inode Name Block From Disk -----\n\n");
    }

    /* Read the special inode name pair data from disk*/
    if(readBlock(fd, INO_NAME_BLK_INDEX_, (void *)block_buffer) != 0){
        sprintf(block_buffer, "!!! readBlock failed when reading INODE NAME PAIR BLOCK from file %s.\n", filename);
        errorout(block_buffer);
    }

    if(verbosity)
        printf("... Copying Inode Name Info To File System Memory ...\n");
    memcpy(&(fs.special_inode_list), block_buffer, BLOCKSIZE_); // Need to test
    if(verbosity){
        printf("Copied %d byte(s) to inode name block from disk: %s.\n", BLOCKSIZE_, filename);
        printf("[INODE NAME DATA]:\n");
        if(isBlockEmpty((uint8_t *)block_buffer)) /* Case where disk wasn't initilized with tfs_mkfs*/
            printf("\b EMPTY\n");
        else
            printInodeNameBlock();
            // printHexDump(block_buffer, BLOCKSIZE_);
    }


    if(verbosity)
        printf("--------- Retrieving Inode Blocks From Disk ---------\n\n");

    for(i = 0; i < NUM_INODES_; i++){
        memset(block_buffer, 0, BLOCKSIZE_); /* Clear block_buffer*/
        if(readBlock(fd, INODE_BLOCK_INDEX_ + i, (void *)block_buffer) != 0){
            sprintf(block_buffer, "readBlock failed when reading INODE BLOCK %d from file %s.\n", i, filename);
            errorout(block_buffer);
        }
        if(verbosity)
            printf("... Copying Inode Information To File System Memory ...\n");
        memcpy(&(fs.inode_blocks[i]), block_buffer, BLOCKSIZE_); // Need to test
        if(verbosity){
            printf("Copied %d byte(s) to inode block %d from disk: %s.\n", BLOCKSIZE_, i, filename);
            printf("[DATA]: ");
            if(fs.inode_blocks[i].st_ino == UNUSED_INODE_)
            // if(isBlockEmpty((uint8_t *)block_buffer))
                printf("INODE BLOCK EMPTY\n\n");
            else{
                printf("\n");
                // printHexDump(block_buffer, BLOCKSIZE_); /* For testing only. To much terminal output otherwisen*/
                printInodeBlock(i, fs.inode_blocks[i]);
            }
        }
    }

    // Moved bitmaps to super block
    // if(verbosity)
    //     printf("------------ Building Inode Bitmaps --------------\n\n");

    // for(i = 0; i < MAX_INO_NAME_PAIRS_; i++){
    //     if(fs.special_inode_list[i].inode != UNUSED_INODE_){
    //         fs.available_inodes[i] = ALLOCATED;
    //         if(verbosity)
    //             printf("[UPDATED INODES BITMAP]: Inode %d Is Marked As ALLOCATED.\n", i);
    //     }
    //     else{
    //         fs.available_inodes[i] = AVAILABLE;
    //         if(verbosity)
    //             printf("[UPDATED INODES BITMAP]: Inode %d Is Marked As AVAILABLE.\n", i);
    //     }
    // }




    if(verbosity)
        printf("--------- Copying Data Blocks From Disk ----------\n\n");

    for(i = 0; i < NUM_DATA_BLOCKS_; i++){
        memset(block_buffer, 0, BLOCKSIZE_); /* Clear block_buffer*/
        if(readBlock(fd, DATA_BLOCK_INDEX_ + i, (void *)block_buffer) != 0){
            sprintf(block_buffer, "readBlock failed when reading DATA BLOCK %d from file %s.\n", i, filename);
            errorout(block_buffer);
        }
        if(verbosity)
            printf("... Copying Data Information To File System Memory ...\n");
        memcpy(&(fs.data_blocks[i]), block_buffer, BLOCKSIZE_); // Need to test
        if(verbosity){
            printf("Copied %d byte(s) to data block %d from disk: %s.\n", BLOCKSIZE_, i, filename);
            printf("[DATA]: ");
            if(isBlockEmpty((uint8_t *)block_buffer))
                printf("EMPTY\n\n");
            else
                printHexDump(block_buffer, BLOCKSIZE_);
        }
    }





    // Moved bitmaps to super block

    // if(verbosity)
    //     printf("--------- Building Data Block Bitmap -------------\n\n");

    // /* Fist check inode bitmap for allocated inodes. As only allocated inodes should have allocated data*/
    // for(i = 0; i < MAX_INO_NAME_PAIRS_; i++){ /* Iterate through available inodes bitmap*/
    //     inode = fs.available_inodes[i]; /* Get allocated inodes*/
    //     if(inode == ALLOCATED){         /* If allocated look start to populate data block bitmap*/
    //         if(verbosity){
    //             printf("[INODE %d] Allocated.\n", i);
    //             printf("... Checking Inode %d's Data Block Allocation ...\n");
    //         }
    //         for(j = 0; j < DATA_BLKS_PER_INODE_; j++){ /* Interate through inode block's data indexs*/
    //             data_index = fs.inode_blocks[inode].data_indexs[j]; /* Get inode's data block index*/
    //             if(data_index != UNUSED_DATA_INDEX_){ /* If data block have been set*/
    //                 fs.available_data_blocks[data_index] = ALLOCATED; /* Update the bitmap*/
    //                 if(verbosity)
    //                     printf("[UPDATED DATA BLOCK BITMAP]: Data Block %d Is Marked As ALLOCATED.\n", i);
    //             }
    //             else{ /* If the data block has not been set.*/
    //                 fs.available_data_blocks[data_index] = AVAILABLE; /* Update the bitmap. Probably not necessary*/
    //                 if(verbosity)
    //                     printf("[UPDATED DATA BLOCK BITMAP]: Data Block %d Is Marked As AVAILABLE.\n", i);
    //             }
    //         }
    //     }
    //     else{
    //         if(verbosity)
    //             printf("[INODE %d] Not Allocated.\n", i);
    //     }
    // }

    if(verbosity)
        printf("--------- Copying Free Blocks From Disk ----------\n\n");

    currHead = fs.free_blocks_head;
    for(i = 0; i < NUM_FREE_BLOCKS_; i++){
        if(currHead == NULL)
            errorout("CurrHead still NULL when copying Free Blocks from Disk");

        memset(block_buffer, 0, BLOCKSIZE_); /* Clear block_buffer*/

        if(readBlock(fd, FREE_BLOCK_INDEX_ + i, (void *)block_buffer) != 0){
            sprintf(block_buffer, "readBlock failed when reading FREE BLOCK %d from file %s.\n", i, filename);
            errorout(block_buffer);
        }

        if(verbosity)
            printf("... Copying Free Block Information To File System Memory...\n");

        block_type = fs.superblock.free_block_type[i];

        if(block_type == INODE){     /* Inode type found in the bitmap*/
            currHead->inode_block = (iNode *)safeMalloc(sizeof(iNode));
            memcpy(currHead->inode_block, block_buffer, BLOCKSIZE_); // Need to test
            currHead->type = INODE;
        }
        else if(block_type == DATA){ /* Data type found in the bitmap*/
            currHead->data_block = (DataBlock *)safeMalloc(sizeof(DataBlock));
            memcpy(currHead->data_block->data, block_buffer, BLOCKSIZE_); // Need to test
            currHead->type = DATA;
        }
        else /* Unallocated*/
            currHead->type = UNALLOCATED;
        
        if(verbosity){
            printf("Copied %d byte(s) to free block %d from disk: %s.\n", BLOCKSIZE_, i, filename);
            printf("[DATA]: ");
            if(block_type == INODE){
                if(currHead->inode_block->st_ino == UNUSED_INODE_)
                    printf("INODE BLOCK EMPTY\n\n");
                else{
                    printf("\n");
                    // printHexDump(block_buffer, BLOCKSIZE_); /* For testing only. To much terminal output otherwisen*/
                    printInodeBlock(i, *(currHead->inode_block));
                }
            }
            else if(block_type == DATA){
                if(isBlockEmpty((uint8_t *)(currHead->data_block->data)))
                    printf(" EMPTY\n\n");
                else{
                    printf("\n");
                    printHexDump(currHead->data_block->data, BLOCKSIZE_);
                }
            }
            else
                printf("Unallocated\n\n");
        }
        
        currHead->position = i;   /* Set position of free block realative to the bitmap*/
        
        currHead = currHead->next; /* Move to the next node*/
    }
   
    
    /* Not implemented correctly below. Should be able to reuse some of the code though. Bascailly need to
        determine the type of free block at i, then read using that type of buffer. Then have the linked list point
        at it. */ 
    // for(i = 0; i < NUM_FREE_BLOCKS_; i++){
    //     memset(block_buffer, 0, BLOCKSIZE_); /* Clear block_buffer*/
    //     if(readBlock(fd, FREE_BLOCK_INDEX_, (void *)block_buffer) != 0){
    //         sprintf(block_buffer, "readBlock failed when reading FREE BLOCK %d from file %s.\n", i, filename);
    //         errorout(block_buffer);
    //     }
    //     if(verbosity)
    //         printf("... Copying Free Block Information To File System Memory...\n");
    //     memcpy(&(fs.data_blocks[i]), block_buffer, BLOCKSIZE_); // Need to test
        // if(verbosity){
        //     printf("Copied %d byte(s) to free block %d from disk: %s.\n", BLOCKSIZE_, i, filename);
        //     printf("[DATA]:\n");
        //     if(isBlockEmpty(block_buffer))
        //         printf("\b EMPTY\n");
        //     else
        //         printHexDump(block_buffer, BLOCKSIZE_);
    //     }
    // }



    return 0;
}

/* Adds the inode name pair to the superblock
   Return special inode name pair index
   Return -1 on failure*/
int addInodeNamePair(char *filename, uint8_t inode){
    int index;

    if(verbosity)
        printf("---------- Adding Inode Name Pair. File: %s | Inode: %d ----------\n", filename, inode);

    if(fs.superblock.inode_name_pair_count >= MAX_INO_NAME_PAIRS_){
        printf("!!!! OUT OF INODES!!!\n--- Delete A File And Try Again.\n");
        return - 1;
    }

    for(index = 0; index < MAX_INO_NAME_PAIRS_; index++){
        if((strlen(fs.special_inode_list[index].name) == 0) && fs.special_inode_list[index].inode == INVALID_INODE_){
            if(verbosity){
                printf("--- New Inode Name Pair Entry:\n");
                printf("[ADDED FILE]: %s\n[ADDED INODE]: %d\n[INODE NAME INDEX]: %d\n", filename, inode, index);
            }
            strcpy(fs.special_inode_list[index].name, filename);
            fs.special_inode_list[index].inode = inode;
            fs.superblock.inode_name_pair_count++;
            return inode;
            }
    }
        
    return -1;
}

/* Uses filename to find the index for the matching filename
   Returns positive number inode pair indexes. 
   If larger than NUM_INODES_ (5) then subtract NUM_INODES_ to find free block position of inode
   Return -1 if no filename match occurs.*/
int getInodeNamePairIndex(char *filename){
    int pairIndex;

    if(verbosity)
        printf("\n------------- Searching For %s's Inode -------------\n", filename);

    for(pairIndex = 0; pairIndex < MAX_INO_NAME_PAIRS_; pairIndex++){
        if(strcmp(fs.special_inode_list[pairIndex].name, filename) == 0){
            if(verbosity)
                printf("File %s Found In Special Inode List!\n[INODE]: %d\n", filename, pairIndex);
            return pairIndex;
        }
    }

    if(verbosity)
        printf("!!! Inode Name Pair Not Found !!!\n");

    return -1; /* No Inode Name found*/
}

// Unnecessary 
// /* Checks if the disk has been mounted */
// int isDiskMounted(char *filename){
//     int disk_index;
//     int file_found = -1;

//     /* Check to see if the file has been opened before trying to mount.*/
//     for(disk_index = 0; disk_index < MAX_NUM_DISKS_; disk_index++){
//         file_found = strcmp(filename, DiskList[disk_index].disk_name);
//         if(file_found == 0 && DiskList[disk_index].status == MOUNTED)   /* File was found already mounted*/
//             errorout("Disk Is Already Mounted.\n");                       /*  then error out*/
//         else if(file_found == 0 && DiskList[disk_index].status == OPEN) /* File was found and open.*/
//             break;                                                     /* Finish searching*/
//         else                                                           /* File not found open.*/
//             file_found = -1;
//     }

//     return disk_index;
// }



/* Finds the first available inode in the file system*/
/* Returns index of avaialbe inode in fs.inode_blocks[]*/
/* Returns INVALID_INODE_ if none are found*/
uint8_t findAvailableInode(void){
    int block_num;

    if(verbosity)
        printf("\n-------- Searching For An Available Inode Block --------\n");
        
    for(block_num = 0; block_num < NUM_INODES_; block_num++){
        if(fs.inode_blocks[block_num].st_ino == UNUSED_INODE_){
            if(verbosity)
                printf("Available Inode Block found!\n[INODE BLOCK]: %d\n\n", block_num);
            return block_num;
        }
    }

    if(verbosity)
        printf("!!!! No Inode Blocks Left. Delete Some Files.\n");

    return INVALID_INODE_; /* No available inodes found*/
}

/* Finds the first available data block in the file system
   Returns index of a avaialbe data block in fs.data_blocks[]
   Returns INVALID_INODE_ if none are found*/
uint8_t findAvailableDataBlock(void){
    int data_block_num;
    int inode_num;
    if(verbosity)
        printf("-------- Searching For An Available Data Block --------\n");
        
    for(inode_num = 0; inode_num < NUM_INODES_; inode_num++){
        if(verbosity)
            printf("... Checking Inode %d's Data Blocks ...\n", inode_num);
        for(data_block_num = 0; data_block_num < NUM_DATA_BLOCKS_; data_block_num++){
            if(fs.inode_blocks[inode_num].data_indexs[data_block_num] == UNUSED_DATA_INDEX_){
                if(verbosity)
                    printf("Available Data Block found!\n[DATA BLOCK]: %d\n", data_block_num);
                return data_block_num;
            }
        }
    }

    if(verbosity)
        printf("!!!! No Inode Blocks Left. Delete Some Files.\n");

    return INVALID_DATA_INDEX_; /* No available inodes found*/
}

/* Checks for a filename match in fs.file_descriptors array.*/
/* Returns the fileDescriptor index if the file is open*/
/* Returns -1 if the file is not found to be open*/
fileDescriptor getFileDescriptorIndex(char *filename){
    fileDescriptor fd;

    if(verbosity)
        printf("-------- Searching For %s's File Descriptor --------\n", filename);

    for(fd = 0; fd < MAX_INO_NAME_PAIRS_; fd++){
        if(strcmp(fs.file_descriptors[fd].name, filename) == 0){
            if(verbosity)
                printf("File %s Found In FS's File Descriptor List!\n[FILE DESCRIPTOR]: %d\n", filename, fd);
            return fd;
        }
    }

    if(verbosity)
        printf("File Not found with a File Descriptor! Open file first\n");

    return -1; /* No file descriptor found*/
}


/* Adds the inode name pair to the FileDescriptor list
   Return FileDescriptor index on success
   Return -1 on failure*/
int addFileDescriptor(char *filename, uint8_t inode){
    int index;

    if(verbosity)
        printf("---------- Adding File Descriptor For: File: %s | Inode: %d ----------\n", filename, inode);

    if(fs.file_descriptor_count >= MAX_INO_NAME_PAIRS_){
        printf("!!!! OUT OF FILE DESCRIPTORS!!!\n--- Delete A File And Try Again.\n");
        return - 1;
    }

    for(index = 0; index < MAX_INO_NAME_PAIRS_; index++){
        if((strlen(fs.file_descriptors[index].name) == 0) && fs.file_descriptors[index].inode == UNUSED_INODE_){
            if(verbosity){
                printf("--- New File Descriptor Entry:\n");
                printf("[ADDED FILE]: %s\n[ADDED INODE]: %d\n[FILE DESCRIPTOR]: %d\n", filename, inode, index);
            }
            strcpy(fs.file_descriptors[index].name, filename);
            fs.file_descriptors[index].inode = inode;
            fs.file_descriptor_count++;
            return inode;
            }
    }
        
    return -1;
}

/* Looks for a open file descriptor in the fs.file_descriptors array.*/
/* Return open file descriptor if one is open*/
/* Returns -1 if no file descriptors are open*/
fileDescriptor getNextOpenFD(void){
    int openFd;

    if(verbosity)
        printf("-------- Searching For Unused File Descriptor --------\n");

    for(openFd = 0; openFd < MAX_INO_NAME_PAIRS_; openFd++){
        if(fs.file_descriptors[openFd].inode == INVALID_INODE_ && 
           (fs.file_descriptors[openFd].name[0] == '\0')){ /* No file name and invaid Inode means fd is available*/
        //    (strlen(fs.file_descriptors[openFd].name) == 0)){ /* No file name and invaid Inode means fd is available*/
            if(verbosity)
               printf("[FILE DESCRIPTOR AVAILABLE]: %d.\n",openFd);
            return openFd;
        }
    }
    
    if(verbosity)
        printf("!!!! NO FILE DESCRIPTORS AVAILABLE !!!!");

    return -1; /* No open file descriptors found*/
}

/* Checks if the block buffer is empty*/
/* Returns 1 if the block buffer is empty*/
/* Returns 0 if the block buffer is not empty*/
int isBlockEmpty(uint8_t *block_buffer){
    int i;
    
    for(i = 0; i < BLOCKSIZE_; i++)
        if(block_buffer[i] != '\0')
            return 0; /* Buffer is not empty*/

    return 1; /* buffer block is empty*/
}


void initFileSystem(void)
{
    if(verbosity)
        printf("... Initilizing File System ...\n");
    initDiskInfo(&(fs.disk_info));
    initFileDescriptors(fs.file_descriptors);
    fs.file_descriptor_count = 0;
    initSuperblock(&(fs.superblock));
    initInodeNamePairs(fs.special_inode_list);
    initInodeBlocks(fs.inode_blocks);
    initDataBlocks(fs.data_blocks);
    initFreeBlocks();
    if(verbosity)
        printf("... File System Initialized ...\n");
    return;
}


void initFileDescriptors(iNodeNamePair *file_descriptors){
    int i;
    for(i = 0; i < MAX_INO_NAME_PAIRS_; i++){
        // strcpy(file_descriptors[i].name, "");      /* Empty string*/
        memset(file_descriptors[i].name, 0, MAX_FILENAME_SIZE_); /* Clear name array*/
        file_descriptors[i].inode = UNUSED_INODE_; // Set to invalid Inode number
    }

}


void initSuperblock(Superblock *sb){
    int i;

    sb->magic_num             = 0;
    sb->special_bNum          = 0;
    sb->disk_size             = 0;
    sb->inode_name_pair_count = 0;
    sb->inode_count           = 0;
    sb->data_blocks_count     = 0;
    sb->free_blocks_count     = 0;
    
    for(i = 0; i < NUM_INODES_; i++)
        sb->available_inodes[i] = AVAILABLE;
    for(i = 0; i < NUM_DATA_BLOCKS_; i++)
        sb->available_data_blocks[i] = AVAILABLE;
    for(i = 0; i < NUM_FREE_BLOCKS_; i++)
        sb->free_block_type[i] = UNALLOCATED;
    
    return;
}

void initInodeNamePairs(iNodeNamePair *pair_list){
    int i;

    for(i = 0; i < MAX_INO_NAME_PAIRS_; i++){
        // strcpy(pair_list[i].name, "");      /* Empty string*/
        memset(pair_list[i].name, 0, MAX_FILENAME_SIZE_); /* Clear name array*/
        pair_list[i].inode = UNUSED_INODE_; /* Set to invalid Inode number*/
        pair_list[i].seek_position = 0;     /* Starting location*/
    }
    return;
}

void initInodeBlocks(iNode *inode_list){
    int i, j;

    for(i = 0; i < MAX_INO_NAME_PAIRS_; i++){
        initInode(&(inode_list[i]));
    }
    return;
}

void initInode(iNode *new_inode){
    int i;

    new_inode->st_dev = TINY_FS_DEV_NO;
    new_inode->st_ino = UNUSED_INODE_;
    for(i = 0; i < DATA_BLKS_PER_INODE_; i++)
        new_inode->data_indexs[i] = UNUSED_DATA_INDEX_;
    new_inode->st_mode = 0;
    new_inode->st_nlink = 0;
    new_inode->st_uid = 0;
    new_inode->st_gid = 0;
    new_inode->st_rdev = 0;
    new_inode->st_size = 0;
    new_inode->st_blksize = BLOCKSIZE_;
    new_inode->st_blocks = 0;
    new_inode->st_atim.tv_sec = 0;
    new_inode->st_atim.tv_nsec = 0;
    new_inode->st_mtim.tv_sec = 0;
    new_inode->st_mtim.tv_nsec = 0;
    new_inode->st_ctim.tv_sec = 0;
    new_inode->st_ctim.tv_nsec = 0;

    return;
}

void initDataBlocks(DataBlock *data_block_list){
    int i;
    for(i = 0; i < NUM_DATA_BLOCKS_; i++)
        memset(data_block_list[i].data, 0, BLOCKSIZE_);
        // strcpy(data_block_list[i].data, "");

    return;
}

// free_block_list is a char[] in file system. May change to linked list of void*
// void initFreeBlocks(char *free_block_list){
//     strcpy(free_block_list, "");
//     return;
// }

// void initAvailableBlocks(uint8_t *available_inodes, uint8_t *available_data_blocks){
//     int i;

//     for(i = 0; i < NUM_INODES_; i++)
//         available_inodes[i] = AVAILABLE;
    
//     for(i = 0; i < NUM_DATA_BLOCKS_; i++)
//         available_data_blocks[i] = AVAILABLE;

//     return;
// }


void initDiskList(void){
    int i;
    for(i = 0; i < MAX_NUM_DISKS_; i++)
        initDiskInfo(&(DiskList[i]));
    
    return;
}

void initDiskInfo(DiskInfo *disk_info){
    
    memset(disk_info->disk_name, 0, BLOCKSIZE_); /* Clear Name*/
    // strcpy(disk_info->disk_name, ""); /* Empty string*/
    disk_info->fd = -1;
    disk_info->disk_size = 0;
    disk_info->status = CLOSED;
    return;
}


void setDiskInfo(DiskInfo* disk_info, char* disk_name, fileDescriptor fd, size_t disk_size, DiskStatus status){
    //disk_info = (DiskInfo*) safeMalloc(sizeof(DiskInfo));
    int strLen = strlen(disk_name);
    if(strLen != 0 && strLen < MAX_DISKNAME_SIZE_)
        strcpy(disk_info->disk_name, disk_name);
    disk_info->fd = fd;
    disk_info->disk_size = disk_size;
    disk_info->status = status;
}

void setTimestamps(iNode *inode){
    struct timespec current_time;
    
    clock_gettime(CLOCK_REALTIME, &current_time);
    
    inode->st_atim = current_time;
    inode->st_mtim = current_time;
    inode->st_ctim = current_time;
}

void printFileSystemInfo(void){
    int bNum;

    printDiskInfo(&fs.disk_info);
    printFileDescriptors();    
    printSuperBlock();
    printInodeNameBlock();
    for(bNum = 0; bNum < NUM_INODES_; bNum++){
        if(fs.inode_blocks[bNum].st_ino != UNUSED_INODE_)
            printInodeBlock(bNum, fs.inode_blocks[bNum]);
        else
            printf("[INODE]: Unused.\n");
    }
    for(bNum = 0; bNum < NUM_INODES_; bNum++){
        if(isBlockEmpty((uint8_t *)(fs.data_blocks[bNum].data)))
            printf("EMPTY\n");
        else
            printHexDump(&(fs.data_blocks[bNum]), BLOCKSIZE_);
    }
    for(bNum = 0; bNum < NUM_INODES_; bNum++){
        printf("Add free block print here");
    }
    
    return;
}

void printDiskList(void){
    int i;

    for(i = 0; i < MAX_NUM_DISKS_; i++){
        printf("[DISK %d]:\n", i);
        printDiskInfo(&(DiskList[i]));
    }
    return;
}

void printDiskInfo(const DiskInfo* disk_info){
    const char *statusString = disk_info->status == CLOSED ? "CLOSED" :
                               disk_info->status == OPEN ? "OPEN" :
                               disk_info->status == MOUNTED ? "MOUNTED" :
                               disk_info->status == UNMOUNTED ? "UNMOUNTED" :
                               "UNKNOWN";
    printf("-------------------------------------- Disk Info ---------------------------------------------\n");
    printf("DiskInfo: disk_name: %s | fileDescriptor: %d | disk_size: %zu | status: %s (%d)|\n", 
    disk_info->disk_name,
    disk_info->fd,
    disk_info->disk_size,
    statusString,
    disk_info->status);
    printf("----------------------------------------------------------------------------------------------\n\n");
}

void printSuperBlock(void){
    int i;

    printf("---------------------------- Super Block ----------------------------\n");
    printf("Magic Number: 0x%02X\n", fs.superblock.magic_num);
    printf("Special Block Number: %d\n", fs.superblock.special_bNum);
    printf("Disk Size: %zu\n", fs.superblock.disk_size);
    printf("Inode Name Pair Count: %hhu\n", fs.superblock.inode_name_pair_count);
    printf("Inode Count: %hhu\n", fs.superblock.inode_count);
    printf("Data Blocks Count: %hhu\n", fs.superblock.data_blocks_count);
    printf("Free Blocks Count: %hhu\n", fs.superblock.free_blocks_count);
    printf("Available Inode Bitmap:\n");
    for(i = 0; i < NUM_INODES_; i++){
        if(fs.superblock.available_inodes[i] == ALLOCATED)
            printf("[INODE %d]: ALLOCATED.\n", i);
        else
            printf("[INODE %d]: AVAILABLE.\n", i);
    }
    printf("Available Data Bitmap:\n");
    for(i = 0; i < NUM_DATA_BLOCKS_; i++){
        if(fs.superblock.available_data_blocks[i] == ALLOCATED)
            printf("[DATA BLOCK %d]: ALLOCATED.\n", i);
        else
            printf("[DATA BLOCK %d]: AVAILABLE.\n", i);
    }
    printf("Free Block Bitmap:\n");
    for(i = 0; i < NUM_FREE_BLOCKS_; i++){
        if(fs.superblock.free_block_type[i] == UNALLOCATED)
            printf("[FREE BLOCK %d TYPE]: UNALLOCATED.\n", i);
        else if(fs.superblock.free_block_type[i] == INODE)
            printf("[FREE BLOCK %d TYPE]: INODE.\n", i);
        else if(fs.superblock.free_block_type[i] == DATA)
            printf("[FREE BLOCK %d TYPE]: DATA.\n", i);
    }
    printf("---------------------------------------------------------------------\n\n");
    
    return;
}

void printFileDescriptors(void){
    int i;

    printf("------------------------- File Descriptor Block Info ----------------\n");
    for(i = 0; i < MAX_INO_NAME_PAIRS_; i++){
        // if(strlen(fs.special_inode_list[i].name[0]) == 0 && fs.special_inode_list[i].inode == UNUSED_INODE_)
        if(fs.special_inode_list[i].name[0] == '\0' && fs.special_inode_list[i].inode == UNUSED_INODE_)
            printf("[FILE DESCRIPTOR %d]: Unused.\n", i);
        else
            printf("[FILE DESCRIPTOR %d]: In Use.\n", i);
    }
    printf("---------------------------------------------------------------------\n\n");

    return;

}

void printInodeNameBlock(void){
    int i;

    printf("------------------------- Inode Name Block Info ---------------------\n");
    for(i = 0; i < MAX_INO_NAME_PAIRS_; i++){
        printf("[PAIR %d]: ", i);
        printf("Filename: %s\n", fs.special_inode_list[i].name[0] == '\0' ? "<EMPTY>" : fs.special_inode_list[i].name);
        printf("[PAIR %d]: ", i);
        if(fs.special_inode_list[i].inode == UNUSED_INODE_)
            printf("Inode: UNUSED.\n");
        else
            printf("Inode: %hhu.\n", fs.special_inode_list[i].inode);
    }
    printf("---------------------------------------------------------------------\n\n");

    return;
}


void printInodeBlock(int bNum, iNode block){
    int i;
    printf("------------------------- Inode Block %d Info ------------------------\n", bNum);
    printf("st_dev: %ju\n", block.st_dev);
    printf("st_ino: %ju\n", block.st_ino);
    for (i = 0; i < DATA_BLKS_PER_INODE_; i++)
        printf("data_indexs[%d]: %hhu\n", i, block.data_indexs[i]);
    printf("st_mode: %d\n", block.st_mode);
    printf("st_nlink: %ju\n", block.st_nlink);
    printf("st_uid: %d\n", block.st_uid);
    printf("st_gid: %d\n", block.st_gid);
    printf("st_rdev: %ju\n", block.st_rdev);
    printf("st_size: %ld\n", block.st_size);
    printf("st_blksize: %ld\n", block.st_blksize);
    printf("st_blocks: %ld\n", block.st_blocks);
    printf("st_atim: %ld.%09ld\n", block.st_atim.tv_sec, block.st_atim.tv_nsec);
    printf("st_mtim: %ld.%09ld\n", block.st_mtim.tv_sec, block.st_mtim.tv_nsec);
    printf("st_ctim: %ld.%09ld\n", block.st_ctim.tv_sec, block.st_ctim.tv_nsec);
    printf("---------------------------------------------------------------------\n\n");

    return;
}

void printFreeBlock(int bNum){
    int block_type, i;
    FreeBlock *currBlock = fs.free_blocks_head;

    for(i = 0; i < bNum; i++)
        currBlock = currBlock->next;

    block_type = currBlock->type;
    printf("------------------------- Free Block %d Info -------------------------\n", bNum);
    if(block_type == INODE)
        printInodeBlock(currBlock->position, *(currBlock->inode_block));
    else if(block_type == DATA)
        printHexDump(currBlock->data_block, BLOCKSIZE_);
    else
        printf("UNALLOCATED");
    printf("---------------------------------------------------------------------\n\n");

    return;
}

void print_timespec(char* time_label, struct timespec* timespec){
    char time_string[30];
    time_t seconds = timespec->tv_sec;
    struct tm* timeinfo = localtime(&seconds);
    
    strftime(time_string, sizeof(time_string), "%c", timeinfo);
    
    printf("%s: %s\n", time_label, time_string);
}
