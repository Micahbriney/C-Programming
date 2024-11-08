#include "messages.h"
#include "dataparser.h"


/**
 *@fn:    build_ethr_hdr
 *@brief: This function will populate the ethernet header. This is 
          done by copying the data and into the provided ehtrPtr struct 
	  using memcpy. The size is provided from a macro. It will also
	  put the type into network byte order.
 *@parms: uint8_t *, ethrPtr, uint16_t
 *@rtrn:  void
**/
void build_ethr_hdr(uint8_t *data, ethrPtr ethrHdrPtr, uint16_t *type){
  memcpy(ethrHdrPtr, data, ETHRHDRSIZE);
  
  /* Put type into network byte order. Leave MAC addresses alone.*/
  *type = ethrHdrPtr->type = htons(ethrHdrPtr->type);

  return; 
}

/**
 *@fn:    build_arp_hdr
 *@brief: This function will populate the ARP header. This is 
          done by copying the data and into the provided arpPtr struct 
	  using memcpy. The size is provided from a macro.
 *@parms: uint8_t *, arpPtr
 *@rtrn:  void
**/
void build_arp_hdr(uint8_t *data, arpPtr arpHdrPtr){
  memcpy(arpHdrPtr, data, ARPHDRSIZE);

  /* Put bytes in network byte order. Leaving MAC addresses alone.*/
  arpHdrPtr->optcode = htons(arpHdrPtr->optcode);
  arpHdrPtr->sndrIP  = htonl(arpHdrPtr->sndrIP);
  arpHdrPtr->trgtIP  = htonl(arpHdrPtr->trgtIP);

  return;
}

/**
 *@fn:    build_ip_hdr
 *@brief: This function will populate the IP header. This is 
          done by copying the data and into the provided ipPtr struct 
	  using memcpy. The size is provided from a macro.
 *@parms: uint8_t *, ipPtr
 *@rtrn:  void
**/
void build_ip_hdr(uint8_t *data, ipPtr ipHdrPtr){
  memcpy(ipHdrPtr, data, sizeof(ip));     /* Put data into ip header struct*/
   
  ipHdrPtr->PDULen  = htons(ipHdrPtr->PDULen);    /* Change to Network Byte order*/
  ipHdrPtr->srcAddr = htonl(ipHdrPtr->srcAddr);   /* Change to Network Byte order*/
  ipHdrPtr->dstAddr = htonl(ipHdrPtr->dstAddr);   /* Change to Network Byte order*/

  return;
}

/**
 *@fn:    build_icmp_hdr
 *@brief: This function will populate the ICMP header. This is 
          done by copying the data and into the provided icmpPtr struct 
	  using memcpy. The size is provided by using the size of the struct.
	  The chksum, id and seqNum are converted to network byte order. This 
	  isn't necessary for this program because they are not printed.
 *@parms: uint8_t, icmpPtr
 *@rtrn:  void
**/
void build_icmp_hdr(uint8_t *data, icmpPtr icmpHdrPtr){
  memcpy(icmpHdrPtr, data, sizeof(icmp));    /* Put data into icmp header struct*/
  
  /* Not used. Doing for completeness*/
  icmpHdrPtr->chksum = htons(icmpHdrPtr->chksum); /* Change to Network Byte order*/
  icmpHdrPtr->id     = htons(icmpHdrPtr->id);     /* Change to Network Byte order*/
  icmpHdrPtr->seqNum = htons(icmpHdrPtr->seqNum); /* Change to Network Byte order*/

  return;
}

