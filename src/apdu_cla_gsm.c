/*
 * ISO 7816 command handler for GSM SIM commands
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

#include <osmocom/core/utils.h>

#include "apdu.h"

static int cmd_select_file(struct apdu_parser *ap, int mode,
			   const struct apdu_hdr *hdr)
{
	return RV_SW(0x9000);
}

static const struct apdu_cmd gsm_cmds[] = {
	{ .ins = 0xA4, .cb = cmd_select_file },
};

const struct apdu_class gsm_class = {
	.mask = 0xff,
	.compare = 0xa0,
	.cmds = gsm_cmds,
	.num_cmds = ARRAY_SIZE(gsm_cmds),
};
