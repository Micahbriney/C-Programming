.PHONY: all clean

LD = gcc
LDFLAGS = -g -Wall -pedantic

CC = gcc
CFLAGS = -g -Wall -pedantic

PROG1 = forkit 
PROG2 = tryit
SRCS  = forkit.c tryit.c
OBJS  = forkit.o tryit.o
HDRS  =

all: forkit tryit


forkit: forkit.o
	$(LD) $(LDFLAGS) -o $@ $<

tryit: tryit.o
	$(LD) $(LDFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

depend:
	makedepend -Y $(SRCS)

test1: $(PROG1)
	@echo Executing $(PROG1) Success test 1
	./$(PROG1)

test2: $(PROG2)
	@echo Executing $(PROG2) Failure test 1 usage error
	-./$(PROG2)

	@echo
	@echo Executing $(PROG2) Failure test 2 no usage error
	-./$(PROG2) command with args

	@echo
	@echo Executing $(PROG2) Failure test 3 no valid command provided
	-./$(PROG2) non-existant

	@echo
	@echo Executing $(PROG2) Failure test 4 no vaild command found
	-./$(PROG2) ls

	@echo
	@echo Executing $(PROG2) Success test 1 valid command found
	-./$(PROG2) /bin/ls

	@echo
	@echo Executing $(PROG2) Success test 1 valid command found
	-./$(PROG2) /bin/true

	@echo
	@echo Executing $(PROG2) Success/Failure test 1 valid command found
	-./$(PROG2) /bin/false

clean:
	-rm -f *.o *~
# DO NOT DELETE
