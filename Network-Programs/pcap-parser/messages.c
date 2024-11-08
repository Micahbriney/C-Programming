#include "messages.h"
#include "dataparser.h"
#include "checksum.h"
#include <inttypes.h>

/**
 *@fn:    print_packet
 *@brief: This function handle all of the print function logic calls. The checks use enum
          values found in message.h.
 *@parms: uint8_t *, int, int
 *@rtrn:  void
**/
void print_packet(uint8_t *data, int pcktNum, int fLen){
  uint16_t eType;
  uint8_t  ipPrtcl;
  uint16_t offset;

  printf("\nPacket number: %d  Frame Len: %u\n", pcktNum, fLen);
  
  eType = print_ethr(data);
  offset = IPOFFSET;

  /* Decide on layer 3 print*/
  if(eType == ARP){                  /* Use ARP print function*/
    print_arp_header(data + offset); /* Offset datastream with IPOFFSET*/
    return;                          /* Done with ARP packets*/
  }
  else if(eType == IP)                                 /* Use IP print function*/
    ipPrtcl = print_ip_header(data + offset, &offset); /* Address of offset is reused*/
  else                                  /* Something else. Error for now*/
    error_out("Unknown Ethernet Type");

  /* Decide on layer 4 print*/
  if(ipPrtcl == ICMP)                 /* Use ICMP print function*/
    print_icmp_header(data + offset);
  else if(ipPrtcl == TCP)             /* Use TCP print function*/
    print_tcp_header(data, offset);
  else if(ipPrtcl == UDP)             /* Use UDP print function*/
    print_udp_header(data + offset);

  return;
}


/**
 *@fn:    print_ethr
 *@brief: This function will parse the data into an ethernet header struct
          after which the typefield is converted to network order for 
	  printing. The function returns the Ethernet type.
 *@parms: uint8_t
 *@rtrn:  uint16_t
**/
uint16_t print_ethr(uint8_t *data){
  ethrPtr ethrHdrPtr;
  uint16_t type;

  if(NULL == (ethrHdrPtr = (ethrPtr)malloc(sizeof(ethr))))
    error_out("ethernet header pointer malloc");
  
  build_ethr_hdr(data, ethrHdrPtr, &type);

  printf("\n\tEthernet Header\n");
  printf("\t\tDest MAC: %s\n",   get_mac_addr(ethrHdrPtr->dstMAC));
  printf("\t\tSource MAC: %s\n", get_mac_addr(ethrHdrPtr->srcMAC)); 
  printf("\t\tType: %s\n",       get_ethr_type(ethrHdrPtr->type));
  free(ethrHdrPtr);
  
  return type;
}

/**
 *@fn:    print_arp_header
 *@brief: This function will print the arp header fields. Most of the logic is taken care
          of in the dataparser file.
 *@parms: uint8_t *
 *@rtrn:  void
**/
void print_arp_header(uint8_t *data){
  arpPtr arpHdrPtr;

  if(NULL == (arpHdrPtr = (arpPtr)malloc(sizeof(arp))))
    error_out("ARP header pointer malloc");

  build_arp_hdr(data, arpHdrPtr);

  printf("\n\tARP header\n");
  printf("\t\tOpcode: %s\n",      get_arp_optcode(arpHdrPtr->optcode));
  printf("\t\tSender MAC: %s\n",  get_mac_addr(arpHdrPtr->sndrMAC));
  printf("\t\tSender IP: %s\n",   get_ip_addr(arpHdrPtr->sndrIP));
  printf("\t\tTarget MAC: %s\n",  get_mac_addr(arpHdrPtr->trgtMAC));
  printf("\t\tTarget IP: %s\n\n", get_ip_addr(arpHdrPtr->trgtIP));
  free(arpHdrPtr);

  return;
}

/**
 *@fn:    print_ip_header
 *@brief: This function will print the IP header data. The following logic occurs in 
          the dataparser file. The PDU length, source & destination address fields are
	  converted to network byte order for printing. To determine the header
	  length multiple the lower nibble of the field by 4. The checksum
	  is calculated using provided code. The checksum takes care of the 
	  calcualtion by providing it with the IP header struct and header length.
	  The result should be 0 for correct checksums. The function will also
	  store the len of the ip header in the offset parameter. Lastly the 
	  function will return the ip protocol value.
 *@parms: uint8_t *, uint16_t *
 *@rtrn:  uint8_t
**/
uint8_t print_ip_header(uint8_t *data, uint16_t *offset){
  ipPtr   ipHdrPtr;
  uint8_t ipPrtcl;
  uint8_t len;
  unsigned short chk;

  if(NULL == (ipHdrPtr = (ipPtr)malloc(sizeof(ip))))
    error_out("IP header pointer malloc");

  build_ip_hdr(data, ipHdrPtr);

  ipPrtcl  = ipHdrPtr->protocol;                       /* Return value*/
  len      = (ipHdrPtr->len & IPLENNIBBLE) * IPLENMUL; /* Calc len of header*/
  *offset += len;                                      /* Offset for layer 4 header*/
  chk      = in_cksum((unsigned short *)data, len);
  
  printf("\n\tIP Header\n");
  printf("\t\tHeader Len: %u (bytes)\n", len);
  printf("\t\tTOS: 0x%x\n",              ipHdrPtr->TOS);
  printf("\t\tTTL: %u\n",                ipHdrPtr->TTL);
  printf("\t\tIP PDU Len: %d (bytes)\n", ipHdrPtr->PDULen);
  printf("\t\tProtocol: %s\n",           get_ip_protocol(ipHdrPtr->protocol));
  printf("\t\tChecksum: %s (0x%x)\n",
	 !chk ? "Correct" : "Incorrect", ipHdrPtr->chksum);
  printf("\t\tSender IP: %s\n",          get_ip_addr(ipHdrPtr->srcAddr));
  printf("\t\tDest IP: %s\n",            get_ip_addr(ipHdrPtr->dstAddr));
  free(ipHdrPtr);
  
  return ipPrtcl;
}

