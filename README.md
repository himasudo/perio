# perio

A C library for async I/O on Linux, built on io_uring.

Submit reads, writes, and network operations without blocking a
thread per request. Completions come back through a small
callback-based API.

Started as a way to understand Linux I/O properly: struct layout and
the C memory model, the kernel's data structures and scheduler, and
how io_uring actually moves bytes.

## Build

```
make
./layout_demo
```

Requires gcc. Built and tested with `-Wall -Wextra -g`.

## Layout

```
perio/
├── Makefile
├── README.md
├── lib/               # data structures and allocator
└── tools/
    ├── layout.h        # struct memory layout inspector
    ├── layout.c
    └── layout_demo.c
```

## Roadmap

- Cache-aware benchmarking
- Raw syscall wrappers
- Intrusive linked list / red-black tree
- Custom allocator
- epoll-based event loop
- io_uring backend