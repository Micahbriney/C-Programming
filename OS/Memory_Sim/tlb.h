#ifndef TLB_H
#define TLB_H

#include "address.h"

#define MAX_TLB_ENTRIES_ 16

typedef enum {MISS = 0, HIT = 1} Seek;

typedef struct tlb_entry TLBEntry;
typedef struct tlb_table TLBTable;


struct tlb_entry
{
    uint8_t page_num;
    uint8_t frame_num;
};

struct tlb_table
{
    uint32_t miss;
    uint32_t hits;
    
    uint8_t max_entries;
    uint8_t num_entries;
    TLBEntry list[MAX_TLB_ENTRIES_];
};

void initTLB(TLBEntry* tlb);
void initTLBTable(TLBTable* tlb_table, uint8_t length, uint8_t num_frames);
void printTLB(const TLBEntry tlb, uint8_t printDetails);
void printTLBTable(const TLBEntry* list, uint8_t length, uint8_t printDetails);
void printTLBTableDebug(const TLBTable* tlb_table, uint8_t printDetails);
void tlbSwap(TLBEntry* dest, TLBEntry* src);
void setTLB(TLBEntry* list, uint8_t index, const TLBEntry entry);
void addPageToTLBTable(TLBTable* tlb_table, const TLBEntry tlb_entry);
void testCheckTLB(TLBTable* tlb_table);
// returns 1 if TLBTable is Full, else 0
int isTLBFull(TLBTable* tlb_table);
// run the page replacement algorithm
void runTLBPRA(TLBTable* tlb_table, const TLBEntry entry);
void testTLBPRA();
Seek checkTLB(TLBTable* tlb_table, Algorithm algorithm, uint8_t page_num, uint8_t *resolved_frame_num);
void printTLBResults(const TLBTable* tlb_table, uint32_t num_entries);


#endif