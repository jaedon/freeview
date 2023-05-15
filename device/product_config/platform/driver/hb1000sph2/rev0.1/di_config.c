#ifndef __BOARD_CFE_H__
#define __BOARD_CFE_H__

#include "drv_gpio.h"
#include "drv_tspath.h"
#include "drv_channel_config.h"
#include "drv_key.h"
#include "drv_fan.h"

#if defined(CONFIG_DINETWORK)
#include "drv_network.h"
#endif

#include "nexus_platform.h"
#include "linden_trace.h"
#if defined(CONFIG_DIRECTFB)
#include "nexus_platform_server.h"
#include "dfb_platform.h"
#endif

/* GPIO Configuration */

const GPIO_ConfigInfo_t GPIO_ConfigInfoTable[] =
{
    /* GPIO_ID      		GPIO_HANDLER    GPIOTYPE						GPIO_NUMBER      GPIO_MODE      	GPIO_INT_MODE      			GPIO_INIT_STATE */
    {GPIO_ID_HW_MUTE,    	NULL,   		NEXUS_GpioType_eStandard, 		27,   			GPIO_MODE_WRITE,    GPIO_INT_MODE_DISABLED,    	GPIO_INVAILD},        
    {GPIO_ID_LNBP_SW_EN,	NULL,			NEXUS_GpioType_eAonStandard, 	10,    			GPIO_MODE_WRITE,	GPIO_INT_MODE_DISABLED,    	GPIO_HI},
	{GPIO_ID_KEY_STBY,		NULL,   		NEXUS_GpioType_eStandard, 		25,   			GPIO_MODE_READ,   	GPIO_INT_MODE_DISABLED, 	GPIO_INVAILD},
    {GPIO_ID_HW_REV0,    	NULL,  			NEXUS_GpioType_eStandard, 		23,    			GPIO_MODE_READ,    	GPIO_INT_MODE_DISABLED,    	GPIO_INVAILD},
    {GPIO_ID_HW_REV1,    	NULL,  			NEXUS_GpioType_eStandard, 		24,    			GPIO_MODE_READ,   	GPIO_INT_MODE_DISABLED,   	GPIO_INVAILD}
    /*{GPIO_ID_HW_REV2,    	NULL,  			NEXUS_GpioType_eStandard, 		25,    			GPIO_MODE_READ,    	GPIO_INT_MODE_DISABLED,    	GPIO_INVAILD}*/
};

HUINT32 DRV_CFG_GetGPIOMaxDevice(void)
{
	return (sizeof(GPIO_ConfigInfoTable)/sizeof(GPIO_ConfigInfo_t));
}

GPIO_KeyMappingTable_t s_GPIO_KeyConfig[] =
{
	/* GPIO_ID				GPIO_NAME		KEY_ID	GPIO_STATE			KEY_TYPE */
	{GPIO_ID_KEY_STBY, 		"STANDBY", 		1, 		GPIO_INVAILD, 		DI_KEY_RELEASED}
};

HUINT32 DRV_CFG_GetGPIOFrontKeyMapMaxNum(void)
{
	return (sizeof(s_GPIO_KeyConfig)/sizeof(GPIO_KeyMappingTable_t));
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
	{0x1, CH_TS_MODE_PARALLEL}
};

#if defined(CONFIG_SUPPORT_SELECTABLE_RF)
const DI_CH_TunerCapacity_t s_SatTunerCapacity[NUM_OF_CHANNEL] =
{
	/* Tuner 0*/
	{
		{DI_CH_RF_INPUT1,DI_CH_RF_INPUT_NONE,DI_CH_RF_INPUT_NONE,DI_CH_RF_INPUT_NONE},
		1,
		FALSE
	}
};
#endif

HUINT32 DRV_CFG_GetMaxChannel(void)
{
	return (sizeof(s_ChConfig)/sizeof(DRV_CH_CAPABILITY));
}

