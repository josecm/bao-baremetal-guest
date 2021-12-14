#include <events.h>

static bool counter_busy[EVENTS_ARCH_MAX_COUNTERS];

static bool counter_is_busy(size_t counter) {
    return counter_busy[counter] || events_arch_counter_is_fixed(counter);
}

ssize_t events_alloc_counter() {
    for (size_t i = 0; i < events_arch_counter_num(); i++) {
        if (!counter_is_busy(i)) {
            return i;
        }
    }
    return -1;
}
