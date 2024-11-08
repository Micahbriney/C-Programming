#ifndef DATAPARSER_H_
#define DATAPARSER_H_

#define ACKFLAG     0x0010
#define RSTFLAG     0x0004
#define SYNFLAG     0x0002
#define FINFLAG     0x0001
#define IPLENNIBBLE 0x0F
#define HTTPPORT    80

#define ETHRHDRSIZE 14
#define ARPHDRSIZE  28
#define IPLENMUL    4
#define MINIPHDRLEN 20
#define IPOFFSET    ETHRHDRSIZE
#define SUDOHDRLEN  12
#define MAXTCPPDU   (65535 - MINIPHDRLEN)

typedef struct ethrHdr    *ethrPtr;
typedef struct arpHdr     *arpPtr;
typedef struct ipHdr      *ipPtr;
typedef struct icmpHdr    *icmpPtr;
typedef struct tcpHdr     *tcpPtr;
typedef struct udpHdr     *udpPtr;
typedef struct sudoTCPHdr *sudoPtr;

typedef struct ethrHdr{
  uint8_t  dstMAC[6];  /* 6 Bytes. First 6 Bytes of data*/
  uint8_t  srcMAC[6];  /* 6 Bytes. Follows destMAC*/
  uint16_t type;       /* 2 Bytes. Follows sourceMAC*/
  void*    layerThree; /* Point to layerThree header*/
} __attribute__((packed)) ethr;

typedef struct arpHdr{
  uint16_t hrdType;    /* 2 Bytes. Follows ethrHdr type*/
  uint16_t prtclType;  /* 2 Bytes. Follows hrdType*/
  uint8_t  hrdSize;    /* 1 Byte. Follows prtclType*/
  uint8_t  prtclSize;  /* 1 Byte. Follows hrdSize*/
  uint16_t optcode;    /* 2 Bytes. Follows prtclSize*/
  uint8_t  sndrMAC[6]; /* 6 Bytes. Follows optcode*/
  uint32_t sndrIP;     /* 4 Bytes. Follows senderMAC*/
  uint8_t  trgtMAC[6]; /* 6 Bytes. Follows senderIP*/
  uint32_t trgtIP;     /* 4 Bytes. Follows targetMAC*/
} __attribute__((packed)) arp;

typedef struct ipHdr{
  uint8_t  len;       /* Version is high nibble, len is low nibble * 4*/
  uint8_t  TOS;       /* 1 Byte. Follows version+len*/
  uint16_t PDULen;    /* 2 Bytes. Follows TOS*/
  uint16_t id;        /* 2 Bytes. Follows PDUlen*/
  uint16_t flags;     /* 2 Bytes. Follows id, includes flagoffsets*/
  uint8_t  TTL;       /* 1 Byte. Follows foffset*/
  uint8_t  protocol;  /* 1 Byte. Follows TTL*/
  uint16_t chksum;    /* 2 Bytes. Follows protocol*/
  uint32_t srcAddr;   /* 4 Bytes. Follows */
  uint32_t dstAddr;   /* 4 Bytes. Follows */
  void*    layerFour; /* Point to layerFour header*/
} __attribute__((packed)) ip;

typedef struct icmpHdr{
  uint8_t  type;         /* 1 Byte. Follow IP destAddr*/
  uint8_t  code;         /* 1 Byte. Follows type*/
  uint16_t chksum;       /* 2 Bytes. Follows code*/
  uint16_t id;           /* 2 Bytes. Follows checksum*/
  uint16_t seqNum;       /* 2 Bytes. Follows id*/
  uint8_t *data;         /* Variable size. Follows seqNum*/
} __attribute__((packed)) icmp;

typedef struct tcpHdr{
  uint16_t srcPort;    /* 2 Bytes. Follows IP destAddr*/
  uint16_t dstPort;    /* 2 Bytes. Follows sourcePort*/
  uint32_t seqNum;     /* 4 Bytes. Follows destPort*/
  uint32_t ackNum;     /* 4 Bytes. Follows seqNum*/
  /* 1st Nibble is hdrLen. hdrLen is high Nibble * 4 low*/
  /* ackFlag, rstFlag, synFlag, finFlag = xxx1 x111*/
  uint16_t hdrLenFlgs; /* Flags 2nd Nibble + next byte 1111 1111 1111*/
  uint16_t winSize;    /* 2 Bytes. Follows flags*/
  uint16_t chksum;     /* 2 Bytes. Follows winSize*/
  uint16_t urgentPtr;  /* 2 Bytes. Follows checksum*/
  uint64_t options;    /* 8 Bytes? Or leftovers. Follows urgentPtr*/
} __attribute__((packed)) tcp;

typedef struct udpHdr{
  uint16_t srcPort;    /* 2 Bytes. Follows IP destAddr*/
  uint16_t dstPort;    /* 2 Bytes. Follows sourcePort*/
  uint16_t len;        /* 2 Bytes. Follows destPort*/
  uint16_t chksum;     /* 2 Bytes. Follows len*/
  uint8_t *UDPpayload; /* Variable size. Follows checksum*/
} __attribute__((packed)) udp;

typedef struct sudoTCPHdr{
  uint32_t srcAddr; /* 4 Bytes. From IP srcAddr*/
  uint32_t dstAddr; /* 4 Bytes. From IP dstAddr*/
  uint8_t  rsvd;    /* 1 Byte. all zeros*/
  uint8_t  ipPrtcl; /* 1 Byte. From IP protocol*/
  uint16_t tcpLen;  /* 2 Bytes. Calculated*/
} __attribute__((packed)) sudoTCP;

void build_ethr_hdr(uint8_t *data, ethrPtr ethrHdrPtr, uint16_t *type);
void build_arp_hdr(uint8_t *data, arpPtr arpHdrPtr);
void build_ip_hdr(uint8_t *data, ipPtr ipHdrPtr);
void build_icmp_hdr(uint8_t *data, icmpPtr icmpHdrPtr);
void build_tcp_hdr(uint8_t *data, uint8_t offset, tcpPtr tcpHdrPtr);
void build_psuedo_hdr(uint8_t *data, sudoPtr sudoHdrPtr);
void build_tcp_chksum(uint8_t *data, uint8_t offset, sudoPtr sudoHdrPtr,
		      uint8_t *chkMem);
void build_udp_hdr(uint8_t *data, udpPtr udpHdrPtr);
char *get_mac_addr(uint8_t *macAddr);
char *get_ethr_type(uint16_t type);
char *get_arp_optcode(uint16_t type);
char *get_icmp_type(uint8_t type);
char *get_ip_addr(uint32_t ipAddr);
char *get_ip_protocol(uint8_t protocol);
char *flag_check(uint16_t hdrData, uint16_t flagMask);

#endif
