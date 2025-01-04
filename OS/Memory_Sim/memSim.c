#include "options.h"
#include "safeutil.h"
#include "address.h"
#include "tlb.h"
#include "page_table.h"

// NOTE: <FRAME> = num_frames + 1: save 8 bits :>

#define BUFFER_     1024
#define BIN_BUFFER_ 65536
/**
 * @fn	   : readFile
 * @brief  : 
 * @retval : void
 * @parms  : FILE *file_ptr, unsigned int *address_list, unsigned int *num_addresses
 */
void readFile(FILE *file_ptr, unsigned int **address_list, unsigned int *address_count){
    //int i;
    unsigned int address;
    uint16_t running_count = 0;
    size_t buffer_size = BUFFER_;
    unsigned int *addresses = NULL;

    addresses = (unsigned int *)safeMalloc(sizeof(unsigned int) * buffer_size); /* Make space*/

    errno = 0;
    while(!feof(file_ptr)){ /* While not at EOF*/
        address = 0; /* Clear address*/
        if(fscanf(file_ptr, "%u", &address) == 1){
            if(!feof(file_ptr) || address != 0){
                if(verbosity)
                    printf("Address: %u\n", address);
                addresses[running_count++] = address; /* Store address*/
            }
            if(running_count >= (buffer_size - 1)){ /* Need more space*/
                addresses = (unsigned int *)safeRealloc(addresses, buffer_size + BUFFER_, buffer_size, sizeof(unsigned int));
                buffer_size += BUFFER_; /* Increase buffer size*/
            }
        } /* Scan for addresses*/
        else if(!feof(file_ptr))
            errorout("readFile failed. Something in address file is not an ASCII integer\n");
    }
    
    if(errno) /* Something went wrong*/
        errorout("ReadFile failed.\n");

    if(verbosity)
    if(verbosity)
        printf("Total number of addresses: %u\n", running_count);
    
    *address_count = running_count; /* Save job count*/

    *address_list = (unsigned int *)safeMalloc(sizeof(unsigned int) * running_count);

    memcpy(*address_list, addresses, sizeof(unsigned int) * running_count);

    free(addresses);
    return;
}

void readBin(FILE *bin_fptr, char **bin_buffer, unsigned int *bin_size){
    char *buffer = NULL;
    int size, i, c;
    
    size = BIN_BUFFER_;

    buffer = (char *)safeMalloc(size * sizeof(char));

    errno = 0;
    while(EOF != (c = fgetc(bin_fptr))){
        if(i >= size - 2){/* Reached end of buffer. Get more space*/
            buffer = (char *)safeRealloc(buffer, size + BIN_BUFFER_, size, sizeof(char));
            size += BIN_BUFFER_;
        }
        buffer[i++] = c;
    }

    if(i > 0)
        buffer[i] = '\0';
    else if(i == 0 && c == EOF){
        free(buffer);
        *bin_buffer = NULL;
        return;
    }
    else
        errorout("Something went wrong with readBin.\n");
    
    *bin_buffer = (char *)safeMalloc(sizeof(char) * i);

    memcpy(*bin_buffer, buffer, sizeof(char) * i);
    *bin_size = i;
    if(verbosity)
        printf("Bin size: %d\n", *bin_size);
    free(buffer);
    
    return;
}

void runSimulator(AddressTable* address_table, 
                  TLBTable* tlb_table, 
                  PageTable* page_table, 
                  PageTable* queue,
                  Algorithm algorithm,
                  uint8_t num_frames)
{
    Seek TLB_seek_result = MISS; /* result of looking for page number in TLB*/
    Seek PT_seek_result = MISS;  /* result of looking for page number in page table*/
    uint8_t resolved_frame_num;
    uint8_t seek_page_num;
    TLBEntry new_tlb_entry;
    int i = 0;
    for(i = 0; i < address_table->num_entries; i++)
    {
        seek_page_num = address_table->list[i].page_num;
        new_tlb_entry.page_num  = seek_page_num;

        /* For debugging*/
        if(seek_page_num == 112)
            printf("Break here");

        if(verbosity)
            printf(">>>>>>>> Seeking TLB for page number: %i <<<<<<<<\n\n", seek_page_num);
        TLB_seek_result = checkTLB(tlb_table, algorithm, seek_page_num, &resolved_frame_num);
        
        if(TLB_seek_result == MISS){
            // PT_seek_result = checkPageTable(&address_table->list[i], page_table); // changed from
            if(verbosity)
                printf(">>>>>>>> Seeking Page Table for page number: %i <<<<<<<<\n\n", seek_page_num);
            PT_seek_result = checkPageTable(seek_page_num, page_table, &resolved_frame_num); // to
            
            // PT_seek_result = HIT;
            if(PT_seek_result == HIT)
            {
                new_tlb_entry.frame_num = resolved_frame_num;

                // 1. update Queue if LRU
                if(algorithm == LRU && queue->num_entries > 1)
                    runQueuePRA(queue, 
                                page_table,
                                address_table, 
                                algorithm, 
                                seek_page_num,
                                TLB_seek_result, 
                                PT_seek_result,
                                i);
            } /* End PT Hit*/
            else /* PT Miss*/
            {
                // miss: 
                // 1. get from "bin" (not necessary) 
                // Need to get the frame number and store it in resolved_frame_num. Here
                
                // 2. make missing page valid
                // Need to get the frame number and store it in resolved_frame_num. or Here
                    // if Queue is not full: 
                    // 1. validate page_num in page table
                    // 2. addtoQueue(): inject to front(queue[0]) based on Algorithm
                    // 3. page fault++

                    // else if is full:
                    // 1. run QueuePRA - replace page in Queue & page fault++
                    // 2. update page table (removed & added page)

                if(!isQueueFull(queue, num_frames))
                {
                    resolved_frame_num = page_table->num_entries;
                    setPage(page_table->list, seek_page_num, resolved_frame_num, 1);
                    page_table->num_entries++;
                    
                    // set frame num in Queue here if needed
                    addToQueue(queue, address_table->list[i].page_num);     // move to outside of hit/miss check
                    address_table->page_faults++;
                }
                else
                {
                    // save (page that will be replaced) last entry in queue's frame_num from the page table
                    uint8_t replaced_page_num = queue->list[queue->num_entries - 1].frame_num;
                    resolved_frame_num = page_table->list[replaced_page_num].frame_num;
                    runQueuePRA(queue, page_table, address_table, algorithm, seek_page_num, TLB_seek_result, PT_seek_result, i);

                    if(algorithm != OPT)
                        updatePageTable(page_table->list, replaced_page_num, seek_page_num, resolved_frame_num);
                }
            }/* End PT Miss*/

            // 4. update TLB w/ missing page
            new_tlb_entry.frame_num = resolved_frame_num;

            if(!isTLBFull(tlb_table))
                addPageToTLBTable(tlb_table, new_tlb_entry);
            else
                runTLBPRA(tlb_table, new_tlb_entry);

        }/* End TLB MISS*/
        else{ /* TLB HIT*/
            new_tlb_entry.frame_num = resolved_frame_num; /* Frame num exists*/
            if(queue->num_entries > 1)
                runQueuePRA(queue, page_table, address_table, algorithm, seek_page_num, TLB_seek_result, PT_seek_result, i);

        }
    
        address_table->list[i].frame_num = new_tlb_entry.frame_num;

        // Moved back to inside hit/miss loops
        // 1. check queue if page fault or hit
        // 2. put into Queue using Algorithm

        if(verbosity)
        {
            printf("#################################  after TLB & Page Table HIT/MISS  #################################");
            printf("\n\n#################################  Address Table  #################################\n");
            printf("| page_faults: %i \n", address_table->page_faults);
            printTLBTableDebug(tlb_table, 0);
            printPageTableDebug(page_table, 0, 0);
            printPageTableDebug(queue, 0, 1);
        }
    }
}

