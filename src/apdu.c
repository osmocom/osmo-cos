/*
 * ISO 7816 APDU handler routines (card side)
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

#include "apdu.h"


static int apdu_handle_cls(const struct apdu_class *ac, struct apdu_parser *ap,
			   int mode, const struct apdu_hdr *hdr)
{
	int i;

	for (i = 0; i < ac->num_cmds; i++) {
		struct apdu_cmd *cmd = &ac->cmds[i];
		if (cmd->ins == hdr->ins)
			return cmd->cb(ap, mode, hdr);
	}

	return RV_SW(0x6D00);
}


int apdu_handle_hdr(struct apdu_parser *ap, struct apdu_hdr *hdr)
{
	int i;

	/* iterate over all registered classes and check for a CLA match */
	for (i = 0; i < ap->num_classes; i++) {
		const struct apdu_class *ac = &ap->classes[i];
		/* if CLA matches, iterate over INS array and call callback */
		if (hdr->cla & ac->mask == ac->compare)
			return apdu_handle_cls(ac, ap, AP_MODE_T0_HDR, hdr);
	}

	return RV_SW(0x6E00);
}
