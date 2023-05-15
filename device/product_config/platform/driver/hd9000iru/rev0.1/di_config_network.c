#ifndef __CONFIG_NETWORK__
#define __CONFIG_NETWORK__

#include "di_err.h"
#include "htype.h"

#include "drv_err.h"
#include "drv_network.h"
#include "di_network.h"


const DRV_NETWORK_DEV_INFO_t s_stDevList[] =
{
	{0, "eth0", DI_NETWORK_DEV_LAN},
	{1, "wlan0", DI_NETWORK_DEV_WIFI},
	{2, "ppp0", DI_NETWORK_DEV_PPPOE}
};

HUINT32 DRV_CFG_GetNetMaxDevice(void)
{
	return (sizeof(s_stDevList)/sizeof(DRV_NETWORK_DEV_INFO_t));
}

#endif