#define DEBUG_MODULE	TRACE_MODULE_DI_INIT
/*Heap*/
/*XVD*/
/*HD+HD is not supported*/
#define BSPCFG_HEAP_XVD_GENERAL_HEAP_SIZE				29*1024*1024	/*FW 1M, General HEAP 23.3M, Still 4.27M===>Main HeaP*/
#define BSPCFG_HEAP_XVD_PICTURE_HEAP_SIZE				62*1024*1024	/*PICTURE HEAP 61.04===>Picture Heap*/
#define BSPCFG_HEAP_XVD_CDBITB_HEAP_SIZE				6*1024*1024	/*CDB and ITB ===>Main Heap*/
#define BSPCFG_HEAP_XVD_SECURE_HEAP_SIZE				0

/*VDC*/
#define BSPCFG_HEAP_VDC_NUM_FULL_HD_BUFFERS_MEMC0		6
#define BSPCFG_HEAP_VDC_NUM_HD_BUFFERS_MEMC0			1
#define BSPCFG_HEAP_VDC_NUM_SD_BUFFERS_MEMC0			0
#define BSPCFG_HEAP_VDC_NUM_FULL_HD_PIP_BUFFERS_MEMC0	4
#define BSPCFG_HEAP_VDC_NUM_HD_PIP_BUFFERS_MEMC0		0
#define BSPCFG_HEAP_VDC_NUM_SD_PIP_BUFFERS_MEMC0		0
#define BSPCFG_HEAP_VDC_HEAP_SIZE						32*1024*1024	/*Dont support PIP and DVI input ==> Picture Heap*/

#define BSPCFG_HEAP_RAP_HEAP_SIZE						11*1024*1024	/*====>Main Heap*/
#define BSPCFG_HEAP_XPT_HEAP_SIZE						25*1024*1024
#define BSPCFG_HEAP_XPT_ACTUAL_HEAP_SIZE				(BSPCFG_HEAP_XPT_HEAP_SIZE + 9*1024*1024	/*Rs buffer and Xc buffer size over 10M. It seems memoryworks sheedt issue*/)

#define BSPCFG_HEAP_NEXUS_HEAP_SIZE						60*1024*1024

/*GRC*/
#define BSPCFG_HEAP_GRC_FRAMEBUFFER_HEAP_SIZE			20*1024*1024	/*===> Main Heap*/
#define BSPCFG_HEAP_GRC_SURFACE_HEAP_SIZE				30*1024*1024	/*APP surface ===> Graphic Heap*/

#if defined(CONFIG_DEVICE_MODE_KERNEL)
#define BSPCFG_HEAP_MEMC0_MAIN_HEAP						(BSPCFG_HEAP_XVD_GENERAL_HEAP_SIZE + BSPCFG_HEAP_RAP_HEAP_SIZE + BSPCFG_HEAP_XPT_ACTUAL_HEAP_SIZE + BSPCFG_HEAP_GRC_FRAMEBUFFER_HEAP_SIZE + BSPCFG_HEAP_GRC_SURFACE_HEAP_SIZE + BSPCFG_HEAP_NEXUS_HEAP_SIZE)/*184M*/
#define BSPCFG_HEAP_PICTURE_BUFFER_HEAP					(BSPCFG_HEAP_XVD_PICTURE_HEAP_SIZE + BSPCFG_HEAP_VDC_HEAP_SIZE) /*94M*/
#define BSPCFG_HEAP_GRAPHICS_HEAP      					(30*1024*1024) /*30*/
#else
#define BSPCFG_HEAP_MEMC0_MAIN_HEAP						(BSPCFG_HEAP_XVD_GENERAL_HEAP_SIZE + BSPCFG_HEAP_RAP_HEAP_SIZE + BSPCFG_HEAP_XPT_ACTUAL_HEAP_SIZE + BSPCFG_HEAP_GRC_FRAMEBUFFER_HEAP_SIZE + BSPCFG_HEAP_NEXUS_HEAP_SIZE)/*154M*/
#define BSPCFG_HEAP_PICTURE_BUFFER_HEAP					(BSPCFG_HEAP_XVD_PICTURE_HEAP_SIZE + BSPCFG_HEAP_VDC_HEAP_SIZE) /*94M*/
#define BSPCFG_HEAP_GRAPHICS_HEAP      					(BSPCFG_HEAP_GRC_SURFACE_HEAP_SIZE) /*30M */
#endif

