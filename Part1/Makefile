# NAME: Anthony Khoshrozeh
# EMAIL: akhoshrozeh@ucla.edu
# ID: 305366728


# variables
CC = gcc
CFLAGS = -Wall -Wextra -pthread


default: build

	
build:
	$(CC) $(CFLAGS) -o lab2_add lab2_add.c
	$(CC) $(CFLAGS) -o lab2_list lab2_list.c SortedList.c

tests:
	chmod +x tests.sh; \
	./tests.sh; \

graphs: build tests
	gnuplot lab2_add.gp; \
	gnuplot lab2_list.gp; \


dist: graphs
	tar -czvf lab2a-305366728.tar.gz lab2_add.c lab2_list.c SortedList.c SortedList.h README Makefile lab2_add-*.png lab2_list-*.png lab2_add.csv lab2_list.csv *.gp tests.sh


clean:
	rm -f *.tar.gz lab2_add lab2_list 