
#include "cc32_irq.h"

#define CC32_SYSC_BASE	0x0F0000

enum cc32_sysc_reg {
	SCCM0		= 0x00,
	SCSYS		= 0x04,
	SCCKOUT		= 0x20,
	SCRSTFLG	= 0x28,
	SCRSTEN		= 0x2C,
	SCSFTRST	= 0x30,
	SCRSTCON0	= 0x34,
	SCRSTCON4	= 0x38,
	SCSLEEP		= 0x3C,
	SCGCON		= 0x40,
	SCINTSTS	= 0x44,
	SCINTEN		= 0x48,
	SCGINT0		= 0x5C,
	SCGLEV		= 0x64,
	SCWUT		= 0x68,
	SCCM4		= 0x7C,
};

#define SYSC_REG(x)	(uint32_t *)((uint8_t *)CC32_SYSC_BASE + x)

void cc32_irq_enable(enum cc32_irq irq)
{
	*SYSC_REG(SCINTEN) |= (1 << irq);
}

void cc32_irq_disable(enum cc32_irq irq)
{
	*SYSC_REG(SCINTEN) &= ~(1 << irq);
}

void cc32_irq_register(enum cc32_irq irq, void (*handler)(enum cc32_irq irq))
{
	/* FIXME */
}
