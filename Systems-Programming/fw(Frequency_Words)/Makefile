.PHONY: clean all

LD = gcc
LDFLAGS = -g -Wall -Wextra -pedantic -ansi

CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -ansi

PROG = fw
SRCS = main.c readLongWord.c avlBinSearchTree.c wordList.c
OBJS = main.o readLongWord.o avlBinSearchTree.o wordList.o
HDRS = main.h readLongWord.h avlBinSearchTree.h wordList.h

all: $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) -o $(PROG) $(OBJS)

###
# makedepends related settings
# -Y	Don't search standard library paths
###
depend:
	@echo Local Dependencies
	@makedepend --Y $(SRCS) $(HDRS)

tags: $(SRCS) $(HDRS)
	etags $(SRCS) $(HDRS)

test: $(PROG)
	@echo Executing $(PROG) Failure test 1 no files provided
	./$(PROG)

	@echo
	@echo Executing $(PROG) Failure test 2 no files provided
	./$(PROG) -n

	@echo
	@echo Executing $(PROG) Failure test 3 no files provided
	./$(PROG) -n 2

	@echo
	@echo Executing $(PROG) Success test 1
	./$(PROG) -n 3 main.c

	@echo
	@echo Executing $(PROG) Success test 2
	./$(PROG) -n 4 main.c readlongline.c

	@echo
	@echo Executing $(PROG) Success test 3
	./$(PROG) -n 15 <main.c

	@echo
	@echo Executing $(PROG) Success test 4
	./$(PROG) main.c

	@echo
	@echo Executing $(PROG) Success test 5
	./$(PROG) main.c readlongline.c

	@echo
	@echo Executing $(PROG) Success test 6
	./$(PROG) <main.c

	@echo
	@echo Executing $(PROG)  Missing file test 1
	./$(PROG) -n 11 test.c

	@echo
	@echo Executing $(PROG)  Missing file test 2
	./$(PROG) -n 12 test.c main.c

	@echo
	@echo Executing $(PROG)  Missing file test 3
	./$(PROG) -n 7 <test.c

	@echo
	@echo Executing $(PROG)  Missing file test 4
	./$(PROG) test.c

	@echo
	@echo Executing $(PROG)  Missing file test 5
	./$(PROG) test.c readlongline.c

	@echo
	@echo Executing $(PROG)  Missing file test 6
	./$(PROG) <test.c

##
# rm removes files listed after the command
# -rm ignores failure if nothing is found to remove
# -f forces removal of files, ignore missing file/argument, never prompt
# *.o all object files
# *.~ emacs backup files
##
clean:
	-rm -f *.o *.~

