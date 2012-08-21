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

#include <string.h>

#include <osmocom/core/utils.h>
#include <osmocom/core/logging.h>

#include "apdu.h"
#include "logging.h"

enum tpdu_rx_state {
	T_STATE_POST_ATR,
	T_STATE_WAIT_PPS,
	T_STATE_WAIT_PCK,
	T_STATE_WAIT_INS,
	T_STATE_WAIT_P1,
	T_STATE_WAIT_P2,
	T_STATE_WAIT_P3,
	T_STATE_POST_HDR,
	T_STATE_WAIT_DATA,
	T_STATE_WAIT_CLA,
};

static const struct value_string tpdu_rx_state_names[] = {
	{ T_STATE_POST_ATR, 	"POST_ATR" },
	{ T_STATE_WAIT_PPS,	"WAIT_PPS" },
	{ T_STATE_WAIT_PCK,	"WAIT_PCK" },
	{ T_STATE_WAIT_INS,	"WAIT_INS" },
	{ T_STATE_WAIT_P1,	"WAIT_P1" },
	{ T_STATE_WAIT_P2,	"WAIT_P2" },
	{ T_STATE_WAIT_P3,	"WAIT_P3" },
	{ T_STATE_POST_HDR,	"POST_HDR" },
	{ T_STATE_WAIT_CLA,	"WAIT_CLA" },
	{ 0, NULL }
};

enum tpdu_tx_state {
	TX_S_IDLE,
	TX_S_WAIT_PROC,
	TX_S_WAIT_SW1,
	TX_S_WAIT_SW2,
	TX_S_WAIT_DATA,
};

static const struct value_string tpdu_tx_state_names[] = {
	{ TX_S_IDLE,		"IDLE" },
	{ TX_S_WAIT_PROC,	"WAIT_PROC" },
	{ TX_S_WAIT_SW1,	"WAIT_SW1" },
	{ TX_S_WAIT_SW2,	"WAIT_SW2" },
	{ TX_S_WAIT_DATA,	"WAIT_DATA" },
	{ 0, NULL }
};


struct tpdu_state {
	union {
		struct apdu_hdr s;
		uint8_t a[5];
	} hdr;
	struct {
		uint8_t pps[4];
		uint8_t pck;
		uint8_t idx;
		uint8_t xor_sum;
	} pps;
	uint16_t sw;
	uint8_t proc;
	enum tpdu_rx_state state;
	enum tpdu_tx_state tx_state;

	uint8_t tx_data_pending;
	uint8_t rx_data_pending;
	uint8_t buf[256];

	struct apdu_parser *ap;
};

static struct tpdu_state _ts;

static void set_state(struct tpdu_state *s, enum tpdu_rx_state new)
{
	DEBUGP(DPDU, "RX State change %s -> %s\n",
		get_value_string(tpdu_rx_state_names, s->state),
		get_value_string(tpdu_rx_state_names, new));

	s->state = new;

	if (s->state == T_STATE_POST_ATR) {
		memset(&s->hdr, sizeof(s->hdr), 0);
		memset(&s->pps, sizeof(s->hdr), 0);
	}
}

static void set_tx_state(struct tpdu_state *s, enum tpdu_tx_state new)
{
	DEBUGP(DPDU, "TX State change %s -> %s\n",
		get_value_string(tpdu_tx_state_names, s->tx_state),
		get_value_string(tpdu_tx_state_names, new));

	s->tx_state = new;

	if (s->tx_state == TX_S_IDLE) {
		s->tx_data_pending = 0;
		memset(&s->hdr, sizeof(s->hdr), 0);
		memset(&s->pps, sizeof(s->hdr), 0);
	}
}


