#include "page_table.h"

void initPage(Page* page)
{
    page->frame_num = 0;
    page->valid = 0;
}

void initPageTable(PageTable* page_table, uint16_t length)
{
    page_table->num_entries = 0;

    int i = 0;
    for(i = 0; i < length; i++)
    {
        initPage(&page_table->list[i]);
    }
}

void printPage(const Page page, uint8_t printDetails, uint8_t printQueue)
{
    if(printQueue)
        printf("page_num: %-3i | valid: %i", page.frame_num, page.valid);
    else
        printf("frame_num: %-3i | valid: %i", page.frame_num, page.valid);

    if(printDetails)
        printf("future stuff (set printDetails = 0 atm)");
    else
        printf("\n");
}

void printPageTable(const Page* list, uint8_t printDetails, uint8_t printQueue)
{
    int i = 0;
    for(i = 0; i < MAX_FRAMES_; i++)
    {
        if(list[i].valid)
        {
            if(printQueue)
                printf("Queue[%-3d]: ", i);
            else
                printf("Page[%-3d]: ", i);
            printPage(list[i], printDetails, printQueue);
        }
    }
}

void printPageTableDebug(const PageTable* page_table, uint8_t printDetails, uint8_t printQueue)
{
    if(printQueue)
    {
        printf("\n\n#################################  Queue Table  #################################\n");
        printf("| num_entries: %i \n", page_table->num_entries);
        // printf("| hits: %i | faults: %i \n", page_table->hits, page_table->faults);
    }
    else
        printf("\n\n#################################  Page Table  #################################\n");
    printPageTable(page_table->list, printDetails, printQueue);
    printf("################################################################################\n\n\n");
}

void pageSwap(Page* dest, Page* src)
{
    Page temp = *dest;
    *dest = *src;
    *src = temp;
}

void setPage(Page* list, uint8_t index, uint8_t frame_num, uint8_t valid)
{
    list[index].frame_num = frame_num;
    list[index].valid = valid;
}

// if page is in Page Table == 1, else == 0
int isPageNumValid(PageTable* page_table, uint8_t page_num, uint8_t *resolved_frame_num)
{
    if(page_table->list[page_num].valid == 1){
        *resolved_frame_num = page_table->list[page_num].frame_num;
        return 1;
    }

    return 0;
}

Seek checkPageTable(uint8_t page_num, PageTable* page_table, uint8_t *resolved_frame_num)
{
    if(isPageNumValid(page_table, page_num, resolved_frame_num))
    {
        // page # is in PageTable => 
        // hit: populate TLB w/ page & put into RAM
        if(verbosity)
            printf(">>>>>>>> Page Table: Page %i Exists <<<<<<<<\n", page_num);

        return HIT;
    }
    else
    {
        if(verbosity)
            printf(">>>>>>>> Page Table: Page %i Doesn't Exist <<<<<<<<\n", page_num);

        return MISS;
    }
}

// returns 1 if TLBTable is Full, else 0; max_entries = <FRAME>
int isQueueFull(PageTable* queue, uint8_t num_frames)
{
    if(queue->num_entries <= num_frames)
    {
        if(verbosity)
            printf("Queue is not Full! \n");

        return 0;
    }

    if(verbosity)
        printf("Queue is Full! | num_entries: %i \n", queue->num_entries);
    
    return 1;
}

// inject to front(queue[0] = MRU) based on Algorithm
void addToQueue(PageTable* queue, uint8_t page_num)
{
    int i = 0;
    for(i = queue->num_entries; i > 0; i--)
    {
        // swap to make room
        pageSwap(&queue->list[i], &queue->list[i-1]);
    }

    // inject
    setPage(queue->list, 0, page_num, 1);
    queue->num_entries++;

    if(verbosity)
        printf("addToQueue | page_num: %i | num_entries: %i \n", page_num, queue->num_entries);
}

// remove last queue entry & make room in queue->list[0]
void removeLastInQueue(PageTable* queue)
{
    setPage(queue->list, queue->num_entries - 1, 0, 0);

    int i = 0;
    for(i = queue->num_entries - 1; i > 0; i--)
    {
        // swap to make room
        pageSwap(&queue->list[i], &queue->list[i-1]);
    }
}

// remove queue entry @ index & make room in queue->list[0]
void removeFromQueue(PageTable* queue, uint8_t index)
{
    setPage(queue->list, index, 0, 0);

    int i = 0;
    for(i = index; i > 0; i--)
    {
        // swap to make room
        pageSwap(&queue->list[i], &queue->list[i-1]);
    }
}

