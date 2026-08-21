#include <stddef.h>
#include <stdio.h>

#include "layout.h"

struct foo {
	char a;
	int  b;
	char c;
	long d;
};

struct bar {
	long  b;
	int   c;
	short e;
	char  a;
	char  d;
};

struct compact {
	int  a;
	int  b;
	long c;
};

int main(void)
{
	printf("\nstruct layout tool - shows padding the compiler inserts\n");
	printf("#  field byte   :  padding byte   .  belongs to another row\n");

	const char *foo_names[] = { "char a", "int b", "char c", "long d" };
	size_t foo_offsets[] = {
		offsetof(struct foo, a), offsetof(struct foo, b),
		offsetof(struct foo, c), offsetof(struct foo, d),
	};
	size_t foo_sizes[] = {
		FIELD_SIZE(struct foo, a), FIELD_SIZE(struct foo, b),
		FIELD_SIZE(struct foo, c), FIELD_SIZE(struct foo, d),
	};
	print_layout("struct foo (bad order)", sizeof(struct foo),
		     foo_names, foo_offsets, foo_sizes, 4);

	const char *bar_names[] = { "long b", "int c", "short e", "char a", "char d" };
	size_t bar_offsets[] = {
		offsetof(struct bar, b), offsetof(struct bar, c),
		offsetof(struct bar, e), offsetof(struct bar, a),
		offsetof(struct bar, d),
	};
	size_t bar_sizes[] = {
		FIELD_SIZE(struct bar, b), FIELD_SIZE(struct bar, c),
		FIELD_SIZE(struct bar, e), FIELD_SIZE(struct bar, a),
		FIELD_SIZE(struct bar, d),
	};
	print_layout("struct bar (largest-first)", sizeof(struct bar),
		     bar_names, bar_offsets, bar_sizes, 5);

	const char *compact_names[] = { "int a", "int b", "long c" };
	size_t compact_offsets[] = {
		offsetof(struct compact, a), offsetof(struct compact, b),
		offsetof(struct compact, c),
	};
	size_t compact_sizes[] = {
		FIELD_SIZE(struct compact, a), FIELD_SIZE(struct compact, b),
		FIELD_SIZE(struct compact, c),
	};
	print_layout("struct compact (already tight)", sizeof(struct compact),
		     compact_names, compact_offsets, compact_sizes, 3);

	printf("\nsame 5 fields, foo vs bar: reordering large-to-small saves ");
	printf("%zu bytes for free.\n\n", sizeof(struct foo) - sizeof(struct bar));

	return 0;
}