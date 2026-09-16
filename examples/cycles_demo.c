#include "../lib/cycles.h"

#include <stdint.h>
#include <stdio.h>

#define REPS  1000

static volatile uint64_t sink;
static volatile uint64_t divisor = 7;

static void empty_fn(void *arg)
{
	(void)arg;
}

static void add_fn(void *arg)
{
	(void)arg;
	uint64_t v = 0;

	for (int i = 0; i < REPS; i++)
		asm volatile("add $1, %0" : "+r"(v) : : "cc");

	sink = v;
}

static void div_fn(void *arg)
{
	(void)arg;
	uint64_t v = 123456789;
	uint64_t d = divisor;

	for (int i = 0; i < REPS; i++)
		v = v / d + 1;

	sink = v;
}

static void chain_fn(void *arg)
{
	(void)arg;
	uint64_t a = 1, b = 2, c = 3, d = 4;

	for (int i = 0; i < REPS; i++) {
		a += b;
		b += c;
		c += d;
		d += a;
	}

	sink = a + b + c + d;
}

static struct perio_bench_result per_op(struct perio_bench_result r)
{
	struct perio_bench_result out;

	out.min    = r.min / REPS;
	out.median = r.median / REPS;
	out.mean   = r.mean / REPS;
	out.iters  = r.iters;

	return out;
}

int main(void)
{
	const unsigned iters = 20000;

	perio_timer_init();

	struct perio_bench_result floor_r = perio_bench(empty_fn, NULL, iters);
	struct perio_bench_result add_r   = perio_bench(add_fn, NULL, iters);
	struct perio_bench_result div_r   = perio_bench(div_fn, NULL, iters);
	struct perio_bench_result chain_r = perio_bench(chain_fn, NULL, iters);

	printf("measurement floor -- one harness call, no work inside it:\n");
	perio_bench_report("empty function", floor_r);

	printf("\nraw batch cost -- %d operations per timed call:\n", REPS);
	perio_bench_report("add xN", add_r);
	perio_bench_report("divide xN", div_r);
	perio_bench_report("dependent chain xN", chain_r);

	printf("\nper-operation cost -- batch divided by %d:\n", REPS);
	perio_bench_report("integer add", per_op(add_r));
	perio_bench_report("integer divide", per_op(div_r));
	perio_bench_report("one step of the dependent chain", per_op(chain_r));

	return 0;
}