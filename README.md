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
├── examples/
│   ├── dummy_backend.c    # vtable dispatch demo
│   └── ring_demo.c        # SPSC ring buffer end-to-end test
├── lib/
│   ├── ops.h              # backend vtable
│   ├── ops.c
│   ├── ring.h             # SPSC ring buffer
│   └── ring.c
└── tools/
    ├── layout.h        # struct memory layout inspector
    ├── layout.c
    └── layout_demo.c
```
## Components
 
**`lib/ops`** — backend dispatch vtable. Operations (submit, cancel,
complete) route through function pointers so the epoll and io_uring
backends can register themselves without changing call sites.
 
**`lib/ring`** — single-producer/single-consumer ring buffer with
acquire/release memory ordering. Fixed-size array, free-running
`uint32_t` head and tail counters, power-of-two capacity so index
wrapping is a mask rather than a division. No locks, no per-item
allocation. This is the same design io_uring uses for its submission
and completion queues.

## Roadmap

- [x] Struct memory layout tooling
- [x] Backend vtable (`lib/ops`)
- [x] SPSC ring buffer with acquire/release ordering (`lib/ring`)
- [ ] Cache-aware benchmarking
- [ ] Raw syscall wrappers
- [ ] Intrusive linked list / red-black tree
- [ ] Custom allocator
- [ ] epoll-based event loop
- [ ] io_uring backend


## License
 
MIT — see [LICENSE](LICENSE).
