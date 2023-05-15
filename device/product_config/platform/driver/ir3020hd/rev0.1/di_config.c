#ifndef __BOARD_CFE_H__
#define __BOARD_CFE_H__

#include "drv_gpio.h"
#include "drv_tspath.h"
#include "drv_channel_config.h"
#include "di_key.h"

#include "nexus_platform.h"
#include "linden_trace.h"

/* GPIO Configuration */

const GPIO_ConfigInfo_t GPIO_ConfigInfoTable[] =
{
	/* GPIO_ID      GPIO_HANDLER      GPIOTYPE	GPIO_NUMBER      GPIO_MODE      GPIO_INT_MODE      GPIO_INIT_STATE */
	{GPIO_ID_HW_MUTE,    	NULL,   NEXUS_GpioType_eStandard, 		78,    	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_INVAILD},
	//{GPIO_ID_nCI_CD1,    	NULL,  	NEXUS_GpioType_eStandard, 		72,    	GPIO_MODE_INTERRUPT,	GPIO_INT_MODE_BOTH_EDGES,    	GPIO_INVAILD},
	//{GPIO_ID_nCI_CD2,    	NULL,  	NEXUS_GpioType_eStandard, 		71,    	GPIO_MODE_READ,    		GPIO_INT_MODE_DISABLED,    		GPIO_INVAILD},
	//{GPIO_ID_CI_RST,    	NULL,   NEXUS_GpioType_eStandard, 		77,    	GPIO_MODE_WRITE,   		GPIO_INT_MODE_DISABLED,    		GPIO_LOW},
	//{GPIO_ID_CI_PWR,    	NULL,   NEXUS_GpioType_eStandard, 		74,    	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
	{GPIO_ID_nCI_OUTEN,		NULL,   NEXUS_GpioType_eStandard, 		89,		GPIO_MODE_WRITE,	GPIO_INT_MODE_DISABLED,		GPIO_HI},
	{GPIO_ID_SC0_PWR_SEL, 	NULL,	NEXUS_GpioType_eStandard,		86,		GPIO_MODE_WRITE,	GPIO_INT_MODE_DISABLED,		GPIO_HI},
	{GPIO_ID_SC1_PWR_SEL, 	NULL,	NEXUS_GpioType_eStandard,		107,		GPIO_MODE_WRITE,	GPIO_INT_MODE_DISABLED,		GPIO_HI},
	//{GPIO_ID_FAN_SPEED,    	NULL,   NEXUS_GpioType_eStandard, 		149,   	GPIO_MODE_INTERRUPT,	GPIO_INT_MODE_FALLING_EDGE,    GPIO_INVAILD},
	//{GPIO_ID_NVRAM_WP,    	NULL,   NEXUS_GpioType_eStandard, 		114,   	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
	//{GPIO_ID_HDD_PWR_CTRL,	NULL,   NEXUS_GpioType_eStandard, 		74,    	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_LOW},
	//{GPIO_ID_FBO_SCART,   	NULL,   NEXUS_GpioType_eStandard, 		113,   	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_LOW},
	//{GPIO_ID_SLB1_SCART,  	NULL,   NEXUS_GpioType_eStandard, 		111,   	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_LOW},
	//{GPIO_ID_SLB2_SCART,    NULL,   NEXUS_GpioType_eStandard, 		112,	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_LOW},
#ifdef CONFIG_STLED
	{GPIO_ID_KEY_STBY,		NULL,   NEXUS_GpioType_eAonStandard, 		6,   	GPIO_MODE_INTERRUPT,   	GPIO_INT_MODE_FALLING_EDGE, 	GPIO_INVAILD},
	{GPIO_ID_IRQ_FRONT,		NULL,   NEXUS_GpioType_eAonStandard, 		3,   	GPIO_MODE_INTERRUPT,   	GPIO_INT_MODE_FALLING_EDGE,   	GPIO_INVAILD},
	{GPIO_ID_SPI_M_CLK,    	NULL,   NEXUS_GpioType_eAonStandard, 		16,   	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
	{GPIO_ID_SPI_M_MOSI,    NULL,   NEXUS_GpioType_eAonStandard, 		17,   	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
	{GPIO_ID_SPI_M_MISO,    NULL,   NEXUS_GpioType_eAonStandard, 		19,   	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
	{GPIO_ID_SPI_M_SS0,    	NULL,   NEXUS_GpioType_eAonStandard, 		15,   	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
#endif
#if defined(CONFIG_LED_GPIO)
	{GPIO_ID_LED_STBY,		NULL,	NEXUS_GpioType_eAonStandard,		5,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,			GPIO_HI},
	{GPIO_ID_LED_POWER,		NULL,	NEXUS_GpioType_eAonStandard,		8,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,			GPIO_HI}
#endif
};

HUINT32 DRV_CFG_GetGPIOMaxDevice(void)
{
	return (sizeof(GPIO_ConfigInfoTable)/sizeof(GPIO_ConfigInfo_t));
}

/* Front Key Mapping */
const DI_KEY_FRONT s_BoardFrontKeyMapping[] =
{
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 0 */
		DI_KEY_FRONT_PWR,				/* Micom KeyID 1 */
		DI_KEY_FRONT_OK,				/* Micom KeyID 2 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 3 */
		DI_KEY_FRONT_MENU,				/* Micom KeyID 4 */
		DI_KEY_FRONT_UP,				/* Micom KeyID 5 */
		DI_KEY_FRONT_DOWN,				/* Micom KeyID 6 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 7 */
		DI_KEY_FRONT_RIGHT,				/* Micom KeyID 8 */
		DI_KEY_FRONT_LEFT,				/* Micom KeyID 9 */
		DI_KEY_FRONT_TV_RADIO,			/* Micom KeyID 10 */
		DI_KEY_FRONT_GUIDE,				/* Micom KeyID 11 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 12 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 13 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 14 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 15 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 16 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 17 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 18 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 19 */
		DI_KEY_FRONT_BACK,				/* Micom KeyID 20 */
};

HUINT32 DRV_CFG_GetKeyMaxNum(void)
{
	return (sizeof(s_BoardFrontKeyMapping)/sizeof(DI_KEY_FRONT));
}

/* TS Path Configuration */
const DRV_TS_PATH_ID s_TsIds[] =
{
	{NEXUS_InputBand_e3, 0},
	{NEXUS_InputBand_e2, 1},
};

const DRV_CI_REMUX_PATH_ID s_RemuxIds[] =
{
		{NEXUS_InputBand_e4},
		{NEXUS_InputBand_e4},
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

#if defined(CONFIG_BCM_7358_SUPPORT_512M)
typedef enum BMEM__HeapId
{
	BMEM_HeapId_General = 0,
	BMEM_HeapId_Xpt = 1,
	BMEM_HeapId_Last
} BMEM__HeapId;

#define MISC_HEAP_SIZE (240*1024*1024)
#define XVD_GENERAL_HEAP_SIZE (36*1024*1024)
#define XPT_HEAP_SIZE	(40*1024*1024)

void DRV_CFG_SetBCMHeapSettings(NEXUS_PlatformSettings *platformSettings)
{
    NEXUS_VideoDecoderModuleSettings *tVideoDecoderModuleSettings = &(platformSettings->videoDecoderModuleSettings);
    tVideoDecoderModuleSettings->avdHeapIndex[0] = BMEM_HeapId_General;
    tVideoDecoderModuleSettings->hostAccessibleHeapIndex = BMEM_HeapId_General;
    tVideoDecoderModuleSettings->heapSize[0].general = XVD_GENERAL_HEAP_SIZE;
    tVideoDecoderModuleSettings->heapSize[0].secure = 0;
    tVideoDecoderModuleSettings->heapSize[0].picture = 0;

    NEXUS_DisplayModuleSettings *tDisplaySettings = &(platformSettings->displayModuleSettings);
    tDisplaySettings->primaryDisplayHeapIndex = BMEM_HeapId_General;
    tDisplaySettings->videoWindowHeapIndex[0][0] = BMEM_HeapId_General;
    tDisplaySettings->videoWindowHeapIndex[1][0] = BMEM_HeapId_General;

    NEXUS_TransportModuleSettings *tTransportModuleSettings = &(platformSettings->transportModuleSettings);
    tTransportModuleSettings->mainHeapIndex = BMEM_HeapId_Xpt;

    NEXUS_SurfaceModuleSettings *tSurfaceModuleSettings = &(platformSettings->surfacePlatformSettings);
    tSurfaceModuleSettings->heapIndex = BMEM_HeapId_General;

	BKNI_Memset(platformSettings->heap, 0x0, sizeof(platformSettings->heap));
	platformSettings->heap[BMEM_HeapId_General].memcIndex = 0;
	platformSettings->heap[BMEM_HeapId_General].subIndex = 1;
	platformSettings->heap[BMEM_HeapId_General].size += MISC_HEAP_SIZE;
	platformSettings->heap[BMEM_HeapId_General].size >>= 20;
	platformSettings->heap[BMEM_HeapId_General].size <<= 20;
	platformSettings->heap[BMEM_HeapId_General].size +=1024*1024;
	platformSettings->heap[BMEM_HeapId_General].memoryType = NEXUS_MemoryType_eFull;
	if(platformSettings->heap[BMEM_HeapId_General].size)
	{
		/*Align 1Mbytes*/
		platformSettings->heap[BMEM_HeapId_General].size >>= 20;
		platformSettings->heap[BMEM_HeapId_General].size <<= 20;
		platformSettings->heap[BMEM_HeapId_General].size +=1024*1024;
	}

	platformSettings->heap[BMEM_HeapId_Xpt].memcIndex = 0;
	platformSettings->heap[BMEM_HeapId_Xpt].subIndex = 0;
	platformSettings->heap[BMEM_HeapId_Xpt].memoryType = NEXUS_MemoryType_eFull;

	platformSettings->heap[BMEM_HeapId_Xpt].size += XPT_HEAP_SIZE;
	if(platformSettings->heap[BMEM_HeapId_Xpt].size)
	{
		/*Align 1Mbytes*/
		platformSettings->heap[BMEM_HeapId_Xpt].size >>= 20;
		platformSettings->heap[BMEM_HeapId_Xpt].size <<= 20;
		platformSettings->heap[BMEM_HeapId_Xpt].size +=1024*1024;
	}
}
#endif

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

#if defined(CONFIG_BCM_7358_SUPPORT_512M)
	DRV_CFG_SetBCMHeapSettings(&platformSettings);
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