/**
 *@fn:    build_tcp_hdr
 *@brief: This function will populate the TCP header. This is 
          done by copying the data and into the provided tcpPtr struct 
	  using memcpy. The size is provided by using the sizeof the struct.
	  Fields are converted to network byte order.
 *@parms: uint8_t *, icmpPtr
 *@rtrn:  void
**/
void build_tcp_hdr(uint8_t *data, uint8_t offset, tcpPtr tcpHdrPtr){
  memcpy(tcpHdrPtr, data + offset, sizeof(tcp));/* Put data into TCP header*/ 

  tcpHdrPtr->srcPort    = htons(tcpHdrPtr->srcPort);    /* Change to Network byte order*/
  tcpHdrPtr->dstPort    = htons(tcpHdrPtr->dstPort);    /* Change to Network byte order*/
  tcpHdrPtr->seqNum     = htonl(tcpHdrPtr->seqNum);     /* Change to Network byte order*/
  tcpHdrPtr->ackNum     = htonl(tcpHdrPtr->ackNum);     /* Change to Network byte order*/
  tcpHdrPtr->hdrLenFlgs = htons(tcpHdrPtr->hdrLenFlgs); /* Change to Network byte order*/
  tcpHdrPtr->winSize    = htons(tcpHdrPtr->winSize);    /* Change to Network byte order*/
  tcpHdrPtr->chksum     = htons(tcpHdrPtr->chksum);     /* Change to Network byte order*/
  tcpHdrPtr->urgentPtr  = htons(tcpHdrPtr->urgentPtr);  /* Change to Network byte order*/
  tcpHdrPtr->options    = htonl(tcpHdrPtr->options);    /* Change to Network byte order*/

  return;
}

/**
 *@fn:    build_psuedo_hdr
 *@brief: This function will populate the psuedo header. This is 
          done first by creating an ipHdrPtr. Then using some of that data to populate 
	  src, dest and protocol fields of the psuedo header. The reserved field is 0x00
	  and the TCP Length field is calculated and stored in network byte order. 
	            IP PDU length - IPHeader Length(LowNibble * Mask)
 *@parms: uint8_t *, sudoPtr
 *@rtrn:  void
**/
void build_psuedo_hdr(uint8_t *data, sudoPtr sudoHdrPtr){
  ipPtr   ipHdrPtr;
  uint8_t len;

  /* Allocate some space for IP Header and copy data into it*/
  if(NULL == (ipHdrPtr = (ipPtr)malloc(sizeof(ip))))
    error_out("IP header pointer malloc");
  build_ip_hdr(data, ipHdrPtr);

  memcpy(ipHdrPtr, data + IPOFFSET, sizeof(ip));/* Put data into ip header struct*/

  /* Build Psuedo header*/
  sudoHdrPtr->srcAddr = ipHdrPtr->srcAddr;             /* Store the IP src Address*/
  sudoHdrPtr->dstAddr = ipHdrPtr->dstAddr;             /* Store the IP dest Address*/
  sudoHdrPtr->rsvd    = 0x00;                          /* Reserved is just 0x00*/
  sudoHdrPtr->ipPrtcl = ipHdrPtr->protocol;            /* Store the IP Protocol*/
  len = (ipHdrPtr->len & IPLENNIBBLE) * IPLENMUL;      /* Calc len of IP header*/
  ipHdrPtr->PDULen    = htons(ipHdrPtr->PDULen);       /* Change to Network Byte order*/
  sudoHdrPtr->tcpLen  = htons(ipHdrPtr->PDULen - len); /* Calc len TCP PDU*/
  free(ipHdrPtr);
    
  return;
}

/**
 *@fn:    build_tcp_chksum
 *@brief: This function will build combine the psuedo header with the rest of the TCP 
          header and any payload. This works by performing a memcopy of the psuedo 
	  header. Next another the remaining data located at the offset will be appended
	  to the chkMem block. Make note of the htons of the tcpLen field. Also there
	  is need for one addition uint8_t, hence the + 1. 
 *@parms: uint8_t *, uint8_t, sudoPtr, uint8_t *
 *@rtrn:  void
**/
void build_tcp_chksum(uint8_t *data, uint8_t offset, sudoPtr sudoHdrPtr,
		      uint8_t *chkMem){
  /* Copy data from Psuedo header into checksum array.*/
  memcpy(chkMem, sudoHdrPtr, SUDOHDRLEN);

  /* Copy and append TCP header and datagram(if it exists)*/
  memcpy(chkMem + SUDOHDRLEN, data + offset, htons(sudoHdrPtr->tcpLen) + 1);

  return;
}

