#ifndef TOOLS_LAYOUT_H
#define TOOLS_LAYOUT_H

#include <stddef.h>

void print_layout(const char *struct_name,
    size_t struct_size,
    const char **field_names,
    size_t *field_offsets,
    size_t *field_sizes,
    int field_count);

#define FIELD_SIZE(TYPE, MEMBER) sizeof(((TYPE *)0)->MEMBER)

#endif