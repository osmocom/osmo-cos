#ifndef _ISO7816_SLAVE_H
#define _ISO7816_SLAVE_H

int iso7816_slave_init(void);
int iso7816_slave_tx(const uint8_t *data, uint8_t len);
int iso7816_slave_tx_ch(uint8_t ch);
int iso7816_slave_tx5(const uint8_t *data);

#endif
