CC = gcc
CFLAGS = -Wall -g -Wextra

##
# Make depend 
# -Y skip standard libray paths
# -f output to file
# -s string
##
DEPFILE = .depends
DEPTOKEN = '\# MAKEDEPENDS.'
DEPFLAGS = -Y -f $(DEPFILE) -s $(DEPTOKEN)

PROG = uniq
OBJS = uniq.o readlongline.o
SRCS = uniq.c readlongline.c
HDRS = readlongline.h

LD = gcc
LDFLAGS = -Wall -Wextra -g -ansi -pedantic

all: $(PROG)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) -o $(PROG) $(OBJS)

all.o: $(SRCS)
	$(CC) $(CFLAGS) -c -o $(OBJS) $(SRCS)

$uniq.o: uniq.c
	$(CC) $(CFLAGS) -c -o $(OBJS) $(SRCS)

depend: 
	rm -f $(DEPFILE)
	make $(DEPFILE)
$(DEPFILE):
	@echo $(DEPTOKEN) > $(DEPFILE)
	makedepend $(DEPFLAGS) -- $(CFLAGS) -- $(SRCS) > /dev/null

test: uniq.c
	@echo Executing $(PROG) test 1.
	./$(PROG) <1_OneLine >1_Out
	diff -y 1_OneLine 1_Out || exit 0

	@echo
	@echo Executing $(PROG) test 2.
	./$(PROG) <2_TwoDifferentLines >2_Out
	diff -y 2_TwoDifferentLines 2_Out || exit 0

	@echo
	@echo Executing $(PROG) test 3.
	./$(PROG) <3_TwoSameLines >3_Out
	diff -y 3_TwoSameLines 3_Out || exit 0

	@echo
	@echo Executing $(PROG) test 4.
	./$(PROG) <4_LotsOfSameLines >4_Out
	diff -y 4_LotsOfSameLines 4_Out || exit 0

	@echo
	@echo Executing $(PROG) test 5.
	./$(PROG) <5_LotsOfSameLinesOneDifferent >5_Out
	diff -y 5_LotsOfSameLinesOneDifferent 5_Out || exit 0

	@echo
	@echo Executing $(PROG) test 6.
	./$(PROG) <6_ReallyLongLine >6_Out
	diff -y 6_ReallyLongLine 6_Out || exit 0

	@echo
	@echo Executing $(PROG) test 7.
	./$(PROG) <7_ThreeReallyLongLines >7_Out
	diff -y 7_ThreeReallyLongLines 7_Out || exit 0

	@echo
	@echo Executing $(PROG) test 8.
	./$(PROG) <8_ShrinkingLines >8_Out
	diff -y 8_ShrinkingLines 8_Out || exit 0

	@echo
	@echo Executing $(PROG) test 9.
	./$(PROG) <9_NoNewLineAtEOF >9_Out
	diff -y 9_NoNewLineAtEOF 9_Out || exit 0

clean:
	rm -f $(OBJS) *~ 
