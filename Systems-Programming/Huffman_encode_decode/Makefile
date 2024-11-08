.PHONY: all clean

CC = gcc
CFLAGS = -ansi -Wall -Wextra -pedantic -g

PROG1 = hdecode
PROG2 = hencode
OBJS  = htable.o
SRCS  = htable.c hencode.c hdecode.c
HDRS  = htable.h
SRCS  = $(wildcard *.c)

all: $(PROG1) $(PROG2)

$(PROG1): $(PROG1).o $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG1) $^

$(PROG2): $(PROG2).o $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG2) $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

tags:
	etags $(SRCS)

depend:
	makedepend -Y $(SRCS)

testhencode: $(PROG2)

testhdecode: $(PROG1)
	@echo Executing $(PROG1)
	@echo Success test 1: Empty file.
	@echo Flag: none.
	@echo Input: 'empty' file via stdin
	@echo Output: Send to 'out' file via stdout
	./$(PROG1) <empty >out
	diff -y empty out || exit 0

	@echo
	@echo Executing $(PROG1)
	@echo Success test 2: Empty file.
	@echo Flag: '-' flag used.
	@echo Input: 'empty' file via stdin.
	@echo Output: Send to 'out' file via stdout
	./$(PROG1) - <empty >out
	diff -y empty out || exit 0

	@echo
	@echo Executing $(PROG1)
	@echo Success test 3: Empty file.
	@echo Flag: none.
	@echo Input: 'empty' file.
	@echo Output: Send to 'out' file via stdout
	./$(PROG1) empty >out
	diff -y empty out || exit 0

	@echo
	@echo Executing $(PROG1)
	@echo Success test 4: Empty file.
	@echo Flag: none.
	@echo Input: 'empty' file.
	@echo Output: 'out' file
	./$(PROG1) empty out
	diff -y empty out || exit 0

	@echo
	@echo Executing $(PROG1)
	@echo Success test 5: Empty file.
	@echo Flag: '-' flag used.
	@echo Input: 'empty' file via stdin
	@echo Output: Send to 'out' file via stdout
	./$(PROG1) - <empty out
	diff -y empty out || exit 0

################## Small test files ##############################

	@echo Executing $(PROG1)
	@echo Success test 6: Small file.
	@echo Flag: none.
	@echo Input: 'smallhencode' file via stdin
	@echo Output: Send to 'out' file via stdout
	./$(PROG1) <smallhencode >out
	diff -y small out || exit 0

	@echo
	@echo Executing $(PROG1)
	@echo Success test 7: Small file.
	@echo Flag: '-' flag used.
	@echo Input: 'smallhencode' file via stdin.
	@echo Output: Send to 'out' file via stdout
	./$(PROG1) - <smallhencode >out
	diff -y small out || exit 0

	@echo
	@echo Executing $(PROG1)
	@echo Success test 8: Small file.
	@echo Flag: none.
	@echo Input: 'smallhencode' file.
	@echo Output: Send to 'out' file via stdout
	./$(PROG1) smallhencode >out
	diff -y small out || exit 0

	@echo
	@echo Executing $(PROG1)
	@echo Success test 9: Small file.
	@echo Flag: none.
	@echo Input: 'smallhencode' file.
	@echo Output: 'out' file
	./$(PROG1) smallhencode out
	diff -y small out || exit 0

	@echo
	@echo Executing $(PROG1)
	@echo Success test 10: Small file.
	@echo Flag: '-' flag used.
	@echo Input: 'smallhencode' file via stdin
	@echo Output: Send to 'out' file via stdout
	./$(PROG1) - <smallhencode out
	diff -y small out || exit 0

clean:
	-rm -f *.o *.~
# DO NOT DELETE

hdecode.o: htable.h
htable.o: htable.h
hencode.o: htable.h
