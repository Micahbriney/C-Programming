.PHONY: all clean
# Example makefile for CPE464 program 1
#
# 

CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic

LD = gcc
LDFLAGS = -g -Wall -Wextra -pedantic

LIBCFLAGS  =
LIBLDFLAGS =


PROG = trace
OBJS = trace.o messages.o dataparser.o checksum.o
SRCS = trace.c messages.c dataparser.c checksum.c
LIBS = -lpcap
HDRS = checksum.h messages.h dataparser.h
#CFLAGS = -g

all: $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(LIBLDFLAGS) -o $(PROG) $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(LIBCFLAGS) -c $<

depend:
	makedepend -Y $(SRCS)

test: $(PROG).c
	@echo Executing $(PROG) Arp Test
	-./$(PROG) ArpTest.pcap >ArpTest.out
	-diff ArpTest.pcap.out ArpTest.out
	@echo
	@echo Executing $(PROG) Http Test
	-./$(PROG) Http.pcap >Http.out
	-diff Http.pcap.out Http.out
	@echo
	@echo Executing $(PROG) IP_bad_checksum Test
	-./$(PROG) IP_bad_checksum.pcap >IP_bad_checksum.out
	-diff IP_bad_checksum.pcap.out IP_bad_checksum.out
	@echo
	@echo Executing $(PROG) largeMix Test
	-./$(PROG) largeMix.pcap >largeMix.out
	-diff largeMix.pcap.out largeMix.out
	@echo
	@echo Executing $(PROG) largeMix2 Test
	-./$(PROG) largeMix2.pcap >largeMix2.out
	-diff largeMix2.pcap.out largeMix2.out
	@echo
	@echo Executing $(PROG) mix_withIPoptions Test
	-./$(PROG) mix_withIPoptions.pcap >mix_withIPoptions.out
	-diff mix_withIPoptions.pcap.out mix_withIPoptions.out
	@echo
	@echo Executing $(PROG) PingTest Test
	-./$(PROG) PingTest.pcap >PingTest.out
	-diff PingTest.pcap.out PingTest.out
	@echo
	@echo Executing $(PROG) smallTCP Test
	-./$(PROG) smallTCP.pcap >smallTCP.out
	-diff smallTCP.pcap.out smallTCP.out
	@echo
	@echo Executing $(PROG) TCP_bad_checksum Test
	-./$(PROG) TCP_bad_checksum.pcap >TCP_bad_checksum.out
	-diff TCP_bad_checksum.pcap.out TCP_bad_checksum.out
	@echo

clean:
	-rm -f *.o *.~ trace
# DO NOT DELETE

trace.o: dataparser.h messages.h
messages.o: messages.h dataparser.h
dataparser.o: messages.h
