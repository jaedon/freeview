#ifndef __BOARD_CFE_H__
#define __BOARD_CFE_H__

#include "drv_gpio.h"
#include "drv_tspath.h"
#include "drv_channel_config.h"
#include "di_key.h"
#if defined(CONFIG_FRONTKEY_GPIO)
#include "drv_key.h"
#endif

#if defined(CONFIG_DINETWORK)
#include "drv_network.h"
#endif

#include "nexus_platform.h"
#include "linden_trace.h"
#if defined(CONFIG_DIRECTFB)
#include "nexus_platform_server.h"
#include "dfb_platform.h"
#endif

#if defined(CONFIG_BMEM_OPTIMIZE)
#include "di_config.h"
#endif

/* GPIO Configuration */

#define SIZE_ALIGNED(size, align)	((((size)+((1<<align)-1)) >> align) << align)

const GPIO_ConfigInfo_t GPIO_ConfigInfoTable[] =
{
    /* GPIO_ID      GPIO_HANDLER      GPIOTYPE	GPIO_NUMBER      GPIO_MODE      GPIO_INT_MODE      GPIO_INIT_STATE */
    {GPIO_ID_HW_MUTE,    	NULL,   NEXUS_GpioType_eStandard, 		110,    	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_INVAILD},
    {GPIO_ID_nCI_OUTEN,		NULL,   NEXUS_GpioType_eStandard, 		89,    	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
    {GPIO_ID_NVRAM_WP,    	NULL,   NEXUS_GpioType_eStandard, 		114,   	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
#if defined(CONFIG_FRONTKEY_GPIO)
	{GPIO_ID_KEY_STBY,		NULL,   NEXUS_GpioType_eAonStandard, 		6,   	GPIO_MODE_READ,   	GPIO_INT_MODE_DISABLED, 	GPIO_INVAILD},
#endif
#if defined(CONFIG_LED_GPIO)
	{GPIO_ID_LED_STBY,		NULL,	NEXUS_GpioType_eAonStandard,		5,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,			GPIO_HI},
	{GPIO_ID_LED_POWER,		NULL,	NEXUS_GpioType_eAonStandard,		8,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,			GPIO_HI}
#endif
};

#if defined(CONFIG_FRONTKEY_GPIO)

GPIO_KeyMappingTable_t s_GPIO_KeyConfig[] = 
{
	{GPIO_ID_KEY_STBY, "STANDBY", 	1, 	GPIO_INVAILD, 	DI_KEY_RELEASED},
};

HUINT32 DRV_CFG_GetGPIOFrontKeyMapMaxNum(void)
{
	return (sizeof(s_GPIO_KeyConfig)/sizeof(GPIO_KeyMappingTable_t));
}
#endif

HUINT32 DRV_CFG_GetGPIOMaxDevice(void)
{
	return (sizeof(GPIO_ConfigInfoTable)/sizeof(GPIO_ConfigInfo_t));
}

/* Front Key Mapping */
const DI_KEY_FRONT s_BoardFrontKeyMapping[] =
{
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 0 */
		DI_KEY_FRONT_PWR,				/* Micom KeyID 1 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 2 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 3 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 4 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 5 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 6 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 7 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 8 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 9 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 10 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 11 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 12 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 13 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 14 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 15 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 16 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 17 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 18 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 19 */
		DI_KEY_FRONT_RESERVED_01		/* Micom KeyID 20 */
};

HUINT32 DRV_CFG_GetKeyMaxNum(void)
{
	return (sizeof(s_BoardFrontKeyMapping)/sizeof(DI_KEY_FRONT));
}

/* TS Path Configuration */
const DRV_TS_PATH_ID s_TsIds[] =
{
	{NEXUS_InputBand_e3, 0}
};

const DRV_CI_REMUX_PATH_ID s_RemuxIds[] =
{
	{NEXUS_InputBand_e4}
};

HUINT32 DRV_CFG_GetTSMaxDevice(void)
{
	return (sizeof(s_TsIds)/sizeof(DRV_TS_PATH_ID));
}

/* Frontend Configuration */
const DRV_CH_CAPABILITY s_ChConfig[] =
{
	{0, DI_CH_TYPE_SAT}
};

const DRV_CH_TSOUTPUTMODE s_TSConfig[] =
{
	{0x1, CH_TS_MODE_SERIAL}
};

HUINT32 DRV_CFG_GetMaxChannel(void)
{
	return (sizeof(s_ChConfig)/sizeof(DRV_CH_CAPABILITY));
}

#define DEBUG_MODULE	TRACE_MODULE_DI_INIT
DRV_Error DRV_CFG_PlatformInit(void)
{

	NEXUS_PlatformSettings platformSettings;
	NEXUS_Error	eNError		= NEXUS_SUCCESS;

	NEXUS_Platform_GetDefaultSettings(&platformSettings);
	platformSettings.openFpga = false;
#if defined (CONFIG_DEMOD_INTERNAL)
	platformSettings.openFrontend = true;
#else
	platformSettings.openFrontend = false;
#endif
	//platformSettings.sharedAnalogMemory = false;	//default : phase 6.5 ¿¡´Â »ç¶óÁø member ?Ì¸ç, default °ª?Ì¶ó ¸·?½.
	platformSettings.displayModuleSettings.vbi.tteShiftDirMsb2Lsb=true;

#if	defined(BCM_MEMORY_DEBUG)
	NEXUS_SetEnv("debug_mem", "");
#endif

#if defined(BCM_HDMI_BYPASS_EDID)
	NEXUS_SetEnv("hdmi_bypass_edid", "");
#endif

#if defined(CONFIG_CONT_COUNT_IGNORE)
	NEXUS_SetEnv("cont_count_ignore","");
#endif

	eNError = NEXUS_Platform_Init(&platformSettings);
	if(eNError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_Platform_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eNError, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->NEXUS_Platform_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
	return DRV_OK;
}

#endif
