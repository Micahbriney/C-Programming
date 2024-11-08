.PHONY: all clean

CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic

LD = gcc
LDFLAGS = -g -Wall -Wextra -pedantic

#LIBCFLAGS  = -I ~pn-cs357/Given/Mush/include
#LIBLDFLAGS = -L ~pn-cs357/Given/Mush/lib ~pn-cs357/Given/Mush/lib64

# For local Machine
LIBCFLAGS  = -I .
LIBLDFLAGS = -L .

PROG = mush2
OBJS = main.o mush2.o
SRCS = main.c mush2.c
LIBS = -lmush
HDRS = mush2.h

all: $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(LIBLDFLAGS) -o $(PROG) $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(LIBCFLAGS) -c $<

depend:
	makedepend -Y $(SRCS)

clean:
	-rm -f *.o *.~
# DO NOT DELETE

main.o: mush2.h
