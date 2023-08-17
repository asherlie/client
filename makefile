CC=gcc
CFLAGS= -Wall -Wextra -Wpedantic -Werror -O3

all: c

c: c.c

.PHONY:
clean:
	rm -f c
