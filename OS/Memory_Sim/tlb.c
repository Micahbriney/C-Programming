#include "tlb.h"
#include "page_table.h"
#include "safeutil.h"

void initTLB(TLBEntry* tlb)
{
    tlb->page_num = 0;
    tlb->frame_num = 0;
}

void initTLBTable(TLBTable* tlb_table, uint8_t length, uint8_t num_frames)
{
    tlb_table->miss = 0;
    tlb_table->hits = 0;

    if(num_frames + 1 >= MAX_TLB_ENTRIES_) /* If there are at least 16 frames*/
        tlb_table->max_entries = MAX_TLB_ENTRIES_; /* Set max entries to 16*/
    else
        tlb_table->max_entries = num_frames + 1; /* Else TLB will only hold < 16*/

    tlb_table->num_entries = 0;

    int i = 0;
    for(i = 0; i < length; i++)
    {
        initTLB(&tlb_table->list[i]);
    }
}

void printTLB(const TLBEntry tlb, uint8_t printDetails)
{
    printf("page_num: %-3i | frame_num: %-3i ", tlb.page_num, tlb.frame_num);

    if(printDetails)
        printf("future stuff (set printDetails = 0 atm)");
    else
        printf("\n");
}

void printTLBTable(const TLBEntry* list, uint8_t length, uint8_t printDetails)
{
    int i = 0;
    for(i = 0; i < length; i++)
    {
        printf("TLBEntry[%-3i]: ", i);
        printTLB(list[i], printDetails);
    }
}

void printTLBTableDebug(const TLBTable* tlb_table, uint8_t printDetails)
{
    printf("\n\n#################################  TLB Table  #################################\n");
    printf("| num_entries: %i | max_entries: %i | hits: %i | miss: %i \n", tlb_table->num_entries, tlb_table->max_entries + 1, tlb_table->hits, tlb_table->miss);
    printTLBTable(tlb_table->list, tlb_table->num_entries, printDetails);
    printf("###############################################################################\n\n\n");
}

void tlbSwap(TLBEntry* dest, TLBEntry* src)
{
    TLBEntry temp = *dest;
    *dest = *src;
    *src = temp;
}

void setTLB(TLBEntry* list, uint8_t index, const TLBEntry tlb_entry)
{
    list[index].page_num = tlb_entry.page_num;
    list[index].frame_num = tlb_entry.frame_num;
}

void addPageToTLBTable(TLBTable* tlb_table, const TLBEntry tlb_entry)
{
    setTLB(tlb_table->list, tlb_table->num_entries, tlb_entry);
    tlb_table->num_entries++;
}

Seek checkTLB(TLBTable* tlb_table, Algorithm algorithm, uint8_t page_num, uint8_t *frame_num)
{
    int TLB_entry;

    /* If there are no entries in the tlb_table then it is a miss*/
    if(tlb_table->num_entries <= 0){
        if(verbosity)
            printf("<<<<<<<< TLB empty: Page %i Doesn't Exist In TLB >>>>>>>>\n\n", page_num);
        tlb_table->miss++;
        return MISS;
    }
    
    for(TLB_entry = 0; TLB_entry < tlb_table->num_entries; TLB_entry++){ /* Search tlb_table for page_num*/
        if(tlb_table->list[TLB_entry].page_num == page_num){ /* Entry found*/
            *frame_num = tlb_table->list[TLB_entry].frame_num;
            if(verbosity)
                printf("<<<<<<<< TLB: Page %i Exists. Its mapped to frame %i >>>>>>>>\n", page_num, *frame_num);
            tlb_table->hits++; /* Page was found*/
            return HIT;
        }
    }
    /* No hit*/
    if(verbosity)
        printf("<<<<<<<< TLB: Page %i Doesn't Exist >>>>>>>>\n", page_num);
    tlb_table->miss++;
    return MISS;
}


// returns 1 if TLBTable is Full, else 0
int isTLBFull(TLBTable* tlb_table)
{
    if(tlb_table->num_entries < tlb_table->max_entries)
    {
        if(verbosity)
            printf("TLBTable is not Full! \n");

        return 0;
    }

    if(verbosity)
        printf("TLBTable is Full! | num_entries: %i \n", tlb_table->num_entries);
    
    return 1;
}

// run the page replacement algorithm for the TLB
void runTLBPRA(TLBTable* tlb_table, const TLBEntry entry)
{
    static uint8_t tlb_fifo_position = 0; /* Start at 0 after TLB is full*/
    tlb_fifo_position %= tlb_table->max_entries; /* Keep between [0:max_entries]*/
    if(verbosity){
        printTLBTableDebug(tlb_table, 0);
        printf("Replacing TLB entry %-3i: Page %-3i -> %-3i | Frame %-3i -> %-3i.\n", tlb_fifo_position,
        tlb_table->list[tlb_fifo_position].page_num,  entry.page_num,
        tlb_table->list[tlb_fifo_position].frame_num, entry.frame_num);
    }
    setTLB(tlb_table->list, tlb_fifo_position++, entry);
    if(verbosity){
        printf("\n\n------------------------- UPDATED TLB TABLE -------------------------\n");
        printTLBTableDebug(tlb_table, 0);
    }

    return;
}

void testCheckTLB(TLBTable* tlb_table){
    verbosity = 1;
    int i;
    Algorithm algorithm = FIFO;
    uint8_t page_num;
    uint8_t frame_num;
    TLBEntry entries[] = {{0, 3}, {1, 7}, {2, 0}};
    Seek TLB_seek_result = MISS;

    struct {
        uint8_t page_num;
        const char* message;
    } tests[] = {{3, "Found"}, {7, "Found"}, {0, "Found"}, {8, "Not Found"}};

    for (i = 0; i < 3; i++) {
        tlb_table->list[i].frame_num = entries[i].frame_num;
        tlb_table->list[i].page_num  = entries[i].page_num;
        tlb_table->num_entries++;
    }
        
    for (i = 0; i < 4; i++) {
        page_num = tests[i].page_num;
        TLB_seek_result = checkTLB(tlb_table, algorithm, page_num, &frame_num);
        printf("TLB %s: Page Number %i %s!\n", 
               (TLB_seek_result == MISS) ? "MISS" : "HIT", page_num, tests[i].message);
    }

    return;
}

void testTLBPRA(){
    verbosity = 1;
    TLBTable* tlb_table;
    TLBEntry entry;
    int i;

    tlb_table = safeMalloc(sizeof(TLBTable));
    initTLBTable(tlb_table, MAX_TLB_ENTRIES_, MAX_FRAME_SIZE_ - 1);

    printf("\n\n------------------------- TLB PRA TEST -------------------------\n");
    printTLBTableDebug(tlb_table, 0);
    /* Fill up TLB */
    for(i = 0; i < tlb_table->max_entries; i++){
        tlb_table->list[i].frame_num   = i;
        tlb_table->list[i].page_num    = i;
        tlb_table->num_entries++;
    }
    
    printf("\n\n------------------------- TLB TABLE FILLED -------------------------\n");

    /* Adjust + # to test more or less replacements*/
    for(; i < tlb_table->max_entries + 20; i++){
        entry.frame_num   = i;
        entry.page_num    = i;
        runTLBPRA(tlb_table, entry);
    }

    return;
}

void printTLBResults(const TLBTable* tlb_table, uint32_t num_entries)
{
    float hit_rate = (float)tlb_table->hits/(float)num_entries;

    printf("TLB Hits = %i \n"
            "TLB Misses = %i \n"
            "TLB Hit Rate = %3.3f \n",
            tlb_table->hits,
            tlb_table->miss,
            hit_rate);
}