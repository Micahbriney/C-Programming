.PHONY: all clean cleano
# Makefile for database.c
# Written by Micah Briney - October 2024
# Updated - June 2025 added readlongline

CC = gcc
CFLAGS = -g -Wall

LD = gcc
LDFLAGS = -g -Wall

LIBS = 

COMMON = database.o readlongline.o
OBJS = database.o readlongline.o
SRCS = database.c redlongline.c

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
