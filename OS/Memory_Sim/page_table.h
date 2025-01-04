#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include "address.h"
#include "tlb.h"

#define PRINT_QUEUE_ 1

typedef struct page Page;
typedef struct page_table PageTable;

struct page
{
    // if its PageTable* Queue->frame_num = page num
    uint8_t frame_num;
    uint8_t valid;
};

struct page_table
{
    // page table = current available frame (Queue is not Empty yet)
    // queue = # entries in the queue
    uint8_t num_entries;

    Page list[MAX_FRAME_SIZE_];
};

void initPage(Page* page);
void initPageTable(PageTable* page_table, uint16_t length);
void printPage(const Page page, uint8_t printDetails, uint8_t printQueue);
void printPageTable(const Page* list, uint8_t printDetails, uint8_t printQueue);
void printPageTableDebug(const PageTable* page_table, uint8_t printDetails, uint8_t printQueue);
void pageSwap(Page* dest, Page* src);
void setPage(Page* list, uint8_t index, uint8_t frame_num, uint8_t valid);
// if page is in Page Table == 1, else == 0
int isPageNumValid(PageTable* page_table, uint8_t page_num, uint8_t *resolved_frame_num);
Seek checkPageTable(uint8_t page_num, PageTable* page_table, uint8_t *resolved_frame_num);
// returns 1 if TLBTable is Full, else 0; max_entries = <FRAME>
int isQueueFull(PageTable* queue, uint8_t max_entries);
// inject to front(queue[0] = MRU) based on Algorithm
void addToQueue(PageTable* queue, uint8_t page_num);
// remove last queue entry & make room in queue->list[0]
void removeLastInQueue(PageTable* queue);
void removeFromQueue(PageTable* queue, uint8_t index);
// run QueuePRA - replace page in Queue & page fault++
void runQueuePRA(PageTable* queue, PageTable* page_table, AddressTable* address_table, Algorithm algorithm, uint8_t seek_page_num, Seek TLB_seek_result, Seek PT_seek_result, uint32_t current_index);
void runQueueFIFO(PageTable* queue, AddressTable* address_table, uint8_t seek_page_num, Seek TLB_seek_result, Seek PT_seek_result);
// update page table (removed & added page)
void updatePageTable(Page* list, uint8_t old_page_num, uint8_t new_page_num, uint8_t frame_num);
void runQueueLRU(PageTable* queue, AddressTable* address_table, Seek TLB_seek_result, Seek PT_seek_result, uint8_t page_num);
uint8_t getQueuePosition(PageTable* queue, uint8_t page_num);
void reorderQueue(PageTable* queue, uint8_t queue_pos);
void slideQueue(PageTable* queue, uint8_t popped_pos);
// check each entry in Queue that does not exist in the future (in AddressTable->list)
// 0 == not found, 1 == removed page num (to invalidate)
uint8_t removeNoFutureInQueue(PageTable* queue, PageTable* page_table, AddressTable* address_table, uint32_t current_index);
uint8_t removeFurthestInQueue(PageTable* queue, AddressTable* address_table, uint32_t current_index);
void runQueueOPT(PageTable* queue, PageTable* page_table, AddressTable* address_table, uint8_t seek_page_num, Seek TLB_seek_result, Seek PT_seek_result, uint32_t current_index);

#endif