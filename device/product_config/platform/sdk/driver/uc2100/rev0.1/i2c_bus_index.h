#ifndef __HUMAX_I2C_BUS_INDEX_H__
#define __HUMAX_I2C_BUS_INDEX_H__

#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
#define I2C_CHANNEL_CHANNEL		0
#define I2C_CHANNEL_CHANNEL2	4
#define I2C_CHANNEL_HDMI		2
#else
#define I2C_CHANNEL_CHANNEL		3
#define I2C_CHANNEL_HDMI		0
#endif
#endif