static void tpdu_rx_byte(struct tpdu_state *ts, uint8_t byte)
{
	int rc;

	switch (ts->state) {
	case T_STATE_POST_ATR:
		if (byte == 0xFF)
			set_state(ts, T_STATE_WAIT_PPS);
		else {
			ts->hdr.s.cla = byte;
			set_state(ts, T_STATE_WAIT_INS);
		}
		break;
	case T_STATE_WAIT_INS:
		ts->hdr.s.ins = byte;
		set_state(ts, T_STATE_WAIT_P1);
		break;
	case T_STATE_WAIT_P1:
		ts->hdr.s.p1 = byte;
		set_state(ts, T_STATE_WAIT_P2);
		break;
	case T_STATE_WAIT_P2:
		ts->hdr.s.p2 = byte;
		set_state(ts, T_STATE_WAIT_P3);
		break;
	case T_STATE_WAIT_P3:
		ts->hdr.s.p3 = byte;
		set_state(ts, T_STATE_POST_HDR);
		/* FIXME: set-up automatic WTX timer */
		/* FIXME: call APDU dispatcher */
		rc = apdu_handle_hdr(ts->ap, &ts->hdr.s);
		switch (rc >> 24) {
		case RC_SW:
			ts->sw = rc & 0xffff;
			set_tx_state(ts, TX_S_WAIT_SW1);
			set_state(ts, T_STATE_WAIT_CLA);
			break;
		case RC_PROC_RX:
			/* procedure byte */
			ts->proc = rc & 0xff;
			set_tx_state(ts, TX_S_WAIT_PROC);
			//ts->rx_data_len = (rc >> 8) & 0xff;
			set_state(ts, T_STATE_WAIT_DATA);
			break;
		case RC_PROC_TX:
			/* procedure byte */
			set_tx_state(ts, TX_S_WAIT_PROC);
			//ts->rx_data_len = (rc >> 8) & 0xff;
			set_state(ts, T_STATE_WAIT_CLA);
			break;
		}
		break;
	case T_STATE_WAIT_PPS:
		ts->pps.pps[ts->pps.idx++] = byte;
		ts->pps.xor_sum ^= byte;
		switch (ts->pps.idx) {
		case 1:
			if (!(ts->pps.pps[0] & 0x10))
				set_state(ts, T_STATE_WAIT_PCK);
			break;
		case 2:
			if (!(ts->pps.pps[0] & 0x20))
				set_state(ts, T_STATE_WAIT_PCK);
			break;
		case 3:
			if (!(ts->pps.pps[0] & 0x40))
				set_state(ts, T_STATE_WAIT_PCK);
			/* otherwise: stay in T_STATE_WAIT_PPS */
			break;
		default:
			set_state(ts, T_STATE_WAIT_PCK);
		}
		break;
	case T_STATE_WAIT_PCK:
		if (byte != ts->pps.xor_sum) {
			/* checksum mismatch ! */
			while (1) {}
		}
		/* FIXME: check if protocol is supported */
		//proto = ts->pps.pps[0] & 0x0f;
		break;
	}

}

/* pull one byte out of the TPDU transmit state machine, negative if thre is
 * none */
static int tpdu_tx_pull(struct tpdu_state *ts)
{
	int rc = -2;

	switch (ts->tx_state) {
	case TX_S_IDLE:
		rc = -1;
		break;
	case TX_S_WAIT_PROC:
		rc = ts->proc;
		if (ts->tx_data_pending)
			set_tx_state(ts, TX_S_WAIT_DATA);
		else
			set_tx_state(ts, TX_S_IDLE);
		break;
	case TX_S_WAIT_SW1:
		rc = ts->sw >> 8;
		set_tx_state(ts, TX_S_WAIT_SW2);
		break;
	case TX_S_WAIT_SW2:
		rc = ts->sw & 0xff;
		set_tx_state(ts, TX_S_IDLE);
		break;
	case TX_S_WAIT_DATA:
		rc = ts->buf[ts->tx_data_pending--];
		if (!ts->tx_data_pending)
			set_tx_state(ts, TX_S_IDLE);
		break;
	}
	return rc;
}
