#ifndef _CC32_IRQ_H
#define _CC32_IRQ_H

#include <stdint.h>

enum cc32_irq {
	IRQ_TIMER0	= 2,
	IRQ_TIMER1	= 3,
	IRQ_TIMER2	= 4,
	IRQ_FLCON	= 6,
	IRQ_UART	= 8,
	IRQ_GPIO	= 11,
	IRQ_SPI		= 13,
	IRQ_SCU		= 15,
	IRQ_MCU		= 17,
	IRQ_DES		= 19,
	IRQ_AES		= 20,
	IRQ_DMA		= 22,
	IRQ_ISO7816_MST	= 26,
	IRQ_WUT		= 27,
	IRQ_RSA		= 28,
};

void cc32_irq_enable(enum cc32_irq irq);
void cc32_irq_disable(enum cc32_irq irq);
void cc32_irq_register(enum cc32_irq irq, void (*handler)(enum cc32_irq irq));

#endif
