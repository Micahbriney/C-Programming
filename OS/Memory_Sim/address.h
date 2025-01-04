#ifndef ADDRESS_H
#define ADDRESS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "options.h"

typedef struct address Address;
typedef struct address_table AddressTable;

struct address
{
    uint32_t address;
    uint8_t page_num;
    uint8_t offset;

    int8_t byte_referenced;
    uint8_t frame_num;
    uint8_t page_data[MAX_FRAME_SIZE_];
};

struct address_table
{
    uint32_t page_faults;
    
    uint32_t num_entries;
    Address* list;
};

void initAddress(Address* address);
void initAddressTable(AddressTable** address_table, uint32_t length);
void printBuffer(char* buffer, int length);
void printAddressPageData(const uint8_t* page_data, int length);
void printAddress(const Address address, uint8_t printFrame);
void printAddressTable(const AddressTable* address_table, uint8_t printFrame);
void addAddress(Address* address, const uint32_t logical_address);
void addAddressToTable(AddressTable* address_table, const uint32_t logical_address);
void parseToAddressTable(AddressTable* address_table, const unsigned int* address_list, int length);
// masks the 32-bit logical address to get the 16 rightmost bits (= 8-bit page #, 2 8-bit page offset)
uint16_t maskLogicalAddress(const uint32_t logical_address);
// masks the 16 rightmost bits to get the left 8-bits = page #
uint8_t maskPageNum(const uint16_t right_most_bits);
// masks the 16 rightmost bits to get the left 8-bits = page #
uint8_t maskOffset(const uint16_t right_most_bits);
void populatePageData(AddressTable* address_table, char* bin_buffer);
void printPageData(char* buffer, int length);
void printPageFaults(const AddressTable* address_table);

#endif