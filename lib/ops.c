#include "ops.h"

static const struct perio_ops *current_ops = NULL;

int perio_submit(int fd, void *buf, size_t len, uint64_t user_data) {
    if (!current_ops || !current_ops->submit) {
        return -1;
    }
    return current_ops->submit(fd, buf, len, user_data);
}

void perio_cancel(uint64_t user_data) {
    if (!current_ops || !current_ops->cancel) {
        return;
    }
    current_ops->cancel(user_data);
}

void perio_complete(uint64_t user_data, int result) {
    if (!current_ops || !current_ops->complete) {
        return;
    }
    current_ops->complete(user_data, result);
}

int perio_register_ops(const struct perio_ops *ops) {
    current_ops = ops;
    return 0;
}

const struct perio_ops *perio_get_ops(void) {
    return current_ops;
}