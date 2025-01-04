.PHONY: all clean

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g
LD = gcc
LDFLAGS = -Wall -Wextra -pedantic

PROG = schedSim
OBJS = options.o schedSim.o safeutil.o
SRCS = options.c schedSim.c safeutil.c
HDRS = options.h schedSim.h safeutil.h

all: $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

depends:
	@echo Regenerating local dependencies.
	makedepend -Y $(SRCS)

clean:
	-rm -f $(OBJS)

testfails:
	@echo ------------- Executing $(PROG) usage test 1 -------------
	-./$(PROG)
	@echo
	@echo ------------- Executing $(PROG) usage test 2 -------------
	-./$(PROG) -q
	@echo
	@echo ------------- Executing $(PROG) usage test 3 -------------
	-./$(PROG) -q -p
	@echo
	@echo ------------- Executing $(PROG) usage test 4 -------------
	-./$(PROG) -q 5 -p RR
	@echo
	@echo ------------- Executing $(PROG) usage test 5 -------------
	-./$(PROG) -q 9
	@echo
	@echo ------------- Executing $(PROG) usage test 6 -------------
	-./$(PROG) -q 20 -p
	@echo
	@echo ------------- Executing $(PROG) usage test 7 -------------
	-./$(PROG) -p
	@echo
	@echo ------------- Executing $(PROG) usage test 8 -------------
	-./$(PROG) -p SRTN
	@echo

testinvalids:
	@echo ----------- Executing $(PROG) invalid option 1 -----------
	-./$(PROG) test_file.txt -p test
	@echo
	@echo ----------- Executing $(PROG) invalid option 2 -----------
	-./$(PROG) test_file.txt -q Not_an_algorithm
	@echo
	@echo ----------- Executing $(PROG) invalid option 3 -----------
	-./$(PROG) test_file.txt -p another -q Still_not_an_algorithm
	@echo
	@echo ----------- Executing $(PROG) invalid option 4 -----------
	-./$(PROG) test_file.txt -q Derp_algorithm -p number
	@echo

testvalids:
	@echo ----------- Executing $(PROG) valid test 1. Defaults -----------
	-./$(PROG) test_file.txt
	@echo
	@echo ----------- Executing $(PROG) valid test 2. Q 3 Default Algo ---
	-./$(PROG) test_file.txt -q 3
	@echo
	@echo ----------- Executing $(PROG) valid test 3. Q 6 Algo RR --------
	-./$(PROG) test_file.txt -q 6 -p RR
	@echo
	@echo ----------- Executing $(PROG) valid test 4. Q 2 Algo SRTN ------
	-./$(PROG) test_file.txt -q 2 -p SRTN
	@echo
	@echo ----------- Executing $(PROG) valid test 5. Algo srtn Q 4 ------
	-./$(PROG) test_file.txt -p srtn -q 4 
	@echo
	@echo ----------- Executing $(PROG) valid test 6. Default Algo FIFO --
	-./$(PROG) test_file.txt -p Not_an_algorithm
	@echo
	@echo ----------- Executing $(PROG) valid test 7. Explict Defaults ---
	-./$(PROG) test_file.txt -q 1 -p FIFO
	@echo
	@echo ----------- Executing $(PROG) valid test 8. Explict Defaults ---
	-./$(PROG) test_file.txt -p FIFO -q 1
	@echo
	@echo ----------- Executing $(PROG) valid test 1. Defaults -----------
	-./$(PROG) test_file.txt -v
	@echo
	@echo ----------- Executing $(PROG) valid test 2. Q 3 Default Algo ---
	-./$(PROG) test_file.txt -v -q 3
	@echo
	@echo ----------- Executing $(PROG) valid test 3. Q 6 Algo RR --------
	-./$(PROG) test_file.txt -q 6 -v -p RR
	@echo
	@echo ----------- Executing $(PROG) valid test 4. Q 2 Algo SRTN ------
	-./$(PROG) test_file.txt -q 2 -p SRTN -v
	@echo
	@echo ----------- Executing $(PROG) valid test 5. Algo srtn Q 4 ------
	-./$(PROG) test_file.txt -v -p rr -q 4 
	@echo
	@echo ----------- Executing $(PROG) valid test 6. Default Algo FIFO --
	-./$(PROG) test_file.txt -p Not_an_algorithm -v
	@echo
	@echo ----------- Executing $(PROG) valid test 7. Explict Defaults ---
	-./$(PROG) test_file.txt -q 1 -p FIFO -v
	@echo
	@echo ----------- Executing $(PROG) valid test 8. Explict Defaults ---
	-./$(PROG) test_file.txt -v -p FIFO -q 1
	@echo

testreals:
	@echo ----------- Executing $(PROG) valid test 9. Basic FIFO ---
	-./$(PROG) FIFO_test.txt
	@echo
	@echo ----------- Executing $(PROG) valid test 10. Basic RR ---
	-./$(PROG) RR_test.txt -p RR -q 4
	@echo
	@echo ----------- Executing $(PROG) valid test 11. Basic SRTN ---
	-./$(PROG) SRTN_test.txt -p SRTN
	@echo

# DO NOT DELETE

options.o: options.h schedSim.h
schedSim.o: options.h schedSim.h safeutil.h
