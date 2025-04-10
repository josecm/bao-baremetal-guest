#include <intc.h>
#include <cpu.h>
#include <plat.h>

#define IPIR_CHANNEL_NUM 4

struct ipir {
    struct {
        volatile uint8_t IPI_ENS;   // Offset: 0x000 + 0x020 * n
        volatile uint8_t IPI_FLGS;  // Offset: 0x004 + 0x020 * n
        volatile uint8_t IPI_FCLRS; // Offset: 0x008 + 0x020 * n
        volatile uint8_t IPI_REQS;  // Offset: 0x010 + 0x020 * n
        volatile uint8_t IPI_RCLRS; // Offset: 0x014 + 0x020 * n
    } channel[IPIR_CHANNEL_NUM];
};

static struct ipir* ipir_hw  = (void*) PLAT_IPIR_BASE;

void irq_enable(unsigned id) {
    intc_set_enable(id, true);
    intc_set_trgt(id, get_cpuid());
}

void irq_set_prio(unsigned id, unsigned prio) {
    intc_set_prio(id, prio);
}

void irq_send_ipi(unsigned long target_cpu_mask) {
    // ipir_hw->channel[0].IPI_REQS = target_cpu_mask;
}
