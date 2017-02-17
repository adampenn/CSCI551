CC=mpicxx
CFLAGS=-use -Wall -W -Werror -g

mmnovec: mm.c
	$(CC) -o mmnovec mm.c $(CFLAGS) #-O2 #-no-vec

mmvec: mm.c
	$(CC) -o mmvec mm.c $(CFLAGS) -O2 -qopt-report=2 -qopt-report-phase=vec
