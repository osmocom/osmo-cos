
#include <stdint.h>
#include <errno.h>

#include "cc32_irq.h"

#ifndef readl
#define readl(addr)	*((unsigned long *)addr)
#endif

#ifndef writel
#define writel(what, addr) *((unsigned long *)addr) = (what)
#endif

enum iso_slave_reg {
	ISOCON		= 0x00,
	ISOCON1		= 0x04,
	ISOCON2		= 0x08,
	ISOSTS		= 0x0c,
	ISOBRC		= 0x10,
	ISOBUF		= 0x14,
	ISODIO		= 0x18,
	ISOMSK		= 0x1c,
	ISODMACON	= 0x30,
	ISODMASTS	= 0x34,
	ISODMABFAD	= 0x38,
	ISODMABFLEN	= 0x3c,
	ISODMABFPT	= 0x40,
	ISODMAMSK	= 0x44,
	ISOTCON		= 0x50,
	ISOTDAT		= 0x54,
	ISOTRLD		= 0x58,
	ISOTMSK		= 0x5c,
	ISONULL		= 0x60,
};

#define CC32_BASE_UART	0x0F8800
#define UART_REG(n)	(CC32_BASE_UART+(n))

#define ISOCON_TR	(1 << 5)
#define ISOCON_TACT	(1 << 4)

#define ISOSTS_TBE	(1 << 0)
#define ISOSTS_RBF	(1 << 1)
#define ISOSTS_PE	(1 << 2)
#define ISOSTS_OE	(1 << 3)

#define ISOCON2_SBIT	(1 << 7)

#define ISOBRC_DI(x)	((x) & 0xf)
#define ISOBRC_FI(x)	(((x) & 0xf) << 4)

struct iso_slave_state {
	uint8_t fi;
	uint8_t di;
};
static struct iso_slave_state iss;

/* will be called from FIQ mode */
static void slave_irq(uint8_t irq)
{
	//FIXME
}

int iso7816_slave_fi_di(uint8_t fi, uint8_t di)
{
	writel(ISOBRC_DI(di) | ISOBRC_FI(fi), UART_REG(ISOBRC));
	iss.fi = fi;
	iss.di = di;

	return 0;
}

int iso7816_slave_tx_ch(uint8_t ch)
{
	writel(ISOCON_TR, UART_REG(ISOCON));
	writel(ch, UART_REG(ISOBUF));

	if (readl(UART_REG(ISOSTS)) & ISOSTS_PE) {
		writel(ISOSTS_PE, UART_REG(ISOSTS));
		return -EIO;
	}

	while (readl(UART_REG(ISOCON)) & ISOCON_TACT) { }

	return 0;
}

int iso7816_slave_tx5(const uint8_t *data)
{
	iso7816_slave_tx_ch(data[0]);
	iso7816_slave_tx_ch(data[1]);
	iso7816_slave_tx_ch(data[2]);
	iso7816_slave_tx_ch(data[3]);
	iso7816_slave_tx_ch(data[4]);

	return 0;
}

int iso7816_slave_tx(const uint8_t *data, uint8_t len)
{
	int i;
	/* FIXME: fiq/dma ? */

	writel(ISOCON_TR, UART_REG(ISOCON));

	for (i = 0; i < len; i++) {
		if (iso7816_slave_tx_ch(data[i]) < 0)
			return -EIO;
	}

	return i;
}

int iso7816_slave_init(void)
{
	/* receive mode */
	writel(0, UART_REG(ISOCON));
	/* up to 6 times re-transmission */
	writel(6 | ISOCON2_SBIT, UART_REG(ISOCON1));
	/* T=0 mode */
	writel(0, UART_REG(ISOCON2));
	/* clear status bits, if any */
	writel(ISOSTS_PE|ISOSTS_OE, UART_REG(ISOSTS));
	/* disable SBIT interrupt */
	writel(ISOCON2_SBIT, UART_REG(ISOMSK));

	/* Initial Fi/Di setting */
	iso7816_slave_fi_di(1, 1);

	cc32_irq_register(IRQ_UART, &slave_irq);
	//cc32_irq_enable(IRQ_UART);

	return 0;
}
