.PHONY: all clean

CC = gcc
CFLAGS = -g -Wall 
#-Werror

PROG   = memSim
SRCS   = memSim.c page_table.c tlb.c safeutil.c options.c address.c
OBJS   = memSim.o page_table.o tlb.o safeutil.o options.o address.o
HEADER = memSim.o page_table.h tlb.h safeutil.h options.h address.h

# MAXFRAMES = 256
MAXFRAMES = 3
MINFRAMES = 1
TESTFILE = optFurthest.txt
# TESTFILE = fifoLarge.txt
# TESTFILE = fifoSmall.txt
# TESTFILE = fifoPRA.txt

all:  $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $<

depends:
	@echo Regenerating local dependencies
	makedepend -Y $(SRCS)

fifo: clean memSim
	./memSim testcases/$(TESTFILE) $(MAXFRAMES) FIFO -v

lru: clean memSim
	./memSim testcases/$(TESTFILE) $(MAXFRAMES) LRU -v

opt: clean memSim
	./memSim testcases/$(TESTFILE) $(MAXFRAMES) OPT -v

testfails: clean memSim
	@echo ------------- Executing $(PROG) usage test 1 -------------
	-./$(PROG)
	@echo
	@echo ------------- Executing $(PROG) usage test 2 -------------
	-./$(PROG) $(TESTFILE)
	@echo
	@echo ------------- Executing $(PROG) usage test 3 -------------
	-./$(PROG) 172
	@echo
	@echo ------------- Executing $(PROG) usage test 4 -------------
	-./$(PROG) LRU
	@echo
	@echo ------------- Executing $(PROG) usage test 5 -------------
	-./$(PROG) $(TESTFILE) 48
	@echo
	@echo ------------- Executing $(PROG) usage test 6 -------------
	-./$(PROG) $(TESTFILE) OPT
	@echo
	@echo ------------- Executing $(PROG) usage test 7 -------------
	-./$(PROG) 241 FIFO
	@echo

testvalids: clean memSim
	@echo ------------- Executing $(PROG) valid test 1 -------------
	-./$(PROG) $(TESTFILE) 148 OPT
	@echo
	@echo ------------- Executing $(PROG) valid test 2 /w verbose --
	-./$(PROG) $(TESTFILE) 222 FIFO v
	@echo
	@echo ------------- Executing $(PROG) valid test 4 /w verbose --
	-./$(PROG) $(TESTFILE) 74 LRU -v
	@echo
	@echo ------------- Executing $(PROG) valid test 5 max frames --
	-./$(PROG) $(TESTFILE) $(MAXFRAMES) FIFO -v
	@echo
	@echo ------------- Executing $(PROG) valid test 6 min frames --
	-./$(PROG) $(TESTFILE) $(MINFRAMES) OPT -v
	@echo

clean:
	-rm -f $(PROG)
	-rm -f *.o
# DO NOT DELETE

memSim.o: options.h
page_table.o: page_table.h
tlb.o: tlb.h page_table.h
safeutil.o: options.h
options.o: options.h safeutil.h
