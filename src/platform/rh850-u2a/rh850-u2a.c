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
