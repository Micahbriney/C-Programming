#include <pcap/pcap.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "dataparser.h"
#include "messages.h"

int process_trace_file(char *filename);

/**
 *@fn:    main
 *@brief: Entry for the program
 *@parms: int, char *
 *@rtrn:  int
**/
int main(int argc, char *argv[]){
  int err = 0;
  
  if(argc != 2)
    error_out("Usage: ./trace tracefile");

  if((err = process_trace_file(argv[argc - 1])) != 1)
    error_out("Failure to open file");
  
  return 0;
}

/**
 *@fn:    process_trace_file
 *@brief: This function handle all reading the pcap files. For each packet 
          it will be send the the print_packet function. This utilizes the 
	  pcap library. Before returning to main the file will be closed.
 *@parms: char *
 *@rtrn:  int
**/
int process_trace_file(char *filename){
  pcap_t *fd;
  char errbuf[PCAP_ERRBUF_SIZE];
  struct pcap_pkthdr *pkt_header;
  const u_char *pkt_data;
  int counter = 1;
  int pcaperr = 0;
  /* Open trace file*/
  if(!(fd = pcap_open_offline(filename, errbuf)))
    printf("PCAP_OPEN FAILED");
  while(1 == (pcaperr = pcap_next_ex(fd, &pkt_header, &pkt_data))){
    print_packet((uint8_t *)pkt_data, counter++, pkt_header->len);
  }
  if(pcaperr != PCAP_ERROR_BREAK)
    error_out(pcap_geterr(fd));

  pcap_close(fd); /* Close file when done*/
  
  return 1;
}

