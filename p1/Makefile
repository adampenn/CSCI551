CC=icc
CFLAGS=-use -Wall -W -Werror -g -O2

mmnovec: mm.c
	$(CC) -o mmnovec mm.c $(CFLAGS) -no-vec

mmvec: mm.c
	$(CC) -o mmvec mm.c $(CFLAGS) -qopt-report=2 -qopt-report-phase=vec

clean:
	rm mmnovec
