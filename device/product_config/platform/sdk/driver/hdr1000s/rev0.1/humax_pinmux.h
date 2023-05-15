#ifndef __HUMAX_PINMUX_H__
#define __HUMAX_PINMUX_H__

#if 0
#define HUMAX_NUM_GPIO_PINS NEXUS_NUM_GPIO_PINS
#else
#define HUMAX_NUM_GPIO_PINS 107
#endif

typedef enum
{
	HUMAX_GPIO_FUNCTION_NCTP = -1,
	HUMAX_GPIO_FUNCTION_GPIO = 0,
	HUMAX_GPIO_FUNCTION_ALT1,
	HUMAX_GPIO_FUNCTION_ALT2,
	HUMAX_GPIO_FUNCTION_ALT3,
	HUMAX_GPIO_FUNCTION_ALT4,
	HUMAX_GPIO_FUNCTION_ALT5,
	HUMAX_GPIO_FUNCTION_ALT6,
	HUMAX_GPIO_FUNCTION_ALT7,
	HUMAX_GPIO_FUNCTION_ALT8,
	HUMAX_GPIO_FUNCTION_ALT9,
	HUMAX_GPIO_FUNCTION_MAX
} HUMAX_GPIO_FUNCTION ;


typedef struct
{
	unsigned int addr;
	unsigned mask;
	unsigned shift;
} HUMAX_GpioTable;

#endif
