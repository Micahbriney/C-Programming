.PHONY: all clean cleano
# Makefile for CPE464 tcp test code
# Written by Micah Briney - October 2024

CC = gcc
CFLAGS = -g -Wall

LD = gcc
LDFLAGS = -g -Wall

LIBS = 

COMMON = database.o
OBJS = database.o
SRCS = database.c

all:   database

database: database.o $(COMMON)
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
