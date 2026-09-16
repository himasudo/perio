#ifndef PERIO_LIB_CYCLES_H
#define PERIO_LIB_CYCLES_H

#include <stdint.h>

/*
 * perio_cycles -- a TSC-based cycle/time measurement harness.
 *
 * Read this before using it:
 *
 *   - Call perio_timer_init() once, at startup, before any other function
 *     in this file. It calibrates the TSC frequency, which every tick->ns
 *     conversion depends on. It costs about 100ms; do not call it per
 *     measurement.
 *
 *   - perio_tsc() is a serialising read of the TSC (rdtscp). It is safe
 *     to use as a measurement boundary. A plain rdtsc is not.
 *
 *   - perio_bench() warms up before it measures. The first several calls
 *     to any function pay for cold caches and branch predictors that have
 *     never seen it before -- that cost is real but is not what you are
 *     trying to measure.
 *
 *   - The harness has its own floor: two perio_tsc() reads plus one
 *     indirect call through a function pointer cost something, typically
 *     tens of ticks. A measurement near that floor is not trustworthy on
 *     its own -- it should be read relative to the empty-function
 *     baseline, not as an absolute number.
 */

struct perio_bench_result {
	uint64_t min;      
	uint64_t median;   
	double   mean;     
	unsigned iters;
};

/* Measure and cache the TSC frequency. Call once, before anything else
 * in this file. Safe to call again later; it just re-measures. */
void perio_timer_init(void);

/* A serialising TSC read: rdtscp, combined into one 64-bit value. */
uint64_t perio_tsc(void);

/* Run fn(arg) iters times, timing each call, after a warm-up phase.
 * Returns min/median/mean in raw ticks. */
struct perio_bench_result perio_bench(void (*fn)(void *arg), void *arg,
                                       unsigned iters);

/* Convert a tick count to nanoseconds. Requires perio_timer_init() to
 * have already run; if it has not, this returns 0.0 and prints a
 * warning to stderr. */
double perio_ticks_to_ns(uint64_t ticks);

/* Print one line: name, then min/median/mean in both ticks and ns. */
void perio_bench_report(const char *name, struct perio_bench_result r);

#endif