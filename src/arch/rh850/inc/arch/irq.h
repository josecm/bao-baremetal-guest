#ifndef ARCH_IRQ_H
#define ARCH_IRQ_H

#include <intc.h>

#define IPI_IRQ_ID (0UL)
#define TIMER_IRQ_ID (209UL) // EEINT TPTM up timer interrupt for PE0 with comparison value 0

#define IRQ_NUM (ARCH_MAX_INTERRUPTS)
#define IRQ_MAX_PRIO (0x1)

#endif /* ARCH_IRQ_H */
