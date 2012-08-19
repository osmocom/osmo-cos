
#include "cc32_regs.h"

#include "cc32_sysc.h"
#include "cc32_irq.h"

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

void cc32_sysc_clk_enable(enum cc32_clk clk)
{
	*SYSC_REG(SCCM0) |= clk;
}

void cc32_sysc_clk_disable(enum cc32_clk clk)
{
	*SYSC_REG(SCCM0) &= ~clk;
}
