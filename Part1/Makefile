# variables
CC = gcc
CFLAGS = -Wall -Wextra -pthread


default: build

	
build:
	$(CC) $(CFLAGS) -o add add.c
	$(CC) $(CFLAGS) -o list list.c SortedList.c

tests:
	chmod +x tests.sh; \
	./tests.sh; \

graphs: build tests
	gnuplot add.gp; \
	gnuplot list.gp; \


dist: graphs
	tar -czvf lab2a.tar.gz add.c list.c SortedList.c SortedList.h README Makefile add-*.png list-*.png add.csv list.csv *.gp tests.sh


clean:
	rm -f *.tar.gz add list 