void printResults(const AddressTable* address_table, const TLBTable* tlb_table)
{
    int i = 0;
    for(i = 0; i < address_table->num_entries; i++)
    {
        printf("%i, %i, %i, \n", 
        address_table->list[i].address,
        address_table->list[i].byte_referenced,
        address_table->list[i].frame_num
        );

        // print page_data & new line after
        printPageData((char*)address_table->list[i].page_data, MAX_FRAME_SIZE_);
    }

    printPageFaults(address_table);
    printTLBResults(tlb_table, address_table->num_entries);
}

int main(int argc, char *argv[]){
    char *filename;                         /* Name of input file*/
    uint8_t num_frames = MAX_FRAMES_;       /* User specified number of frames*/
    Algorithm algorithm = DEFAULT_ALGO_;    /* User specified Algorithm*/
    FILE *file_ptr;                         /* Pointer to file*/
    FILE *bin_fptr;                         /* Pointer to bin*/
    char *bin_file_path = "BACKING_STORE.bin";
    char *bin_buffer = NULL;                /* Bin buffer*/
    unsigned int bin_size = 0;              /* Size of backing store*/
    unsigned int address_count;             /* Number addresses found in file*/
    unsigned int *address_list = NULL;      /* List of addresses*/
    TLBTable* tlb_table = NULL;             // TLB unit
    PageTable* page_table = NULL;           // Page Table Unit
    AddressTable* address_table = NULL;     // Holds output data for all Address in input.txt
    PageTable* queue = NULL;                // Queue 

    /* Get the user input from the terminal and perform checks*/
    parseOptions(argc, argv, &filename, &num_frames, &algorithm);

    file_ptr = safefOpen(filename); /* Open the user input file*/
    /* Read the file and return a list of addresses and address count*/
    readFile(file_ptr, &address_list, &address_count);
    fclose(file_ptr);

    bin_fptr = safefOpen(bin_file_path);
    readBin(bin_fptr, &bin_buffer, &bin_size);

    initAddressTable(&address_table, address_count);
    parseToAddressTable(address_table, address_list, address_count);
    
    populatePageData(address_table, bin_buffer);

    tlb_table = safeMalloc(sizeof(TLBTable));
    initTLBTable(tlb_table, MAX_TLB_ENTRIES_, num_frames);
    // tlb_table->max_entries = 5; /* For testing FIFO5.txt*/

    page_table = safeMalloc(sizeof(PageTable));
    initPageTable(page_table, MAX_FRAME_SIZE_);

    queue = safeMalloc(sizeof(PageTable));
    initPageTable(queue, MAX_FRAME_SIZE_);

    // 0 is printDetails: for more details in later implementation
    if(verbosity){
        printAddressTable(address_table, 0);
        printTLBTableDebug(tlb_table, 0);
        printPageTableDebug(page_table, 0, 0);
        printPageTableDebug(queue, 0, 1);
    }

    // if(verbosity)
    //     testTLBPRA();

    runSimulator(address_table, tlb_table, page_table, queue, algorithm, num_frames);
    printResults(address_table, tlb_table);

    // for double checking tables / bin
    if(verbosity)
    {
        // printAddressTable(address_table, 0);
        // printTLBTableDebug(tlb_table, 0);
        // printPageTableDebug(page_table, 0, 0);
        // printPageTableDebug(queue, 0, 1);
        // printBuffer(bin_buffer, 256*3);
    }

    fclose(bin_fptr);
    free(bin_buffer);
    free(address_list);

    free(address_table->list);
    free(address_table);
    free(page_table);
    free(tlb_table);

    return 0; 
}
