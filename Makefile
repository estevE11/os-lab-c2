CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

all: act1 act2 act3

act1: act1.c
	$(CC) $(CFLAGS) -o act1 act1.c

act2: act2.c
	$(CC) $(CFLAGS) -o act2 act2.c

act3: act3.c
	$(CC) $(CFLAGS) -o act3 act3.c

clean:
	rm -f act1 act2 act3