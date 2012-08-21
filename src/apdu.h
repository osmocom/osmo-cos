#include <stdint.h>

#ifndef _APDU_H
#define _APDU_H

#define RC_SW		0
#define RC_PROC_RX	1
#define RC_PROC_TX	2

#define RV(class, value)	(((class) << 24) | (value) & 0xff)
#define RV_SW(x)		RV(RC_SW, x)
#define RV_PROC_RX(x)		RV(RC_PROC_RX, x)
#define RV_PROC_TX(x)		RV(RC_PROC_TX, x)

/*! \brief In which mode do we call the APDU call-back? */
enum apdu_cb_mode {
	AP_MODE_T0_HDR,
};

/* dynamic in-memory structure listing classes currently available/visible for
 * the given APDU channel */
struct apdu_parser {
	unsigned int num_classes;
	const struct apdu_class *classes;
};

struct apdu_hdr {
	uint8_t cla;
	uint8_t ins;
	uint8_t p1;
	uint8_t p2;
	uint8_t p3;
} __attribute((packed))__;

struct apdu_cmd {
	uint16_t _pad;
	uint8_t flags;
	uint8_t ins;
	int (*cb)(struct apdu_parser *ap, int mode, const struct apdu_hdr *hdr);
} __attribute__ ((packed));

struct apdu_class {
	uint8_t mask;
	uint8_t compare;
	uint8_t num_cmds;
	const struct apdu_cmd *cmds;
};

#endif
