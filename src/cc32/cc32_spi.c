/*
 * ChipCity CC32RS512 SPI controller driver
 *
 * Copyright (C) 2012 Harald Welte <laforge@gnumonks.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 or
 * (at your option) version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>

#include "cc32_sysc.h"
#include "cc32_spi.h"
#include "cc32_gpio.h"
#include "board.h"

#define CC32_SPI_BASE	0x0f9800

enum cc32_spi_reg {
	SPICON2		= 0x08,
	SPIDAT		= 0x0C,
	SPISTS		= 0x10,
	SPIMSK		= 0x14,
	SPIDMALEN	= 0x20,
	SPIDMACON	= 0x28,
	SPIDMASTS	= 0x2C,
	SPIDMAMSK	= 0x30,
	SPITBUF		= 0x40,
	SPIRBUF		= 0x44,
	SPIDMARPT	= 0x48,
};

#define SPI_REG(x)	(volatile uint32_t *)((uint8_t *)CC32_SPI_BASE + x)

#define SPICON2_RUNEN	(1 << 7)

#define SPISTS_BOVER	(1 << 0)
#define SPISTS_MODF	(1 << 1)

#define SPIMSK_MBOVER	(1 << 0)

#define SPIDMACON_FULL_DUPLEX	(0 << 1)
#define SPIDMACON_TX_ONLY	(1 << 1)
#define SPIDMACON_RX_ONLY	(2 << 1)
#define SPIDMACON_START		(1 << 0)

#define SPIDMASTS_OVER		(1 << 0)
#define SPIDMASTS_BOVER		(1 << 1)

#define SPIDMAMSK_MOVER		(1 << 0)
#define SPIDMAMSK_MBOVER	(1 << 1)

int cc32_spi_init(uint8_t cpol, uint8_t cpha, uint8_t divide_2n)
{
	uint32_t val;

	cc32_sysc_clk_enable(CLK_SPI);

	/* configure GPIOs as output */
	cc32_gpio_output(24, 1);
	cc32_gpio_output(25, 1);
	cc32_gpio_output(26, 1);
	/* configure P3 as SPI function */
	cc32_gpio_alt(CC32_GPIO_P3, CC32_GPIO_FUNC2);

	/* configure GPIO11 as output (SPI_nCS) */
	cc32_gpio_output(BOARD_GPIO_SPI_NCS, 1);
	/* set to 1 (disable) */
	cc32_gpio_set(BOARD_GPIO_SPI_NCS, 1);

	if (divide_2n < 2 || divide_2n > 256)
		return -EINVAL;

	val = ((cpol & 1) << 0) | ((cpha & 1) << 1) | ((divide_2n-1) << 2) | SPICON2_RUNEN;

	/* this is an undocumented value, but without it seems the SPI
	 * is not working! */
	*SPI_REG(SPICON2) = val | 0x60;

	return 0;
}

int cc32_spi_xcv_byte(uint8_t tx)
{
	*SPI_REG(SPIDAT) = tx;
	while (!(*SPI_REG(SPISTS) & SPISTS_BOVER)) { }
	return *SPI_REG(SPIDAT);
}

void cc32_spi_ncs(uint8_t high)
{
	cc32_gpio_set(BOARD_GPIO_SPI_NCS, high);
}