/**
 *@fn:    build_udp_hdr
 *@brief: This function will populate the udp header. This is done first by 
          copying the data into the struct. Next the fields are stored 
	  in network byte order. 
 *@parms: uint8_t *, udpPtr
 *@rtrn:  void
**/
void build_udp_hdr(uint8_t *data, udpPtr udpHdrPtr){
  memcpy(udpHdrPtr, data, sizeof(udp));
     
  udpHdrPtr->srcPort = htons(udpHdrPtr->srcPort); /* Change to Network Byte order*/
  udpHdrPtr->dstPort = htons(udpHdrPtr->dstPort); /* Change to Network Byte order*/
  udpHdrPtr->len     = htons(udpHdrPtr->len);     /* Change to Network Byte order*/
  udpHdrPtr->chksum  = htons(udpHdrPtr->chksum);  /* Change to Network Byte order*/

  return;
}

/**
 *@fn:    get_mac_addr
 *@brief: This function will create a printable mac address. This is 
          done by copying the mac address into a ether_addr
	  struct. Then using the ether_ntoa function to return the mac 
	  address as the perferred usable format for printing. The
	  function returns the printable mac address as a char pointer.
 *@parms: uint8_t
 *@rtrn:  char *
**/
char *get_mac_addr(uint8_t *macAddr){
  struct ether_addr addr;
  memcpy(addr.ether_addr_octet, macAddr, 6);
  return ether_ntoa(&addr);
}

/**
 *@fn:    get_ip_addr
 *@brief: This function will create a printable ip address. This is done
          by copying the ip address into a in_addr struct. Then using 
	  the inet_ntoa function to return the printable ip address as 
	  the perferred usable format for printing. The
	  function returns the printable ip address as a char pointer.
 *@parms: uint32_t
 *@rtrn:  char *
**/
char *get_ip_addr(uint32_t ipAddr){
  struct in_addr addr;
  addr.s_addr = htonl(ipAddr); /* change to network byte order*/ 
  return inet_ntoa(addr);
}

/**
 *@fn:    get_ethr_type
 *@brief: This function tests the ethernet type value and returns the
          string value equivilent for printing. Default case is not
	  established so it will error out. Case statements use enums so
	  more types can be quickly added.
 *@parms: uint16_t
 *@rtrn:  char *
**/
char *get_ethr_type(uint16_t type){
  switch(type){
  case IP:
    return "IP";
  case ARP:
    return "ARP";
  default:
    fprintf(stderr, "Something wrong with ethernet header type value");
    exit(EXIT_FAILURE);
  }
}

/**
 *@fn:    get_arp_opcode
 *@brief: This function tests the ARP optcode  value and returns the
          string value equivilent for printing. Default case response is 
	  not established so it will errorout. Case statements use enums. 
 *@parms: uint16_t
 *@rtrn:  char *
**/
char *get_arp_optcode(uint16_t optcode){
  switch(optcode){
  case ARPREQUEST:
    return "Request";
  case ARPREPLY:
    return "Reply";
  default:
    fprintf(stderr, "Something wrong with arp header optcode value");
    exit(EXIT_FAILURE);
  }
}

/**
 *@fn:    get_ip_protocol
 *@brief: This function tests the ip protocol value and returns the
          string value equivilent for printing. Default case returns
	  the string "Unknown" which can be tested agains. Case 
	  statements use enums so more types can be quickly added.
 *@parms: uint8_t
 *@rtrn:  char *
**/
char *get_ip_protocol(uint8_t protocol){
  switch(protocol){
  case ICMP:
    return "ICMP";
  case TCP:
    return "TCP";
  case UDP:
    return "UDP";
  default:
    return "Unknown";
  }
}

/**
 *@fn:    get_icmp_type
 *@brief: This function tests the icmp type value and returns the
          string value equivilent for printing. Default case returns
	  "Unknown" it can be checked against. Case statements use enums.
 *@parms: uint8_t
 *@rtrn:  char *
**/
char *get_icmp_type(uint8_t type){
  switch(type){
  case REQUEST:
    return "Request";
  case REPLY:
    return "Reply";
  default:
    return "Unknown";
  }
}

/**
 *@fn:    flag_check
 *@brief: This function will use a mask to check if a flag is set in the
          provided header data. It returns a char pointer "Yes" or "No".
 *@parms: uint16_t, uint16_t
 *@rtrn:  char *
**/
char *flag_check(uint16_t hdrData, uint16_t flagMask){
  return (hdrData & flagMask) == flagMask ? "Yes" : "No";
}
