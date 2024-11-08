CC = gcc
CFLAGS = -Wall -Wextra -g -pg

LD = gcc
LDFLAGS = -Wall -Wextra -g -pedantic

PROG1 = hencode
PROG2 = hdecode

PROG1O = hencode.o
PROG2O = hdecode.o

SHAREDO = htable.o
OBJS1 = encode.o $(SHAREDO)
OBJS2 = decode.o $(SHAREDO)

SHAREDC = htable.c
SRCS1 = encode.c $(SHAREDC)
SRCS2 = decode.c $(SHAREDC)

all: $(PROG1) $(PROG2)

$(PROG1): $(OBJS1) 
	$(LD) $(LDFLAGS) -o $@ $^

$(PROG2): decode.o $(SHAREDO)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c $(SHAREDC)
	$(CC) $(CFLAGS) -c $<

$(PROG2O): decode.c $(SHAREDC)
	$(CC) $(CFLAGS) -c -o $(OBJS1) $(SRCS1)

$(PROG2O): decode.c $(SHAREDC)
	$(CC) $(CFLAGS) -c -o $(OBJS2) $(SRCS2)

testen: hencode
	@echo Executing $(PROG1)
	./$(PROG1) in out

testde: hdecode
	@echo Executing $(PROG2)
	./$(PROG2) out decoded

clean:
	rm -f $(OBJS1)
	rm -f $(OBJS2)