// run QueuePRA - replace page in Queue & page fault++
void runQueuePRA(PageTable* queue, PageTable* page_table, AddressTable* address_table, Algorithm algorithm, uint8_t seek_page_num, Seek TLB_seek_result, Seek PT_seek_result, uint32_t current_index)
{
    switch(algorithm)
    {
        case FIFO:
            if(verbosity)
                printf("runQueuePRA FIFO\n");

            runQueueFIFO(queue, address_table, seek_page_num, TLB_seek_result, PT_seek_result);
            break;

        case LRU:
            if(verbosity)
                printf("runQueuePRA LRU\n");
            runQueueLRU(queue, address_table, TLB_seek_result, PT_seek_result, seek_page_num);
            break;

        case OPT:
            if(verbosity)
                printf("runQueuePRA OPT\n");
            runQueueOPT(queue, page_table, address_table, seek_page_num, TLB_seek_result, PT_seek_result, current_index);
            break;

        default:
            perror("#ERROR: runQueuePRA defaulted, does not exist \n");
            exit(EXIT_FAILURE);
            break;
    }
}

void runQueueFIFO(PageTable* queue, AddressTable* address_table, uint8_t seek_page_num, Seek TLB_seek_result, Seek PT_seek_result)
{
    if(TLB_seek_result == MISS && PT_seek_result == MISS)
    {
        removeLastInQueue(queue);

        // inject
        setPage(queue->list, 0, seek_page_num, 1);

        if(verbosity)
            printf("runQueueFIFO | page_num: %i | num_entries: %i \n", seek_page_num, queue->num_entries);

        address_table->page_faults++;
    }
}

// update page table (removed & added page)
void updatePageTable(Page* list, uint8_t old_page_num, uint8_t new_page_num, uint8_t frame_num)
{
    // invalidate old page
    setPage(list, old_page_num, frame_num, 0);
    // validate new page
    setPage(list, new_page_num, frame_num, 1);

    printf("updatePageTable | old page: %i | new page: %i \n", old_page_num, new_page_num);
}


void runQueueLRU(PageTable* queue, AddressTable* address_table, Seek TLB_seek_result, Seek PT_seek_result, uint8_t page_num)
{
    uint8_t queue_pos;
    
    if(TLB_seek_result == HIT || PT_seek_result == HIT)
    {
        if(queue->num_entries > 1)
        {
            queue_pos = getQueuePosition(queue, page_num);
            reorderQueue(queue, queue_pos);
        }
        else
            return;
    }
    else if(PT_seek_result == MISS)
    {
        runQueueFIFO(queue, address_table, page_num, TLB_seek_result, PT_seek_result);
    }
    return;
}

uint8_t getQueuePosition(PageTable* queue, uint8_t page_num)
{
    int queue_pos;
    uint8_t curr_page_num; /* Queue page num is stored in frame_num*/

    for(queue_pos = queue->num_entries; queue_pos >= 0; queue_pos--){
        curr_page_num = queue->list[queue_pos].frame_num; /* For clarity, queue frame num = page num*/
        if(curr_page_num == page_num){
            if(verbosity){
                printf("Queue[%-3d]: ", queue_pos);
                printPage(queue->list[queue_pos], 0, PRINT_QUEUE_);
            }
            return queue_pos;
        }
    }
    
    errorout("Something went wrong. Can't find page in queue.\n");
    return 0;
}

void reorderQueue(PageTable* queue, uint8_t queue_pos)
{
    Page temp_page;
    temp_page = queue->list[queue_pos]; /* Store the queue page*/
    slideQueue(queue, queue_pos); /* Slide queue and leave a spot at the front*/
    queue->list[0] = temp_page;/* Push to back of queue*/
    return;
}

void slideQueue(PageTable* queue, uint8_t popped_pos)
{
    int queue_pos;

    for(queue_pos = popped_pos; queue_pos > 0; queue_pos--)
    {
        pageSwap(&queue->list[queue_pos], &queue->list[queue_pos - 1]);
    }
    return;
}

