#ifndef PERIO_LIB_OPS_H
#define PERIO_LIB_OPS_H

#include <stddef.h>
#include <stdint.h>

typedef int  (*submit_fn)(int fd, void *buf, size_t len, uint64_t user_data);
typedef void (*cancel_fn)(uint64_t user_data);
typedef void (*complete_fn)(uint64_t user_data, int result);

struct perio_ops {
    const char  *name;
    submit_fn    submit;
    cancel_fn    cancel;
    complete_fn  complete;
    int         (*init)(void);
    void        (*destroy)(void);
};

int                     perio_register_ops(const struct perio_ops *ops);
const struct perio_ops *perio_get_ops(void);

int  perio_submit(int fd, void *buf, size_t len, uint64_t user_data);
void perio_cancel(uint64_t user_data);
void perio_complete(uint64_t user_data, int result);

#endif