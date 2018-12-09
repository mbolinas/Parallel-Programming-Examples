test: pin_pt
	echo Running example pin locally, threads = 20, count = 1000000
	./p.exe 20 1000000

pin_pt: pin_pt.c
	gcc -std=c99 -pthread -o p.exe pin_pt.c -lm

clean:
	rm -f *.out *~ *.exe
