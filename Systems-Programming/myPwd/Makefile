LD = gcc
LDFLAGS = -Wall -Wextra -pedantic
CC = gcc
CFLAGS = -Wall -Wextra -g

PROG = mypwd
OBJS = mypwd.o
SRCS = mypwd.c

all: $(PROG)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) -o $(PROG) $(OBJS)

all.o: $(SRCS)
	$(CC) $(CFLAGS) -c -o $(OBJS) $(SRCS)

test: $(PROG)
	@echo Executing $(PROG)
	-./$(PROG)

clean:
	-rm -f $(OBJS) *~