// check each entry in Queue that does not exist in the future (in AddressTable->list)
// 0 == not found, 1 == removed page num (to invalidate)
uint8_t removeNoFutureInQueue(PageTable* queue, PageTable* page_table, AddressTable* address_table, uint32_t current_index)
{
    // int i = 0, j = 0;

    // for(i = 0; i < queue->num_entries; i++)  // might be incorrect
    // {
    //     // check the all items from the next index (= current address + 1)
    //     // j <= address_table->num_entries: checked all addresses
    //     for(j = current_index + 1; j <= address_table->num_entries; j++)
    //     {
    //             // if not in the future address list => remove
    //             if(queue->list[i].frame_num != address_table->list[j].page_num)
    //             {
    //                 if(j == address_table->num_entries)
    //                 {
    //                     if(verbosity)
    //                     {
    //                         printf("removeNoFutureInQueue | removed: ");
    //                         printPage(queue->list[i], 0, 1);
    //                     }
    //                     removeFromQueue(queue, i);

    //                     return queue->list[i].frame_num;
    //                 }
    //             }
    //     }
    // }

    // return 0;

    int i = 0, j = 0, isInFuture = 0;
    uint8_t old_page_num = 0;

    for(i = queue->num_entries - 1; i > 0; i--)     // changed to FIFO
    // for(i = 0; i < queue->num_entries; i++)  // might be incorrect
    {
        isInFuture = 0;
        // check the all items from the next index (= current address + 1)
        // j <= address_table->num_entries: checked all addresses
        for(j = current_index + 1; j < address_table->num_entries; j++)
        {
                // if not in the future address list => remove
                if(queue->list[i].frame_num == address_table->list[j].page_num)
                {
                    isInFuture = 1;
                    break; // moves onto the next queue entry
                }
                else
                {
                    old_page_num = queue->list[i].frame_num;
                }
        }

        if(j >= address_table->num_entries && !isInFuture)
        {
            if(verbosity)
            {
                printf("removeNoFutureInQueue | removed: ");
                printPage(queue->list[i], 0, 1);
            }
            removeFromQueue(queue, i);

            return old_page_num;
        }
    }

    return 0;
}

/* Returns page number*/
uint8_t removeFurthestInQueue(PageTable* queue, AddressTable* address_table, uint32_t current_index)
{
    int i = 0, j = 0, furthest_index = 0, curr_page_num;
    uint8_t old_page_num = 0;

    for(i = 0; i < queue->num_entries; i++) /* Index for queue*/
    {
        curr_page_num = queue->list[i].frame_num; /* queue frame num = page num*/
        if(verbosity)
            printf("<<<<<<<< Looking for Future Address (Page) [%-3d] >>>>>>>>\n", curr_page_num);
        // check the all items from the back of the queue to current address
        for(j = address_table->num_entries - 1; j > current_index; j--) /* Index for address list*/
        {
            // if queue page num is found, check if its the largest index
            if(curr_page_num == address_table->list[j].page_num){
                if(verbosity)
                    printf(">>>>>>>> Future Address (Page) [%-3d] Found. Keep Looking <<<<<<<<\n", 
                           address_table->list[j].page_num);
                // continue;
                if(j > furthest_index)
                    furthest_index = j;
            }
        }
        if(verbosity){
            printf("<<<<<<<< NO Future Address (Page) [%-3d] Found >>>>>>>>\n", 
                   curr_page_num);
            printf(">>>>>>>> Removing Page [%-3d] From Queue <<<<<<<<\n", curr_page_num);
            printPage(queue->list[i], 0, 1);
        }

        // furthest_index = i;
        old_page_num = queue->list[furthest_index].frame_num;
        removeFromQueue(queue, furthest_index);

        if(verbosity){
            printf("<<<<<<<< Removed Page [%-3d] From Queue >>>>>>>>\n", 
                   curr_page_num);
            printPage(queue->list[i], 0, 1);
        }

        return old_page_num;
    }
    if(verbosity){
        printf("<<<<<<<< All Pages Exist in the Future >>>>>>>>\n");
    }

    // return 0; /* Shouldn't get here? */
    return queue->list[queue->num_entries - 1].frame_num;
}

void runQueueOPT(PageTable* queue, PageTable* page_table, AddressTable* address_table, uint8_t seek_page_num, Seek TLB_seek_result, Seek PT_seek_result, uint32_t current_index)
{
    uint8_t isInFuture = 0;
    if(TLB_seek_result == MISS && PT_seek_result == MISS)
    {
        isInFuture = removeNoFutureInQueue(queue, page_table, address_table, current_index);
        if(!isInFuture)
            isInFuture = removeFurthestInQueue(queue, address_table, current_index);

        updatePageTable(page_table->list, isInFuture, seek_page_num, page_table->list[isInFuture].frame_num);

        // inject
        setPage(queue->list, 0, seek_page_num, 1);

        if(verbosity)
            printf("runQueueOPT | page_num: %i | num_entries: %i \n", seek_page_num, queue->num_entries);
        address_table->page_faults++;
    }
}