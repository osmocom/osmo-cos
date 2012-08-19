#ifndef _CC32_GPIO_H
#define _CC32_GPIO_H

enum cc32_gpio_port {
	CC32_GPIO_P0,
	CC32_GPIO_P1,
	CC32_GPIO_P2,
	CC32_GPIO_P3,
	CC32_GPIO_P4,
	CC32_GPIO_P5,
};

enum cc32_gpio_alt {
	CC32_GPIO_FUNC1,
	CC32_GPIO_FUNC2,
	CC32_GPIO_FUNC3,
};

void cc32_gpio_output(uint8_t gpio, int output);
void cc32_gpio_set(uint8_t gpio, int val);
uint8_t cc32_gpio_get(uint8_t gpio);
void cc32_gpio_init(void);
void cc32_gpio_alt(enum cc32_gpio_port port,
		   enum cc32_gpio_alt alt);

#endif
