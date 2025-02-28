.PHONY: all clean cleano
# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019
# Updated by Micah Briney - February 2023

CC = gcc
CFLAGS = -g -Wall

LD = gcc
LDFLAGS = -g -Wall

LIBS = 

COMMON = networks.o gethostbyname.o pollLib.o safeUtil.o transport.o clienthandle.o
OBJS = cclient.o server.o
SRCS = cclient.c server.c gethostbyname.c networks.c safeUtil.c pollLib.c transport.c clienthandle.c

all:   cclient server

cclient: cclient.o $(COMMON)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

server: server.c $(COMMON)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

depend:
	makedepend -Y $(SRCS)

cleano:
	rm -f *.o

clean:
	-rm -f server cclient *.o *.~




# DO NOT DELETE

cclient.o: pollLib.h networks.h safeUtil.h transport.h clienthandle.h
server.o: networks.h transport.h clienthandle.h pollLib.h safeUtil.h
gethostbyname.o: gethostbyname.h
networks.o: networks.h gethostbyname.h
safeUtil.o: networks.h safeUtil.h
pollLib.o: networks.h safeUtil.h pollLib.h
transport.o: transport.h clienthandle.h safeUtil.h
clienthandle.o: clienthandle.h safeUtil.h networks.h transport.h
