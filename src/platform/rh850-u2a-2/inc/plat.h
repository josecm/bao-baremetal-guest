#ifndef PLAT_H
#define PLAT_H

#include <core.h>

#define PLAT_MEM_BASE 0x200000
#define PLAT_MEM_SIZE 0x8000000

#define PLAT_CLK_RLIN  (80000000) // 80 MHz
#define PLAT_UART_ADDR (0xFFD28400) // RLIN34 Base
#define UART_IRQ_ID    (434) // RLIN34 Receive completion interrupt

#define PLAT_TPTM_BASE  (0xFFFBB000UL) // TPTM Self base
#define PLAT_CLK_CPU    (400000000UL) // 400 MHz TODO: check this is right

#define PLAT_INTC1_BASE  (0xFFFC0000UL)
#define PLAT_INTC2_BASE  (0xFFF80000UL)
#define PLAT_INTIF_BASE  (0xFF090000UL)
#define PLAT_EINTS_BASE  (0xFFC00000UL)
#define PLAT_FENC_BASE   (0xFF9A3A00UL)
#define PLAT_FEINC_BASE  (0xFF9A3B00UL)
#define PLAT_IPIR_BASE   (0xFFFB9000UL)

#define PLAT_BOOTCTRL_ADDR  (0xFFFB2000UL)

#define PLAT_NUM_PORT_REGS  (21)
#define PLAT_NUM_PORT_GROUPS (11)
#define PLAT_PORT_BASE  (0xFFD90000UL)

extern uint16_t port_reg_val[PLAT_NUM_PORT_REGS][PLAT_NUM_PORT_GROUPS];
extern uint32_t port_reg_offset[PLAT_NUM_PORT_REGS];
extern uint32_t group_number[PLAT_NUM_PORT_GROUPS];

#endif
