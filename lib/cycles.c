#include "cycles.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WARMUP_ITERS 1000

static double tsc_freq_hz = 0.0;

static double timespec_seconds(const struct timespec *t)
{
	return (double)t->tv_sec + (double)t->tv_nsec / 1e9;
}

uint64_t perio_tsc(void)
{
	unsigned int lo, hi, aux;

	asm volatile("rdtscp"
	             : "=a"(lo), "=d"(hi), "=c"(aux)
	             :
	             : "memory");

	return ((uint64_t)hi << 32) | lo;
}

void perio_timer_init(void)
{
	struct timespec t0, t1;
	uint64_t tsc0, tsc1;

	tsc0 = perio_tsc();
	clock_gettime(CLOCK_MONOTONIC, &t0);

	usleep(100000);

	clock_gettime(CLOCK_MONOTONIC, &t1);
	tsc1 = perio_tsc();

	double elapsed = timespec_seconds(&t1) - timespec_seconds(&t0);
	uint64_t ticks = tsc1 - tsc0;

	tsc_freq_hz = (double)ticks / elapsed;
}

double perio_ticks_to_ns(uint64_t ticks)
{
	if (tsc_freq_hz == 0.0) {
		fprintf(stderr,
		        "perio_ticks_to_ns: perio_timer_init() has not run\n");
		return 0.0;
	}

	return (double)ticks * 1e9 / tsc_freq_hz;
}

static int compare_u64(const void *a, const void *b)
{
	uint64_t x = *(const uint64_t *)a;
	uint64_t y = *(const uint64_t *)b;

	if (x < y)
		return -1;
	if (x > y)
		return 1;
	return 0;
}

struct perio_bench_result perio_bench(void (*fn)(void *arg), void *arg,
                                       unsigned iters)
{
	struct perio_bench_result result = {0};
	uint64_t *deltas;
	unsigned i;

	for (i = 0; i < WARMUP_ITERS; i++)
		fn(arg);

	deltas = malloc(iters * sizeof(*deltas));
	if (!deltas) {
		fprintf(stderr, "perio_bench: allocation failed\n");
		return result;
	}

	for (i = 0; i < iters; i++) {
		uint64_t start = perio_tsc();
		fn(arg);
		uint64_t end = perio_tsc();
		deltas[i] = end - start;
	}

	qsort(deltas, iters, sizeof(*deltas), compare_u64);

	uint64_t sum = 0;
	for (i = 0; i < iters; i++)
		sum += deltas[i];

	result.min = deltas[0];
	result.median = (iters % 2)
	                    ? deltas[iters / 2]
	                    : (deltas[iters / 2 - 1] + deltas[iters / 2]) / 2;
	result.mean = (double)sum / iters;
	result.iters = iters;

	free(deltas);
	return result;
}

void perio_bench_report(const char *name, struct perio_bench_result r)
{
	printf("%-24s  min %6lu ticks (%8.2f ns)  "
	       "median %6lu ticks (%8.2f ns)  "
	       "mean %8.1f ticks (%8.2f ns)  [n=%u]\n",
	       name,
	       (unsigned long)r.min, perio_ticks_to_ns(r.min),
	       (unsigned long)r.median, perio_ticks_to_ns(r.median),
	       r.mean, perio_ticks_to_ns((uint64_t)r.mean),
	       r.iters);
}