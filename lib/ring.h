#ifndef PERIO_LIB_RING_H
#define PERIO_LIB_RING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
 * perio_ring -- single-producer / single-consumer lock-free ring buffer.
 *
 * Ownership rules:
 *   - Only the producer writes tail.
 *   - Only the consumer writes head.
 *
 * Memory ordering pairs:
 *   producer RELEASE on tail  <->  consumer ACQUIRE on tail
 *   consumer RELEASE on head  <->  producer ACQUIRE on head
 *
 * capacity must be a non-zero power of two.
 * head and tail are free-running; they are never wrapped to capacity,
 * only masked at slot access: slots[tail & mask].
 */
struct perio_ring {
    void             **slots;
    uint32_t           capacity;
    uint32_t           mask;
    _Atomic uint32_t   head;
    _Atomic uint32_t   tail;
};

_Static_assert(sizeof(struct perio_ring) <= 64,
               "perio_ring must fit in one cache line");

/*
 * PERIO_RING_STATIC_CHECK -- verify a compile-time capacity constant.
 *
 *     PERIO_RING_STATIC_CHECK(1024);
 *     perio_ring_init(&r, 1024);
 */
#define PERIO_RING_STATIC_CHECK(cap)                              \
    _Static_assert(((cap) & ((cap) - 1)) == 0 && (cap) != 0,     \
                   "perio ring capacity must be a non-zero power of two")

int      perio_ring_init   (struct perio_ring *r, uint32_t capacity);
void     perio_ring_destroy(struct perio_ring *r);
int      perio_ring_push   (struct perio_ring *r, void *item);
int      perio_ring_pop    (struct perio_ring *r, void **item);
uint32_t perio_ring_count  (const struct perio_ring *r);
bool     perio_ring_empty  (const struct perio_ring *r);
bool     perio_ring_full   (const struct perio_ring *r);

#endif /* PERIO_LIB_RING_H */