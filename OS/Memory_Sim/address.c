#include "address.h"
#include "safeutil.h"

void initAddress(Address* address)
{
    address->address = 0;
    address->page_num = 0;
    address->offset = 0;

    address->byte_referenced = 0;
    address->frame_num = 0;

    int i = 0;
    for(i = 0; i < MAX_FRAME_SIZE_; i++)
    {
        address->page_data[i] = 0;
    }
}

void initAddressTable(AddressTable** address_table, uint32_t length)
{
    *address_table = (AddressTable*) safeMalloc(sizeof(AddressTable));
    (*address_table)->list = (Address*) safeMalloc(sizeof(Address) * length);
    (*address_table)->num_entries = 0;

    int i = 0;
    for(i = 0; i < length; i++)
    {
        initAddress(&(*address_table)->list[i]);
    }
}

void printBuffer(char* buffer, int length)
{
    int i;

    if(buffer == NULL)
    {
        perror("#ERROR: buffer empty!!!!");
        exit(1);
    }

    for(i = 0; i < length; i++)
    {
        if(i % 8 == 0 && i != 0)
        {
            printf("|");

            if(i % 32 == 0)   // for spacing octets
                printf("\n");
            if(i % 256 == 0)   // for spacing octets
                printf("\n");
        }

        // printf("[%02d] 0x%0X ('%c')\t", i, buffer[i], (char)buffer[i]);
        printf("%02X ", (uint8_t)buffer[i]);

    }
    printf("\n");
}

void printAddressPageData(const uint8_t* page_data, int length)
{
    int i = 0;
    for(i = 0; i < length; i++)
    {
        if(i % 4 == 0)
            printf("\n");

        printf("page_data[%-3i]: 0x%02X (%-3i) | ", i, page_data[i], page_data[i]);
    }
    printf("\n\n");
}

void printAddress(const Address address, uint8_t printFrame)
{
    printf("address: 0x%04x (%-4i) | page_num: 0x%02x (%-3i) | offset: 0x%02x (%-3i) | frame_num: 0x%02x (%-3i)  | byte_referenced: 0x%02x (%-3i) ", 
    address.address, address.address, address.page_num, address.page_num, address.offset, address.offset, address.frame_num, address.frame_num, address.byte_referenced, address.byte_referenced);

    if(printFrame)
        printAddressPageData(address.page_data, MAX_FRAME_SIZE_);
    else
        printf("\n");
}

void printAddressTable(const AddressTable* address_table, uint8_t printFrame)
{
    int i = 0;
    printf("\n\n#################################  Address Table  #################################\n");
    printf("| page_faults: %i \n", address_table->page_faults);
    for(i = 0; i < address_table->num_entries; i++)
    {
        printf("AddressTable[%-3i] | ", i);
        printAddress(address_table->list[i], printFrame);
    }
    printf("################################################################################\n\n\n");
}

void addAddress(Address* address, const uint32_t logical_address)
{
    uint16_t right_most_bits = maskLogicalAddress(logical_address);

    address->address = logical_address;
    address->page_num = maskPageNum(right_most_bits);
    address->offset = maskOffset(right_most_bits);
}

void addAddressToTable(AddressTable* address_table, const uint32_t logical_address)
{
    addAddress(&address_table->list[address_table->num_entries], logical_address);
    address_table->num_entries++;
}

void parseToAddressTable(AddressTable* address_table, const unsigned int* address_list, int length)
{
    int i = 0;
    for(i = 0; i < length; i++)
    {
        addAddressToTable(address_table, address_list[i]);
    }
}

// masks the 32-bit logical address to get the 16 rightmost bits (= 8-bit page #, 2 8-bit page offset)
uint16_t maskLogicalAddress(const uint32_t logical_address)
{
    uint16_t result = 0;
    uint32_t mask = 0x0000FFFF;

    result = logical_address & mask;

    return result;
}

// masks the 16 rightmost bits to get the left 8-bits = page #
uint8_t maskPageNum(const uint16_t right_most_bits)
{
    uint8_t result = 0;
    uint16_t mask = 0x0000FF00;

    result = (right_most_bits & mask) >> 8;

    return result;
}

// masks the 16 rightmost bits to get the left 8-bits = page #
uint8_t maskOffset(const uint16_t right_most_bits)
{
    uint8_t result = 0;
    uint16_t mask = 0x000000FF;

    result = right_most_bits & mask;

    return result;
}

void populatePageData(AddressTable* address_table, char* bin_buffer)
{
    // implement reading data from bin for the specified 
    int i = 0;
    // int index = 0;
    for(i = 0; i < address_table->num_entries; i++)
    {
        // page data
        memcpy(address_table->list[i].page_data, 
               bin_buffer + (address_table->list[i].page_num * MAX_FRAME_SIZE_), 
               MAX_FRAME_SIZE_);

        // byte referenced
        memcpy(&address_table->list[i].byte_referenced, 
               bin_buffer + (address_table->list[i].page_num * MAX_FRAME_SIZE_) + address_table->list[i].offset, 
               1);

        // if(verbosity)
        // {
        //     checking if memcpys is correct => yes
        //     printf("Page Data: \n");
        //     printAddressPageData(address_table->list[i].page_data, MAX_FRAME_SIZE_);

        //     printf("Bin Data: \n");
        //     printBuffer((bin_buffer + (address_table->list[i].page_num * MAX_FRAME_SIZE_)), 
        //                 MAX_FRAME_SIZE_);
        // }
    }

    // if(verbosity)
        // printAddressTable(address_table, 0);

    return;
}

void printPageData(char* buffer, int length)
{
    int i;

    if(buffer == NULL)
    {
        perror("#ERROR: buffer empty!!!!");
        exit(1);
    }

    for(i = 0; i < length; i++)
    {
        printf("%02X", (uint8_t)buffer[i]);

    }
    printf("\n");
}

void printPageFaults(const AddressTable* address_table)
{
    float page_faults = (float)address_table->page_faults/(float)address_table->num_entries;

    printf("Number of Translated Addresses = %i \n"
            "Page Faults = %i \n"
            "Page Fault Rate = %3.3f \n", 
            address_table->num_entries,
            address_table->page_faults,
            page_faults
            );
}