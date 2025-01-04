
#include "libDisk.h"
#include "libTinyFS.h"

#include <fcntl.h>

DiskInfo DiskList[MAX_NUM_DISKS_];
FileSystem fs;
int verbosity = 1;

void putBLOCKSIZE(char* block);
void testOpenDisk();
void failTests(void);
void testHexDump(void);


int main(int argc, char *argv[]){
    fileDescriptor fd;
    char* filename = "file2.txt";
    char block[BLOCKSIZE_ + 1];
    char write[BLOCKSIZE_ + 1];
    char readByteChar;

    initDiskList();
    char *smallMsg = "This is a test to write data to the first data block";

    // testOpenDisk();
    // testHexDump();
    // tfs_mkfs("deleteme.txt", DEFAULT_DISK_SIZE_); /* Demo to show that you can create a new file*/
    
    
    openDisk("deleteme.txt", DEFAULT_DISK_SIZE_);    /* Demo to show that you can open a disk*/
    tfs_mount("deleteme.txt");                       /* Demo to show that you can mount a disk*/
    
    fd = tfs_open("Hellotxt");
    tfs_write(fd, smallMsg, strlen(smallMsg));
    printf("Wrote message.\n");

    tfs_seek(fd, 2); /* Should move to 'i' in "This" */
    tfs_readByte(fd, &readByteChar); /* Should store 'i' in readByte*/

    printf("Should return 'i': %c.\n", readByteChar);

    closeDisk(fs.disk_info.fd); /* Will close and commit file*/
    
    // printf("File Descriptor found: %d\n", tfs_open("Hello_World.txt")); /* For testing*/
    // initFileSystem();


    // fileDescriptor disk = openDisk(filename, BLOCKSIZE_);
    // // fileDescriptor disk = safeOpen(filename, O_RDWR, S_IRUSR | S_IWUSR);

    // writeBlock(disk, 0, write);
    // writeBlock(disk, 10, write);
    // printf("write: %s \n", write);

    // readBlock(disk, 0, block);
    // printf("block: %s \n", block);

    // readBlock(disk, 5, block);
    // printf("block: %s \n", block);

    // readBlock(disk, 10, block);
    // printf("block: %s \n", block);

    // // testing init structs
    // DiskInfo* disk_info = NULL;
    
    // printf("\n\n");
    // setDiskInfo(disk_info, "Disk Info 1", 0, 0, CLOSED);
    // printDiskInfo(disk_info);

    return 0;
}


void testOpenDisk() {
    /* Test case 1: Test with a non-existent file and positive nBytes value*/
    char* filename1 = "testfile1.txt";
    int nBytes1 = BLOCKSIZE_;
    fileDescriptor fd1 = openDisk(filename1, nBytes1);
    printf("File: %s opened.\n", filename1);

    /* Test case 2: Test with an existing file and positive nBytes value*/
    char* filename2 = "testfile2.txt";
    int nBytes2 = BLOCKSIZE_;
    fileDescriptor fd2 = openDisk(filename2, nBytes2);
    printf("File: %s opened.\n", filename2);

    /* Test case 3: Test with a file and nBytes set to 0*/
    char* filename3 = "testfile3.txt";
    int nBytes3 = 0;
    fileDescriptor fd3 = openDisk(filename3, nBytes3);
    printf("File: %s opened.\n", filename3);

}

void failTests(void){
    /* Fail Test case 1: Test with a negative nBytes value*/
    char* filename1 = "testfile1.txt";
    int nBytes1 = -1;
    fileDescriptor fd1 = openDisk(filename1, nBytes1);

    /* Fail Test case 2: Test with an empty filename*/
    char* filename2 = ""; /* Empty filename*/
    int nBytes2 = BLOCKSIZE_;
    fileDescriptor fd2 = openDisk(filename2, nBytes2);

    /* Fail Test case 3: Test with NULL filename*/
    char* filename3 = NULL; /* NULL filename*/
    int nBytes3 = BLOCKSIZE_;
    fileDescriptor fd3 = openDisk(filename3, nBytes3);

    /* Test case 7: Test with nBytes not divisible by BLOCKSIZE*/
    char* filename4 = "testfile1.txt";
    int nBytes4 = BLOCKSIZE_ + 1;
    fileDescriptor fd4 = openDisk(filename4, nBytes4);
}

void testHexDump(void){
    unsigned char sampleData[] = {
        0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x57,
        0x6F, 0x72, 0x6C, 0x64, 0x21, 0x20, 0x54, 0x68,
        0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x74, 0x68,
        0x65, 0x20, 0x74, 0x69, 0x6E, 0x79, 0x46, 0x53,
        0x64, 0x65, 0x6D, 0x6F, 0x27, 0x73, 0x20, 0x68,
        0x65, 0x78, 0x64, 0x75, 0x6D, 0x70, 0x65, 0x72
    };
    
    size_t dataSize = sizeof(sampleData);

    printf("Sample Data:\n");
    printHexDump(sampleData, dataSize);
    return;
}