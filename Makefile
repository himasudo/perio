CC     = gcc
CFLAGS = -Wall -Wextra -g

all: layout_demo

layout_demo: tools/layout_demo.c tools/layout.c tools/layout.h
	$(CC) $(CFLAGS) tools/layout_demo.c tools/layout.c -o layout_demo

clean:
	rm -f layout_demo