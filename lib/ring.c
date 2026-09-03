#include "ring.h"

#include <stdlib.h>

static int is_power_of_2(uint32_t n)
{
    return n != 0 && (n & (n - 1)) == 0;
}

int perio_ring_init(struct perio_ring *r, uint32_t capacity)
{
    if (!is_power_of_2(capacity))
        return -1;

    r->slots = calloc(capacity, sizeof(void *));
    if (!r->slots)
        return -1;

    r->capacity = capacity;
    r->mask     = capacity - 1;
    __atomic_store_n(&r->head, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&r->tail, 0, __ATOMIC_RELAXED);

    return 0;
}

void perio_ring_destroy(struct perio_ring *r)
{
    free(r->slots);
    r->slots    = NULL;
    r->capacity = 0;
    r->mask     = 0;
    __atomic_store_n(&r->head, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&r->tail, 0, __ATOMIC_RELAXED);
}

int perio_ring_push(struct perio_ring *r, void *item)
{
    uint32_t tail = __atomic_load_n(&r->tail, __ATOMIC_RELAXED);
    uint32_t head = __atomic_load_n(&r->head, __ATOMIC_ACQUIRE);

    if (tail - head == r->capacity)
        return -1;

    r->slots[tail & r->mask] = item;
    __atomic_store_n(&r->tail, tail + 1, __ATOMIC_RELEASE);

    return 0;
}

int perio_ring_pop(struct perio_ring *r, void **item)
{
    uint32_t head = __atomic_load_n(&r->head, __ATOMIC_RELAXED);
    uint32_t tail = __atomic_load_n(&r->tail, __ATOMIC_ACQUIRE);

    if (head == tail)
        return -1;

    *item = r->slots[head & r->mask];
    __atomic_store_n(&r->head, head + 1, __ATOMIC_RELEASE);

    return 0;
}

uint32_t perio_ring_count(const struct perio_ring *r)
{
    uint32_t tail = __atomic_load_n(&r->tail, __ATOMIC_ACQUIRE);
    uint32_t head = __atomic_load_n(&r->head, __ATOMIC_ACQUIRE);
    return tail - head;
}

bool perio_ring_empty(const struct perio_ring *r)
{
    return perio_ring_count(r) == 0;
}

bool perio_ring_full(const struct perio_ring *r)
{
    return perio_ring_count(r) == r->capacity;
}