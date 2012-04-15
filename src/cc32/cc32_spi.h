#ifndef _CC32_SPI_H
#define _CC32_SPI_H

#include <stdint.h>

int cc32_spi_init(uint8_t cpol, uint8_t cpha, uint8_t divide_2n);
int cc32_spi_xcv_byte(uint8_t tx);

#endif
