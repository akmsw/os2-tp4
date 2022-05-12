CC = gcc
CFLAGS = -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11 -fopenmp -O2

all: lab4

lab4: lab4.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f ./img/*.pgm ./img/mainDrawn.png *.txt lab4