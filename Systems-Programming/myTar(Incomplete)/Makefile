.PHONY: all clean

CC = gcc
CFLAGS = -g -pedantic -Wall -Wextra

PROG = mytar
OBJS = options.o main.o modes.o
SRCS = options.c main.c modes.c
HDRS = options.h modes.h

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

depends:
	makedepend -Y $(SRCS)

tests: $(PROG)
##-------- Failure tests
	@echo Failure Test 1: Incorrect options.
	@echo Flags: xvS.
	@echo Flagmask: Failed Usage
	-./$(PROG) xvS testfile
	@echo Failure Test 2: Incorrect options.
	@echo Flags: xv.
	@echo Flagmask: Failed Usage
	-./$(PROG) xv testfile
	@echo Failure Test 3: Incorrect options.
	@echo Flags: xS.
	@echo Flagmask: Failed Usage
	-./$(PROG) xS testfile
	@echo Failure Test 4: Incorrect options.
	@echo Flags: x.
	@echo Flagmask: Failed Usage
	-./$(PROG) x testfile
	@echo Failure Test 5: Incorrect options.
	@echo Flags: vS.
	@echo Flagmask: Failed Usage
	-./$(PROG) vS testfile
	@echo Failure Test 6: Incorrect options.
	@echo Flags: v.
	@echo Flagmask: Failed Usage
	-./$(PROG) v testfile
	@echo Failure Test 7: Incorrect options.
	@echo Flags: S.
	@echo Flagmask: Failed Usage
	-./$(PROG) S testfile
	@echo Failure Test 8: Incorrect options.
	@echo Flags: f.
	@echo Flagmask: Failed Usage
	-./$(PROG) f testfile
	@echo Failure Test 9: Incorrect options.
	@echo Flags: .
	@echo Flagmask: Failed Usage
	-./$(PROG)  testfile
	@echo Failure Test 10: Incorrect options.
	@echo Flags: cvS.
	@echo Flagmask: Failed Usage
	-./$(PROG) cvS testfile
	@echo Failure Test 11: Incorrect options.
	@echo Flags: cv.
	@echo Flagmask: Failed Usage
	-./$(PROG) cv testfile
	@echo Failure Test 12: Incorrect options.
	@echo Flags: cS.
	@echo Flagmask: Failed Usage
	-./$(PROG) cS testfile
	@echo Failure Test 13: Incorrect options.
	@echo Flags: c.
	@echo Flagmask: Failed Usage
	-./$(PROG) c testfile
	@echo Failure Test 14: Incorrect options.
	@echo Flags: tvS.
	@echo Flagmask: Failed Usage
	-./$(PROG) tvS testfile
	@echo Failure Test 15: Incorrect options.
	@echo Flags: tv.
	@echo Flagmask: Failed Usage
	-./$(PROG) tv testfile
	@echo Failure Test 16: Incorrect options.
	@echo Flags: tS.
	@echo Flagmask: Failed Usage
	-./$(PROG) tS testfile
	@echo Failure Test 17: Incorrect options.
	@echo Flags: t.
	@echo Flagmask: Failed Usage
	-./$(PROG) t testfile

##---------- Successful flag tests
	@echo Success Test 1: Correct options.
	@echo Flags: xvSf.
	@echo Flagmask: 60
	-./$(PROG) xvSf testfile
	@echo Success Test 2: Correct options.
	@echo Flags: xvf.
	@echo Flagmask: 28
	-./$(PROG) xvf testfile
	@echo Success Test 3: Correct options.
	@echo Flags: xSf.
	@echo Flagmask: 52
	-./$(PROG) xSf testfile
	@echo Success Test 4: Correct options.
	@echo Flags: xf.
	@echo Flagmask: 20
	-./$(PROG) xf testfile
	@echo Success Test 5: Correct options.
	@echo Flags: vSf.
	@echo Flagmask: 56
	-./$(PROG) vSf testfile
	@echo Success Test 6: Correct options.
	@echo Flags: vf.
	@echo Flagmask: 24
	-./$(PROG) vf testfile
	@echo Success Test 7: Correct options.
	@echo Flags: Sf.
	@echo Flagmask: 48
	-./$(PROG) Sf testfile
	@echo Success Test 8: Correct options.
	@echo Flags: cvSf.
	@echo Flagmask: 57
	-./$(PROG) cvSf testfile
	@echo Success Test 9: Correct options.
	@echo Flags: cvf.
	@echo Flagmask: 25
	-./$(PROG) cvf testfile
	@echo Success Test 10: Correct options.
	@echo Flags: cSf.
	@echo Flagmask: 49
	-./$(PROG) cSf testfile
	@echo Success Test 11: Correct options.
	@echo Flags: cf.
	@echo Flagmask: 17
	-./$(PROG) cf testfile
	@echo Success Test 12: Correct options.
	@echo Flags: vSf.
	@echo Flagmask: 56
	-./$(PROG) vSf testfile
	@echo Success Test 13: Correct options.
	@echo Flags: tvSf.
	@echo Flagmask: 58
	-./$(PROG) tvSf testfile
	@echo Success Test 14: Correct options.
	@echo Flags: tvf.
	@echo Flagmask: 26
	-./$(PROG) tvf testfile
	@echo Success Test 15: Correct options.
	@echo Flags: tSf.
	@echo Flagmask: 50
	-./$(PROG) tSf testfile
	@echo Success Test 16: Correct options.
	@echo Flags: tf.
	@echo Flagmask: 18
	-./$(PROG) tf testfile


clean:
	-rm -f *.o *.~

# DO NOT DELETE

options.o: options.h
main.o: options.h modes.h
modes.o: options.h modes.h
