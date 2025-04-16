#ifndef ARCH_TIMER_H
#define ARCH_TIMER_H

#include <core.h>
#include <plat.h>
#include <intc.h>

// We will set TPTMSUDIV to match this frequency (1 MHz, each tick 1us)
#define TIMER_FREQ (1000000UL)

static struct {
    volatile uint32_t TPTMSIRUN;
    volatile uint32_t TPTMSIRRUN;
    volatile uint32_t TPTMSISTP;
    volatile uint32_t TPTMSISTR;
    volatile uint32_t TPTMSIIEN;
    volatile uint32_t TPTMSIUSTR;
    volatile uint32_t TPTMSIDIV;
    uint8_t res0[4];
    volatile uint32_t TPTMSFRUN;
    volatile uint32_t TPTMSFRRUN;
    volatile uint32_t TPTMSFSTP;
    volatile uint32_t TPTMSFSTR;
    volatile uint32_t TPTMSFDIV;
    uint8_t res1[12];
    volatile uint32_t TPTMSURUN;
    volatile uint32_t TPTMSURRUN;
    volatile uint32_t TPTMSUSTP;
    volatile uint32_t TPTMSUSTR;
    volatile uint32_t TPTMSUIEN;
    uint8_t res2[4];
    volatile uint32_t TPTMSUDIV;
    volatile uint32_t TPTMSUTRG;
    uint8_t res3[32];
    volatile uint32_t TPTMSICNT0;
    volatile uint32_t TPTMSILD0;
    volatile uint32_t TPTMSICNT1;
    volatile uint32_t TPTMSILD1;
    uint8_t res43[16];
    volatile uint32_t TPTMSFCNT;
    uint8_t res5[28];
    volatile uint32_t TPTMSUCNT0;
    volatile uint32_t TPTMSUCMP00;
    volatile uint32_t TPTMSUCMP01;
    volatile uint32_t TPTMSUCMP02;
    volatile uint32_t TPTMSUCMP03;
    uint8_t res6[12];
    volatile uint32_t TPTMSUCNT1;
    volatile uint32_t TPTMSUCMP10;
    volatile uint32_t TPTMSUCMP11;
    volatile uint32_t TPTMSUCMP12;
    volatile uint32_t TPTMSUCMP13;
} *tptm = (void*) PLAT_TPTM_BASE;

static inline void timer_enable()
{
    intc_set_tptmsel(TPTM_SEL_EIINT); // configure interrupt as ei
    tptm->TPTMSUIEN = (1ULL << 0); // enable interrupt to channel 0, compare value 0
    tptm->TPTMSUDIV = (PLAT_CLK_CPU / TIMER_FREQ) - 1;
    tptm->TPTMSUCMP00 = ~0; // set compare value 0 to maximum in channel 0
    tptm->TPTMSURUN = 1; // load counter with 0 in channel 0
}

static inline uint64_t timer_get()
{
    return tptm->TPTMSUCNT0;
}

static inline void timer_set(uint64_t n)
{
    tptm->TPTMSUCMP00 = timer_get() + n;
}


#endif /* ARCH_TIMER_H */
