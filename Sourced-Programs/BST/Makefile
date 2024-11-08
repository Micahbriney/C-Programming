.PHONY: clean all

LD = gcc
LDFLAGS = -g -Wall -Wextra -pedantic -ansi

CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -ansi

PROG = avl
OBJS = main.o bistree.o bitree.o
SRCS = main.c bistree.c bitree.c

##
# patsubst, string subsitution. $(patsubst %suffix, %replacement, $(var)
# include directive suspends reading of makefile to read prerequisites
# "-include" ignores failure to recreate or non existent files 
# -MF specify file dependicies writen to. /w -MMD Override default dep file
# "-MF -" will write dependicies (deps) to stdout  
# -MMD .d files are created from .o file names. No system header files
# $(@: .o=.d) another string sub. Will expand to replace @ with target %.o
##
DEPS := $(patsubst %.o, %.d, %(OBJS))
-include $(DEPS)
DEPFLAGS = -MMD -MF $(@: .o=.d)

all: $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) -o $(PROG) $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c %< $(DEPFLAGS)

main.o: main.c bistree.h bitree.h
	$(CC) $(CFLAGS) -c main.c

bistree.o: bistree.c bistree.h
	$(CC) $(CFLAGS) -c bistree.c

bitree.o: bitree.c bitree.h
	$(CC) $(CFLAGS) -c bitree.c

##
# rm removes files listed after the command
# -rm ignores failure if nothing is found to remove
# -f forces removal of files, ignore missing file/argument, never prompt
# *.o all object files
# *.~ emacs backup files
##
clean:
	-rm -f *.o *.~

