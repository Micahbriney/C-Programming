.PHONY: all clean

CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic

LD = gcc
LDFLAGS = -g -Wall -Wextra -pedantic

LIBCFLAGS  = -I ~pn-cs357/Given/Talk/include
LIBLDFLAGS = -L ~pn-cs357/Given/Talk/lib64

# For local machine
#LIBCFLAGS  = -I .
#LIBLDFLAGS = -L .

PROG = mytalk
OBJS = main.o options.o modes.o mytalk.o
SRCS = main.c options.c modes.c mytalk.c
LIBS = -ltalk -lncurses
HDRS = options.h modes.h mytalk.h
# For local machine
#HDRS = options.h talk.h modes.h mytalk.h

all: $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(LIBLDFLAGS) -o $(PROG) $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(LIBCFLAGS) -c $< 


depends:
	makedepend -Y $(SRCS)

clean:
	-rm -f *.o *.~
# DO NOT DELETE

main.o: options.h modes.h mytalk.h
options.o: options.h
modes.o: modes.h
