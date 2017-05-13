CC=icc
CFLAGS= -Wall -W -Werror -g -lm -std=gnu99 -O3 -qopenmp -qopt-report=3 -qopt-report-phase=vec -o
FILE=gaussian

$(FILE): $(FILE).c
	$(CC) $(FILE).c $(CFLAGS) $(FILE)

style: ../cpplint.py $(FILE) 
	python $< --filter=-runtime/references,-legal/copyright,-readability/streams,-runtime/explicit,-build/header_guard,-build/include $(FILE).cpp


clean:
	rm $(FILE)
