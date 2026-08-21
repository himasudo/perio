#include <stdio.h>

#include "layout.h"

#define BAR_MAX_BYTES 48

#define CH_RULE  '-'
#define CH_USED  '#'
#define CH_PAD   ':'
#define CH_EMPTY '.'

#define PAD_LABEL "-- padding --"

static void print_rule(int width)
{
	for (int i = 0; i < width; i++)
		putchar(CH_RULE);
	putchar('\n');
}

static void print_bar(size_t struct_size, size_t offset, size_t size, int is_padding)
{
	char fill = is_padding ? CH_PAD : CH_USED;

	if (struct_size > BAR_MAX_BYTES)
		return;

	printf("  [");
	for (size_t i = 0; i < struct_size; i++)
		putchar(i >= offset && i < offset + size ? fill : CH_EMPTY);
	putchar(']');
}

static void print_row(size_t struct_size, size_t offset, size_t size,
		      const char *label, int is_padding)
{
	printf("  offset %3zu   size %3zu   %-16s", offset, size, label);
	print_bar(struct_size, offset, size, is_padding);
	putchar('\n');
}

void print_layout(const char *struct_name,
		  size_t struct_size,
		  const char **field_names,
		  size_t *field_offsets,
		  size_t *field_sizes,
		  int field_count)
{
	size_t cursor  = 0;
	size_t used    = 0;
	size_t padding = 0;
	int    width   = struct_size > BAR_MAX_BYTES ? 46 : 46 + (int)struct_size;

	printf("\n%s  [%zu bytes]\n", struct_name, struct_size);
	print_rule(width);

	for (int i = 0; i < field_count; i++) {
		if (cursor < field_offsets[i]) {
			size_t gap = field_offsets[i] - cursor;

			print_row(struct_size, cursor, gap, PAD_LABEL, 1);
			padding += gap;
		}

		print_row(struct_size, field_offsets[i], field_sizes[i],
			  field_names[i], 0);

		used   += field_sizes[i];
		cursor  = field_offsets[i] + field_sizes[i];
	}

	if (cursor < struct_size) {
		size_t gap = struct_size - cursor;

		print_row(struct_size, cursor, gap, PAD_LABEL, 1);
		padding += gap;
	}

	print_rule(width);
	printf("  %zu bytes total  -  %zu used  -  %zu padding",
	       struct_size, used, padding);

	if (struct_size > 0)
		printf("  (%.0f%% wasted)", 100.0 * (double)padding / (double)struct_size);

	printf("\n");
}