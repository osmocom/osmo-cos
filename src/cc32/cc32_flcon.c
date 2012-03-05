/*
 * ChipCity CC32RS512 Flash controller driver
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

#include <stdint.h>
#include <errno.h>

#include "cc32_irq.h"
#include "cc32_flcon.h"

#define CC32_FLCON_BASE	0x0f2000

enum cc32_flcon_reg {
	FLCON		= 0x00,
	FLSDP1		= 0x04,
	FLSDP2		= 0x08,
	FLSTS		= 0x0C,
	FLBUF		= 0x10,
};

#define FLCON_REG(x)	(uint32_t *)((uint8_t *)CC32_FLCON_BASE + x)


int cc32_flash_erase(uint32_t offset, uint16_t page_size)
{
	switch (page_size) {
	case 256:
		*FLCON_REG(FLCON) = (1 << 3);
		break;
	case 512:
		*FLCON_REG(FLCON) = (0 << 3);
		break;
	default:
		return -EINVAL;
	}

	/* if we program/erase the first page, we have to disable the interrupt */
	if (offset < page_size)
		cc32_irq_disable(IRQ_FLCON);

	/* clear all flags */
	*FLCON_REG(FLSTS) = 3;

	*FLCON_REG(FLSDP1) = 0x55;
	*FLCON_REG(FLSDP2) = 0xAA;

	*(uint32_t *)offset = 0xFF;

	while (!(*FLCON_REG(FLSTS) & 1)) {}

	return 0;
}

int cc32_flash_program(uint32_t offset, uint16_t page_size, uint32_t *buffer)
{
	switch (page_size) {
	case 256:
		*FLCON_REG(FLCON) = (1 << 3);
		break;
	case 512:
		*FLCON_REG(FLCON) = (0 << 3);
		break;
	default:
		return -EINVAL;
	}

	/* if we program/erase the first page, we have to disable the interrupt */
	if (offset < page_size)
		cc32_irq_disable(IRQ_FLCON);

	/* clear all flags */
	*FLCON_REG(FLSTS) = 3;

	*FLCON_REG(FLBUF) = offset;
	*FLCON_REG(FLSDP1) = 0xAA;
	*FLCON_REG(FLSDP2) = 0x55;

	*(uint32_t *)offset = 0x00;

	while (!(*FLCON_REG(FLSTS) & 1)) {}

	/* clear flag */
	*FLCON_REG(FLSTS) |= 1;

	return 0;
}
