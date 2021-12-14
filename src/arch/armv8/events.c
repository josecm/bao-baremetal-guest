
#include <events.h>
#include <pmu.h>

void events_global_enable() {
    pmu_start();
}

void events_global_disable() {
    pmu_stop();
}

void events_global_reset() {
    pmu_reset();
}

size_t events_counter_num() {
    return pmu_num_counters();
}

bool events_counter_set_event(size_t counter, size_t event) {
    if (counter >= pmu_num_counters() || event >= PMU_MAX_EVENT) {
        return false;
    }
    pmu_counter_set_event(counter, event);
    return true;
}

size_t events_counter_get_event(size_t counter) {
    return pmu_counter_get_event(counter);
}

unsigned long events_counter_get(size_t counter) {
    return pmu_counter_get(counter);
}

void events_counter_enable(size_t counter) {
    pmu_counter_enable(counter, true);
}

void events_counter_disable(size_t counter) {
    pmu_counter_enable(counter, false);
}

void events_counter_reset(size_t counter) {
    pmu_counter_set(counter, 0);
}

ssize_t events_get_fixed_counter(size_t event) {
    switch (event) {
        case PMU_EVENT_CYCLE_ID: return PMU_CYCLE_COUNTER_ID; 
        default: return -1;
    }
}

ssize_t events_counter_get_fixed_event(size_t counter) {
    switch (counter) {
        case PMU_CYCLE_COUNTER_ID: return PMU_EVENT_CYCLE_ID; 
        default: return -1;
    }
}