#if !defined(CONFIG_SHARED_COMPILE)
HUINT32 DRV_CFG_PlatformInit(void)
{

	NEXUS_PlatformSettings platformSettings;
	NEXUS_Error	eNError		= NEXUS_SUCCESS;
#if defined(CONFIG_DIRECTFB)
	NEXUS_PlatformStartServerSettings platformStartServerSettings;
	DFB_PlatformSettings dfbSettings;
	DFB_PlatformResult eDfbError	= DFB_PLATFORM_OK;
	int i=0;
	NEXUS_HeapHandle * pHeap = NULL;
#endif

	NEXUS_Platform_GetDefaultSettings(&platformSettings);

#if defined (CONFIG_DEMOD_INTERNAL)
	platformSettings.openFrontend = true;
#else
	platformSettings.openFrontend = false;
#endif

	/* Enable Teletext function */
	platformSettings.displayModuleSettings.vbi.allowTeletext = true;
	platformSettings.displayModuleSettings.vbi.tteShiftDirMsb2Lsb = true;

#if defined(CONFIG_DIRECTFB)
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
	NEXUS_Platform_GetDefaultStartServerSettings(&platformStartServerSettings);

	platformStartServerSettings.allowUnauthenticatedClients = true;
	platformStartServerSettings.allowUnprotectedClientsToCrash = true;
	platformStartServerSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eUnprotected;
	pHeap = platformStartServerSettings.unauthenticatedConfiguration.heap;
	
	{
		NEXUS_PlatformConfiguration nexusPlatformConfig;
		NEXUS_Platform_GetConfiguration(&nexusPlatformConfig);

		for(i=0;i<NEXUS_MAX_HEAPS;i++)
			pHeap[i] = nexusPlatformConfig.heap[i];
		
	}
	eNError = NEXUS_Platform_StartServer(&platformStartServerSettings);
	if(eNError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_Platform_StartSever(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eNError, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->NEXUS_Platform_StartServer(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
	DFB_Platform_GetDefaultSettings(DFB_PlatformClientType_eMasterNexusInitialized, &dfbSettings);
	eDfbError=DFB_Platform_Init(&dfbSettings);
	if(eDfbError != DFB_PLATFORM_OK)
	{
		PrintError("%s->DFB_Platform_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDfbError, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->DFB_Platform_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

#else /* CONFIG_DIRECTFB */
	platformSettings.openFpga = false;
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

	NEXUS_DisplayModuleSettings *tDisplaySettings = &(platformSettings.displayModuleSettings);
	NEXUS_VideoDecoderModuleSettings *tVideoDecoderModuleSettings = &(platformSettings.videoDecoderModuleSettings);
//	NEXUS_SurfaceModuleSettings *tSurfaceModuleSettings = &(platformSettings.surfacePlatformSettings);

	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].subIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].size = BSPCFG_HEAP_MEMC0_MAIN_HEAP;
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;

	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memcIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 1;
	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = BSPCFG_HEAP_PICTURE_BUFFER_HEAP;
	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].memcIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].subIndex = 1;
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = BSPCFG_HEAP_GRAPHICS_HEAP;
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* cached only */

	tVideoDecoderModuleSettings->heapSize[0].general = BSPCFG_HEAP_XVD_GENERAL_HEAP_SIZE;
	tVideoDecoderModuleSettings->heapSize[0].secure = BSPCFG_HEAP_XVD_SECURE_HEAP_SIZE;
	tVideoDecoderModuleSettings->heapSize[0].picture = BSPCFG_HEAP_XVD_PICTURE_HEAP_SIZE;

	/*Should call NEXUS_Platform_GetFramebufferHeap for Framebuffer*/
//	tSurfaceModuleSettings->heapIndex = NEXUS_MEMC0_GRAPHICS_HEAP;
	tDisplaySettings->vbi.tteShiftDirMsb2Lsb = true;

	tDisplaySettings->fullHdBuffers.count = BSPCFG_HEAP_VDC_NUM_FULL_HD_BUFFERS_MEMC0;
	tDisplaySettings->fullHdBuffers.pipCount = BSPCFG_HEAP_VDC_NUM_FULL_HD_PIP_BUFFERS_MEMC0;
	tDisplaySettings->hdBuffers.count = BSPCFG_HEAP_VDC_NUM_HD_BUFFERS_MEMC0;
	tDisplaySettings->hdBuffers.pipCount = BSPCFG_HEAP_VDC_NUM_HD_PIP_BUFFERS_MEMC0;
	tDisplaySettings->sdBuffers.count = BSPCFG_HEAP_VDC_NUM_SD_BUFFERS_MEMC0;
	tDisplaySettings->sdBuffers.pipCount = BSPCFG_HEAP_VDC_NUM_SD_PIP_BUFFERS_MEMC0;
	tDisplaySettings->primaryDisplayHeapIndex = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;

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
#endif /* End of CONFIG_DIRECTFB */
	return DRV_OK;
}
#endif

#if defined(CONFIG_FAN_SCENARIO_30)
/************************************************************************************
	FAN
*************************************************************************************/
/* FAN Execution Temperature */
const HUINT8 s_FanExecuteTemp[7] =
{
	40,	/* INITIAL_THRESHOLD */
	35,	/* LOW_THRESHOLD */
	53,	/* LOW_MIDDLE_THRESHOLD */
	56,	/* MIDDLE_THRESHOLD */
	59,	/* MIDDLE_HIGH_THRESHOLD */
	60,	/* HIGH_THRESHOLD */
	68	/* SHUTDOWN_THRESHOLD */
};

/* FAN Execution Temperature */
const HUINT8 s_FanExecuteCPUTemp[7] =
{
	105,	/* INITIAL_THRESHOLD */
	100,	/* LOW_THRESHOLD */
	103,	/* LOW_MIDDLE_THRESHOLD */
	108,	/* MIDDLE_THRESHOLD */
	111,	/* MIDDLE_HIGH_THRESHOLD */
	114,	/* HIGH_THRESHOLD */
	120		/* SHUTDOWN_THRESHOLD */
};


const HUINT32 s_FanFreq = 0x0270;

HUINT32 DRV_CFG_GetFanInterval(HUINT32 ulVolt)
{
	HUINT32 usOnInterval;
	if( ulVolt >= DRV_FAN_CTRL_HIGH )
	{
		usOnInterval = 255;
	}
	else if( ulVolt >= DRV_FAN_CTRL_MIDDLE_HIGH )
	{
		usOnInterval = 106;
	}
	else if( ulVolt >= DRV_FAN_CTRL_MIDDLE )
	{
		usOnInterval = 60;
	}
	else if( ulVolt >= DRV_FAN_CTRL_LOW_MIDDLE )
	{
		usOnInterval = 39;
	}
	else if( ulVolt >= DRV_FAN_CTRL_LOW )
	{
		usOnInterval = 29;
	}
	else
	{
		usOnInterval = 0;
	}
	return usOnInterval;
}
#endif

#if defined(CONFIG_DINETWORK)
const DRV_NETWORK_DEV_INFO_t s_stDevList[] =
{
	{0, "eth0", DI_NETWORK_DEV_LAN},
	{1, "wlan0", DI_NETWORK_DEV_WIFI}
};

HUINT32 DRV_CFG_GetNetMaxDevice(void)
{
	return (sizeof(s_stDevList)/sizeof(DRV_NETWORK_DEV_INFO_t));
}
#endif

#endif
