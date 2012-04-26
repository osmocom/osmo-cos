#ifndef _CC32_SYSC_H
#define _CC32_SYSC_H

#include <stdint.h>

enum cc32_clk {
	CLK_TIMER	= (1 << 0),
	CLK_CRC		= (1 << 2),
	CLK_DES		= (1 << 3),
	CLK_AES		= (1 << 4),
	CLK_RSA		= (1 << 5),
	CLK_FLASH	= (1 << 8),
	CLK_RAM0	= (1 << 11),
	CLK_RAM12	= (1 << 12),
	CLK_ISO7816	= (1 << 16),
	CLK_GPIO	= (1 << 18),
	CLK_SPI		= (1 << 20),
	CLK_VD		= (1 << 24),
	CLK_FD		= (1 << 25),
	CLK_RNG		= (1 << 27),
	CLK_DMA		= (1 << 28),
};

void cc32_sysc_clk_enable(enum cc32_clk clk);
void cc32_sysc_clk_disable(enum cc32_clk clk);

#endif
