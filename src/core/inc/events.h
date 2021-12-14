

#ifndef EVENTS_H_
#define EVENTS_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <arch/events.h>

enum event_std {
    EVENT_CYCLES,
    EVENT_INSTRET,
    EVENT_STD_MAX
};

#define INVALID

void events_arch_global_enable();
void events_arch_global_disable();
void events_arch_global_reset();
size_t events_arch_counter_num();
bool events_arch_counter_set_event(size_t event, size_t counter);
size_t events_arch_counter_get_event(size_t counter);
unsigned long events_arch_counter_get(size_t counter);
void events_arch_counter_reset();
void events_arch_counter_enable(size_t counter);
void events_arch_counter_disable(size_t counter);
void events_arch_counter_reset(size_t counter);

ssize_t events_arch_get_fixed_counter(size_t event);
ssize_t events_arch_counter_get_fixed_event(size_t counter);


// char const * const events_event_name(size_t event);

// TODO:
// typedef void (*events_threashold_callback_fn)(void);
// bool events_set_threshold(event_id_t id, unsigned long threshold);
// bool events_set_threshold_callback(event_id_t id, events_threashold_callback_fn callback);

#endif /* EVENTS_H_ */
