CC = gcc

# Note: we use -std=gnu11 rather than -std=c11 in order to use the
# sigjmp_buf data type
CFLAGS = -g -Wall -Wextra -pedantic -std=gnu11

%.o : %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o -lm

all : fixedpoint_tests

fixedpoint_tests : fixedpoint.o fixedpoint_tests.o tctest.o
	$(CC) -o $@ fixedpoint.o fixedpoint_tests.o tctest.o -lm

fixedpoint.o : fixedpoint.c fixedpoint.h

fixedpoint_tests.o : fixedpoint_tests.c fixedpoint.h tctest.h -lm

tctest.o : tctest.c tctest.h -lm

clean :
	rm -f fixedpoint_tests *.o
