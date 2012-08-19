#ifndef _UART_SC61IS740_H
#define _UART_SC61IS740_H

#include <stdint.h>

void uart_sc16is740_init();
void uart_sc16is740_putchar(uint8_t ch);
uint8_t uart_sc16is740_getchar(void);

#endif
