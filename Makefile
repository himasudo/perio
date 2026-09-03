CC     = gcc
CFLAGS = -Wall -Wextra -g

all: layout_demo dummy_backend ring_demo

layout_demo: tools/layout_demo.c tools/layout.c tools/layout.h
	$(CC) $(CFLAGS) tools/layout_demo.c tools/layout.c -o layout_demo

dummy_backend: examples/dummy_backend.c lib/ops.c lib/ops.h
	$(CC) $(CFLAGS) examples/dummy_backend.c lib/ops.c -o dummy_backend

ring_demo: examples/ring_demo.c lib/ring.c lib/ring.h
	$(CC) $(CFLAGS) -pthread examples/ring_demo.c lib/ring.c -o ring_demo

clean:
	rm -f layout_demo dummy_backend ring_demo