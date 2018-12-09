all: test1 test2

test1: ragged.out
	./ragged.out 3 3 3

test2: ragged.out
	./ragged.out 1 2 3

ragged.out: ragged.c
	cc -std=c11 -pedantic -o ragged.out ragged.c

ragged.exe: ragged.c
	cc -std=c11 -pedantic -o ragged.exe ragged.c

clean:
	rm -f *.exe *~ *.o *.out
