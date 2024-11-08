#ifndef MESSAGES_H_
#define MESSAGES_H_

#include <stdio.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ether.h>

void print_packet(uint8_t *data, int pcktNum, int fLen);
uint16_t print_ethr(uint8_t *data);
void print_arp_header(uint8_t *data);
uint8_t print_ip_header(uint8_t *data, uint16_t *offset);
void print_icmp_header(uint8_t *data);
void print_tcp_header(uint8_t *data, uint8_t offset);
void print_udp_header(uint8_t *data);
void error_out(char *error);


enum ethrtypes   {IP = 0x0800, ARP = 0x0806};
enum arpopcodes  {ARPREQUEST = 1, ARPREPLY = 2};
enum ipprotocols {ICMP = 1, TCP = 6, UDP = 17};
enum icmptypes   {REPLY = 0, REQUEST = 8};

#endif
