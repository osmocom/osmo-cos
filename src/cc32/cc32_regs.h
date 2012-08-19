#ifndef _CC32_REGS_H
#define _CC32_REGS_H

#define CC32_SYSC_BASE	0x0F0000

enum cc32_sysc_reg {
	SCCM0		= 0x00,
	SCSYS		= 0x04,
	SCCKOUT		= 0x20,
	SCRSTFLG	= 0x28,
	SCRSTEN		= 0x2C,
	SCSFTRST	= 0x30,
	SCRSTCON0	= 0x34,
	SCRSTCON4	= 0x38,
	SCSLEEP		= 0x3C,
	SCGCON		= 0x40,
	SCINTSTS	= 0x44,
	SCINTEN		= 0x48,
	SCGINT0		= 0x5C,
	SCGLEV		= 0x64,
	SCWUT		= 0x68,
	SCCM4		= 0x7C,
};

#define SYSC_REG(x)	(uint32_t *)((uint8_t *)CC32_SYSC_BASE + x)


#define CC32_GPIO_BASE	0x0f8c00

enum cc32_gpio_reg {
	GPIODAT0	= 0x00,
	GPIODIR0	= 0x04,
	GPIODAT1	= 0x08,
	GPIODIR1	= 0x0C,
};

#define GPIO_REG(x)	(uint32_t *)((uint8_t *)CC32_GPIO_BASE + x)


#endif /* _CC32_REGS_H */