/**
 *@fn:    print_icmp_header
 *@brief: This function will print the icmp header data. REPLY and REQUEST are enums.
          If the type matches then it will print a string otherwise print the
	  decimal value of the type. 
 *@parms: uint8_t *
 *@rtrn:  void
**/
void print_icmp_header(uint8_t *data){
  icmpPtr icmpHdrPtr;
  
  if(NULL == (icmpHdrPtr = (icmpPtr)malloc(sizeof(icmp))))
    error_out("ICMP header pointer malloc");

  build_icmp_hdr(data, icmpHdrPtr);

  printf("\n\tICMP Header\n");
  printf("\t\tType: ");
  if(icmpHdrPtr->type == REPLY || icmpHdrPtr->type == REQUEST)
    printf("%s\n", get_icmp_type(icmpHdrPtr->type));
  else
    printf("%d\n", icmpHdrPtr->type);

  free(icmpHdrPtr);
  
  return;
}

/**
 *@fn:    print_tcp_header
 *@brief: This function will print the TCP header infomation. The checksum is done using 
          logic from parser functions and a data copy/append helper function. These are 
	  located in the dataparser file.
 *@parms: uint8_t *, uint8_t
 *@rtrn:  void
**/
void print_tcp_header(uint8_t *data, uint8_t offset){
  tcpPtr  tcpHdrPtr;
  sudoPtr sudoHdrPtr;
  unsigned short chkRes;
  uint8_t *chkMem;
  
  /* Allocate some space for TCP header and Psuedo header*/
  if(NULL == (tcpHdrPtr = (tcpPtr)malloc(sizeof(tcp))))
    error_out("TCP header pointer malloc");
  if(NULL == (sudoHdrPtr = (sudoPtr)malloc(sizeof(sudoTCP))))
    error_out("Sudo header pointer malloc");
  if(NULL == (chkMem = (uint8_t *)malloc(sizeof(uint8_t) * MAXTCPPDU)))
    error_out("Checksum pointer malloc");
  
  /* Build the headers*/
  build_tcp_hdr(data, offset, tcpHdrPtr);
  build_psuedo_hdr(data, sudoHdrPtr);

  build_tcp_chksum(data, offset, sudoHdrPtr, chkMem);

  /* Get the results of the checksum*/	      
  chkRes = in_cksum((unsigned short *)chkMem, ntohs(sudoHdrPtr->tcpLen) + SUDOHDRLEN);
    
  printf("\n\tTCP Header\n");
  printf("\t\tSource Port: ");
  tcpHdrPtr->srcPort == HTTPPORT ? 
    printf(" HTTP\n") : printf(": %hu\n", tcpHdrPtr->srcPort); /* Src prt num or HTTP*/
  printf("\t\tDest Port: ");
  tcpHdrPtr->dstPort == HTTPPORT ? 
    printf(" HTTP\n") : printf(": %hu\n", tcpHdrPtr->dstPort); /* Dest port num or HTTP*/
  printf("\t\tSequence Number: %u\n",     tcpHdrPtr->seqNum);      
  printf("\t\tACK Number: ");
  (tcpHdrPtr->hdrLenFlgs & ACKFLAG) == ACKFLAG ? 
    printf("%u\n", tcpHdrPtr->ackNum) : printf("<not valid>\n"); /* Check for validity*/

  printf("\t\tACK Flag: %s\n",     flag_check(tcpHdrPtr->hdrLenFlgs, ACKFLAG));
  printf("\t\tSYN Flag: %s\n",     flag_check(tcpHdrPtr->hdrLenFlgs, SYNFLAG));
  printf("\t\tRST Flag: %s\n",     flag_check(tcpHdrPtr->hdrLenFlgs, RSTFLAG));
  printf("\t\tFIN Flag: %s\n",     flag_check(tcpHdrPtr->hdrLenFlgs, FINFLAG));
  printf("\t\tWindow Size: %hu\n", tcpHdrPtr->winSize);
  printf("\t\tChecksum: ");
  chkRes == 0 ? printf("Correct ") : printf("Incorrect "); /* Checksum check*/
  printf("(0x%x)\n", tcpHdrPtr->chksum);
  free(tcpHdrPtr);
  free(sudoHdrPtr);
  free(chkMem);
  
  return;
}

/**
 *@fn:    print_udp_header
 *@brief: This function will print the UDP header infomation. The data is parsed in
          dataparser file.
 *@parms: uint8_t *
 *@rtrn:  void
**/
void print_udp_header(uint8_t *data){
  udpPtr udpHdrPtr;

  if(NULL == (udpHdrPtr = (udpPtr)malloc(sizeof(udp))))
    error_out("UDP header pointer malloc");

  build_udp_hdr(data, udpHdrPtr);

  printf("\n\tUDP Header\n");
  printf("\t\tSource Port: : %d\n", udpHdrPtr->srcPort);
  printf("\t\tDest Port: : %d\n",   udpHdrPtr->dstPort);
  free(udpHdrPtr);
  
  return;
}

/**
 *@fn:    error_out
 *@brief: This function will print error messages using perror if errno is set or 
          fprintf otherwise. The function is provided a message that can be used to
	  track the offending location. After printing the message to stderr the program
	  will exit with a failure code.
 *@parms: char *
 *@rtrn:  void
**/
void error_out(char *error){
  if(errno)
    perror(error);
  else
    fprintf(stderr, "%s\n",error);
  exit(EXIT_FAILURE);
}
