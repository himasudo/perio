#include "../lib/ring.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#define CAPACITY  1024
#define COUNT     1000000

PERIO_RING_STATIC_CHECK(CAPACITY);

struct context {
    struct perio_ring *ring;
    int                error;
};

static void *producer(void *arg)
{
    struct context *ctx = arg;

    for (uintptr_t i = 0; i < COUNT; i++)
        while (perio_ring_push(ctx->ring, (void *)i) != 0)
            ;

    return NULL;
}

static void *consumer(void *arg)
{
    struct context *ctx = arg;
    uintptr_t expected = 0;

    while (expected < COUNT) {
        void *item;

        if (perio_ring_pop(ctx->ring, &item) != 0)
            continue;

        if ((uintptr_t)item != expected) {
            fprintf(stderr, "FAIL: expected %lu got %lu\n",
                    (unsigned long)expected, (unsigned long)(uintptr_t)item);
            ctx->error = 1;
            return NULL;
        }

        expected++;
    }

    return NULL;
}

int main(void)
{
    struct perio_ring ring;
    struct context    ctx;
    pthread_t         prod, cons;

    if (perio_ring_init(&ring, CAPACITY) != 0) {
        fprintf(stderr, "perio_ring_init failed\n");
        return 1;
    }

    ctx.ring  = &ring;
    ctx.error = 0;

    pthread_create(&prod, NULL, producer, &ctx);
    pthread_create(&cons, NULL, consumer, &ctx);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    perio_ring_destroy(&ring);

    if (ctx.error) {
        printf("FAIL\n");
        return 1;
    }

    printf("PASS: %d integers received in order\n", COUNT);
    return 0;
}