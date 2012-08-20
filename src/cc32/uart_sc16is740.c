/*
 * NXP SC16IS740 SPI UART driver
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
#include "cc32_spi.h"

#define RHR	0x00
#define THR	0x00
#define	IER	0x01
#define FCR	0x02
#define IIR	0x02
#define LCR	0x03
#define MCR	0x04
#define LSR	0x05
#define MSR	0x06
#define SPR	0x07

#define DLL	RHR
#define DLH	IER

#define EFR	FCR

static void reg_write(uint8_t reg, uint8_t val)
{
	uint8_t cmd = (reg << 3);

	cc32_spi_ncs(0);
	cc32_spi_xcv_byte(cmd);
	cc32_spi_xcv_byte(val);
	cc32_spi_ncs(1);
}

static int reg_read(uint8_t reg)
{
	uint8_t tx = (reg << 3) | 0x80;
	uint8_t ret;

	cc32_spi_ncs(0);
	cc32_spi_xcv_byte(tx);
	ret = cc32_spi_xcv_byte(0);
	cc32_spi_ncs(1);

	return ret;
}

static void fifo_write(uint8_t *buf, uint8_t len)
{
	uint8_t cmd = (0 << 3);

	cc32_spi_ncs(0);
	cc32_spi_xcv_byte(cmd);
	while (len--)
		cc32_spi_xcv_byte(*buf++);
	cc32_spi_ncs(1);
}

static void fifo_read(uint8_t *buf, uint8_t len)
{
	uint8_t cmd = (0 << 3) | 0x80;

	cc32_spi_ncs(0);
	cc32_spi_xcv_byte(cmd);
	while (len--)
		*(buf++) = cc32_spi_xcv_byte(0);
	cc32_spi_ncs(1);
}


void uart_sc16is740_init()
{
	/* set 8N1 */
	reg_write(LCR, (3 << 0) | (0 << 2));
	/* set LCR[7] = 1 (divisor latch) */
	reg_write(LCR, reg_read(LCR) | 0x80);
	/* baud rate generator for 115200 */
	reg_write(DLL, 1);
	reg_write(DLH, 0);
	/* clear LCR[7] = 0 (divisor latch) */
	reg_write(LCR, reg_read(LCR) & ~0x80);
}

void uart_sc16is740_putchar(uint8_t ch)
{
	/* wait until THR empty */
	while (!(reg_read(LSR) & (1 << 5))) { }

	reg_write(THR, ch);
}

uint8_t uart_sc16is740_getchar(void)
{
	do {
	} while (!(reg_read(LSR) & 1));

	return reg_read(THR);
}
