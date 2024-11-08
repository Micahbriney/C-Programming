.PHONY: all clean

CC = gcc
CFLAGS = -Wall -pedantic -g

PROG = timeit
OBJS = main.o
SRCS = main.c
HDRS =

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CLFAGS) -o $(PROG) $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f *.o *.~

tests: $(PROG)
##---------- Failure Tests
	@echo Failure Test 1: Overflow
	-./$(PROG) 9223372036854775808
	@echo
	@echo Failure Test 2: Seconds are negative
	-./$(PROG) -1
	@echo
	@echo Failure Test 3: Underflow
	-./$(PROG) -9223372036854775809
	@echo
	@echo Failure Test 4: Non number
	-./$(PROG) Test
	@echo
	@echo Failure Test 5: No paramters
	-./$(PROG)
	@echo

##---------- Success Tests
	@echo Success Test 1: Zero seconds
	-./$(PROG) 0
	@echo
	@echo Success Test 2: Non Zero
	-./$(PROG) 5
	@echo
	@echo Success Test 3: Max seconds. Good luck...
	-./$(PROG) 9223372036854775807
