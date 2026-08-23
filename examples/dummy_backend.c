#include <stdio.h>
#include <stdint.h>
#include "../lib/ops.h"

static int dummy_init(void) {
    printf("dummy: init called\n");
    return 0;
}

static void dummy_destroy(void) {
    printf("dummy: destroy called\n");
}

static int dummy_submit(int fd, void *buf, size_t len, uint64_t user_data) {
    (void)buf;
    printf("dummy: submit called — fd=%d len=%zu user_data=%lu\n",
           fd, len, user_data);
    return 0;
}

static void dummy_cancel(uint64_t user_data) {
    printf("dummy: cancel called — user_data=%lu\n", user_data);
}

static void dummy_complete(uint64_t user_data, int result) {
    printf("dummy: complete called — user_data=%lu result=%d\n",
           user_data, result);
}

static const struct perio_ops dummy_ops = {
    .name     = "dummy",
    .init     = dummy_init,
    .destroy  = dummy_destroy,
    .submit   = dummy_submit,
    .cancel   = dummy_cancel,
    .complete = dummy_complete,
};

int main(void) {
    perio_register_ops(&dummy_ops);

    dummy_ops.init();

    perio_submit(3, NULL, 512, 1001);
    perio_cancel(1001);
    perio_complete(1001, 512);

    dummy_ops.destroy();
    return 0;
}