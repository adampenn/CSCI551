CC=gcc
CFLAGS= -Wall -W -Werror -g -lm -std=c99 -O1 -o
FILE=gaussian

$(FILE): $(FILE).c
	$(CC) $(FILE).c $(CFLAGS) $(FILE)

style: ~/bin/helpful_files/cpplint.py $(FILE) 
	python $< --filter=-runtime/references,-legal/copyright,-readability/streams,-runtime/explicit,-build/header_guard,-build/include $(FILE).cpp


clean:
	rm $(FILE)
