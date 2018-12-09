bank: bank.c
	gcc -std=c11 -pthread -o bank bank.c
bank2: bank2.c
	gcc -std=c11 -pthread -o bank2 bank2.c
banktest: bank
	./bank 2.2 5 5
banktest2: bank
	./bank 5.0 1 3

bank2test: bank2
	./bank2 10.0 3 2
bank2test2: bank2
	./bank2 0.0 5 3


clean:
	rm -f *.out *~ bank bank2 *#
