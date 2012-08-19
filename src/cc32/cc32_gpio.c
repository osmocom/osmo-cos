/*
 * ChipCity CC32RS512 GPIO controller driver
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

#include "cc32_regs.h"

#include "cc32_sysc.h"
#include "cc32_gpio.h"

void cc32_gpio_output(uint8_t gpio, int output)
{
	uint32_t reg = GPIODIR0;

	if (gpio >= 32) {
		reg = GPIODIR1;
		gpio -= 32;
	}

	if (output)
		*GPIO_REG(reg) &= ~(1 << gpio);
	else
		*GPIO_REG(reg) |= (1 << gpio);
}

void cc32_gpio_set(uint8_t gpio, int val)
{
	uint32_t reg = GPIODAT0;

	if (gpio >= 32) {
		reg = GPIODAT1;
		gpio -= 32;
	}

	if (val)
		*GPIO_REG(reg) |= (1 << gpio);
	else
		*GPIO_REG(reg) &= ~(1 << gpio);
}

uint8_t cc32_gpio_get(uint8_t gpio)
{
	uint32_t reg = GPIODAT0;

	if (gpio >= 32) {
		reg = GPIODAT1;
		gpio -= 32;
	}

	if (*GPIO_REG(reg) & (1 << gpio))
		return 1;
	else
		return 0;
}

void cc32_gpio_alt(enum cc32_gpio_port port,
		  enum cc32_gpio_alt alt)
{
	uint32_t tmp;

	tmp = *SYSC_REG(SCGCON);
	tmp &= ~(3 << (2*port));
	tmp |= ((alt & 3) << (2*port));
	*SYSC_REG(SCGCON) = tmp;
}


void cc32_gpio_init(void)
{
	cc32_sysc_clk_enable(CLK_GPIO);
}
