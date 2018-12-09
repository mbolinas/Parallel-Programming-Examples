SEMLIBDIR = ../sem
SEMLIB = $(SEMLIBDIR)/sem.o
LIBS = $(SEMLIB) -lpthread 
CC = cc -std=c11 -Wall -pedantic -I$(SEMLIBDIR)

run_md: spscmd.exec
	./spscmd.exec 5

spscmd.exec: spscmd.c $(SEMLIB)
	$(CC) -o spscmd.exec spscmd.c $(LIBS)

all: run_spscsd

$(SEMLIB): $(SEMLIBDIR)/sem.h $(SEMLIBDIR)/sem.c $(SEMLIBDIR)/Makefile
	make -C $(SEMLIBDIR)

spscsd.exec: spscsd.c $(SEMLIB)
	$(CC) -o spscsd.exec spscsd.c $(LIBS)

run_spscsd: spscsd.exec
	./spscsd.exec

clean:
	rm -f *~ *.tmp *.exec *#
