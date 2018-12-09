heat_pt: heat_pt.c
	gcc -pthread -std=c11 -o heat_pt heat_pt.c -lgd

test: heat_pt
	srun -c 20 ./heat_pt 0.30 200 15000 150 heat1_pt.gif 20

clean:
	rm -f *~ *.out *.exe heat_pt *#
