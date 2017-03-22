CC=gcc
CFLAGS= -Wall -W -Werror -g -lm

tmin: tmin.c
	$(CC) -o tmin tmin.c $(CFLAGS)

style: ~/bin/helpful_files/cpplint.py ${FILE} 
	python $< --filter=-runtime/references,-legal/copyright,-readability/streams,-runtime/explicit,-build/header_guard,-build/include tmin.cpp

clean:
	rm tmin
