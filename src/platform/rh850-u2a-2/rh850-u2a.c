#include <plat.h>
#include <renesas_rlin3.h>

static struct renesas_rlin3 *uart = (void*)PLAT_UART_ADDR;

void uart_init(){
    renesas_rlin3_init(uart);
}

void uart_putc(char c)
{
    renesas_rlin3_putc(uart, c);
}

char uart_getchar(void)
{
    return renesas_rlin3_getc(uart);
}

void uart_enable_rxirq()
{
    renesas_rlin3_enable_rxirq(uart);
}

void uart_clear_rxirq()
{
    renesas_rlin3_clear_rxirq(uart);
}

#define MCU_CKSC_DISABLE_REG_PROTECT_VALUE (0xA5A5A501UL)
#define MCU_CKSC_ENABLE_REG_PROTECT_VALUE (0xA5A5A500UL)
#define MCU_CLKKCPROT1 (0xFF980700UL)

#define MCU_MOSCE (0xFF988000UL)
#define MCU_MOSCE_ENABLE_TRIGGER  (0x00000001UL)
#define MCU_MOSCE_DISABLE_TRIGGER (0x00000002UL)

#define MCU_PLLE  (0xFF980000UL)
#define MCU_PLLE_ENABLE_TRIGGER  (0x00000001UL)
#define MCU_PLLE_DISABLE_TRIGGER (0x00000002UL)

#define MCU_CKSC_CPUC  (0xFF980100UL)

#define MCU_PLLSTPM  (0xFF98000CUL)
#define MCU_MOSCSTPM (0xFF98800CUL)
#define MCU_HSOSCSTPM (0xFF988104UL)

#define MCU_PKCPROT (0xFFD92F40UL)
#define MCU_PWE (0xFFD92F44UL)

#define PORT_PWE_ALL_MASK      (0x0BF724F8UL)
#define PORT_PWE_ENABLE_WRITE  (0xA5A5A501UL)
#define PORT_PWE_DISABLE_WRITE (0xA5A5A500UL)

void plat_init() {
    // // Start main oscillator
    // (*(volatile uint32_t*)MCU_CLKKCPROT1) = MCU_CKSC_DISABLE_REG_PROTECT_VALUE;
    // (*(volatile uint32_t*)MCU_MOSCE) = MCU_MOSCE_ENABLE_TRIGGER;
    // (*(volatile uint32_t*)MCU_CLKKCPROT1) = MCU_CKSC_ENABLE_REG_PROTECT_VALUE;

    // // Trigger start of PLL
    // (*(volatile uint32_t*)MCU_CLKKCPROT1) = MCU_CKSC_DISABLE_REG_PROTECT_VALUE;
    // (*(volatile uint32_t*)MCU_PLLE) = MCU_PLLE_ENABLE_TRIGGER;
    // (*(volatile uint32_t*)MCU_CLKKCPROT1) = MCU_CKSC_ENABLE_REG_PROTECT_VALUE;

    // Select CLK_PLLO clock source
    (*(volatile uint32_t*)MCU_CLKKCPROT1) = MCU_CKSC_DISABLE_REG_PROTECT_VALUE;
    (*(volatile uint32_t*)MCU_CKSC_CPUC) = 0;
    (*(volatile uint32_t*)MCU_CLKKCPROT1) = MCU_CKSC_ENABLE_REG_PROTECT_VALUE;

    // Set CLKs to continue in standby mode
    (*(volatile uint32_t*)MCU_CLKKCPROT1) = MCU_CKSC_DISABLE_REG_PROTECT_VALUE;
    (*(volatile uint32_t*)MCU_PLLSTPM) = 1UL;
    (*(volatile uint32_t*)MCU_MOSCSTPM) = 1UL;
    (*(volatile uint32_t*)MCU_HSOSCSTPM) = 1UL;
    (*(volatile uint32_t*)MCU_CLKKCPROT1) = MCU_CKSC_ENABLE_REG_PROTECT_VALUE;

    // Enable Write Port
    (*(volatile uint32_t*)MCU_PKCPROT) = PORT_PWE_ENABLE_WRITE;
    (*(volatile uint32_t*)MCU_PWE) = PORT_PWE_ALL_MASK;
    (*(volatile uint32_t*)MCU_PKCPROT) = PORT_PWE_DISABLE_WRITE;

    for (int i = 0; i < PLAT_NUM_PORT_REGS; i++) {
        for (int j = 0; j < PLAT_NUM_PORT_GROUPS; j++) {
            uint16_t *port_reg_addr = (
                    void*)(PLAT_PORT_BASE + (0x40 * group_number[j]) + port_reg_offset[i]);
            *port_reg_addr = port_reg_val[i][j];
        }
    }


    // Disable Write Port
    (*(volatile uint32_t*)MCU_PKCPROT) = PORT_PWE_ENABLE_WRITE;
    (*(volatile uint32_t*)MCU_PWE) = 0;
    (*(volatile uint32_t*)MCU_PKCPROT) = PORT_PWE_DISABLE_WRITE;
}
