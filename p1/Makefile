<<<<<<< HEAD
CC=gcc
CFLAGS=-use -Wall -W -Werror -g
=======
CC=icc
CFLAGS=-use -Wall -W -Werror -g -O2
>>>>>>> f887ebcb09a5c0267ab86e8296f066a4d24fffe4

mmnovec: mm.c
	$(CC) -o mmnovec mm.c $(CFLAGS) -no-vec

<<<<<<< HEAD
#mmvec: mm.c
#	$(CC) -o mmvec mm.c $(CFLAGS) -O2 -qopt-report=2 -qopt-report-phase=vec
=======
mmvec: mm.c
	$(CC) -o mmvec mm.c $(CFLAGS) -qopt-report=2 -qopt-report-phase=vec

clean:
	rm mmnovec
>>>>>>> f887ebcb09a5c0267ab86e8296f066a4d24fffe4
