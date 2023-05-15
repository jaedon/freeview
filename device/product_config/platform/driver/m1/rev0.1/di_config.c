#ifndef __BOARD_CFE_H__
#define __BOARD_CFE_H__

#include "drv_gpio.h"
#include "drv_tspath.h"
#include "drv_channel_config.h"
#include "di_key.h"
#include "drv_key.h"
#include "drv_fan.h"
#include "flash_map.h"
#include "ram_map.h"
#include "di_loader.h"
#include "di_led.h"
#include "drv_led.h"

#if defined(CONFIG_BMEM_OPTIMIZE)
#include "di_config.h"
#endif

#if defined(CONFIG_DINETWORK)
#include "drv_network.h"
#endif

#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO)
#include "di_audio.h"
#include "drv_audio.h"
#endif

#include "nexus_platform.h"
#include "linden_trace.h"
#if defined(CONFIG_DIRECTFB)
#include "nexus_platform_server.h"
#include "dfb_platform.h"
#endif

#if defined(CONFIG_CRYPTO_NEW_SLOT_NUM)
#include "bchp_sun_top_ctrl.h"
#endif

/* GPIO Configuration */

const GPIO_ConfigInfo_t GPIO_ConfigInfoTable[] =
{
    /* GPIO_ID      GPIO_HANDLER      GPIOTYPE	GPIO_NUMBER      GPIO_MODE      GPIO_INT_MODE      GPIO_INIT_STATE */
	{GPIO_ID_HW_MUTE,    	NULL,   NEXUS_GpioType_eStandard, 		110,    	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_INVAILD},
	{GPIO_ID_SC0_PWR_SEL,    	NULL,   NEXUS_GpioType_eStandard, 		86,    	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
	{GPIO_ID_WIFI_EN,    	NULL,   NEXUS_GpioType_eStandard, 		122,    	GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
#if defined(CONFIG_FRONTKEY_GPIO)
	{GPIO_ID_KEY_STBY,		NULL,   NEXUS_GpioType_eAonStandard, 		6,   	GPIO_MODE_INTERRUPT,   	GPIO_INT_MODE_BOTH_EDGES, 	GPIO_INVAILD},
#endif

#if defined(CONFIG_STLED_MC2003_NEXUS_SPI_USING_GPIO_FOR_STROBE)
	{GPIO_ID_SPI_M_SS0, 	NULL,	NEXUS_GpioType_eAonStandard,		15, 	GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED, 		GPIO_HI},
#endif
	{GPIO_ID_LED_STBY,		NULL,	NEXUS_GpioType_eAonStandard,		5,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,			GPIO_LOW},
	{GPIO_ID_LED_POWER,		NULL,	NEXUS_GpioType_eAonStandard,		8,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,			GPIO_LOW},
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	//{GPIO_ID_WOL,	NULL,	NEXUS_GpioType_eAonStandard,	0,	GPIO_MODE_INTERRUPT,	GPIO_INT_MODE_BOTH_EDGES, GPIO_INVAILD},
	{GPIO_ID_DDR_CLAMPING_CIRCUIT,	NULL,	NEXUS_GpioType_eAonStandard,	13,	 GPIO_MODE_WRITE,	GPIO_INT_MODE_DISABLED, 	GPIO_LOW},
	{GPIO_ID_BOOT_MODE,		NULL,	NEXUS_GpioType_eAonStandard,	14,	 GPIO_MODE_WRITE,	GPIO_INT_MODE_DISABLED, 	GPIO_HI}, /* H : Warm Boot  L : Cold Boot */
#endif
};

#if defined(CONFIG_BMEM_OPTIMIZE)
#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "nexus_surface.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static void mem_usage( NEXUS_PlatformSettings *pPlatformSettings, NEXUS_MemoryConfigurationSettings *pMemConfigSettings ) {
#if NEXUS_HAS_VIDEO_DECODER
    pMemConfigSettings->videoDecoder[0].used      = 1;
    memset( pMemConfigSettings->videoDecoder[0].supportedCodecs, 0, sizeof( pMemConfigSettings->videoDecoder[0].supportedCodecs ));
    pMemConfigSettings->videoDecoder[0].maxFormat = HEVC_MAX_RES; /* 1080p */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[1 ] = 1; /* mpeg1 */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[2 ] = 1; /* mpeg2 */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[3 ] = 1; /* divx */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[4 ] = 1; /* h263 */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[5 ] = 1; /* avc */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[8 ] = 1; /* vc1 */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[9 ] = 1; /* vc1sm */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[10] = 1; /* divx311 */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[11] = 1; /* avs */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[12] = 1; /* rv40 */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[13] = 1; /* vp6 */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[14] = 1; /* vp7 */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[15] = 1; /* vp8 */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[17] = 1; /* spark */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[19] = 1; /* hevc */
    pMemConfigSettings->videoDecoder[0].colorDepth = 8;
    pMemConfigSettings->videoDecoder[0].mosaic.maxNumber = 0;
    pMemConfigSettings->videoDecoder[0].mosaic.maxWidth = 0;
    pMemConfigSettings->videoDecoder[0].mosaic.maxHeight = 0;
    pMemConfigSettings->videoDecoder[0].avc51Supported = 0;
#endif /* if NEXUS_HAS_VIDEO_DECODER */

#if NEXUS_NUM_STILL_DECODES && !defined(CONFIG_PRODUCT_IMAGE_FACTORY)
    pMemConfigSettings->stillDecoder[0].used      = 1;
    memset( pMemConfigSettings->stillDecoder[0].supportedCodecs, 0, sizeof( pMemConfigSettings->stillDecoder[0].supportedCodecs ));
    pMemConfigSettings->stillDecoder[0].maxFormat = HEVC_MAX_RES; /* 1080p */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[1 ] = 1; /* mpeg1 */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[2 ] = 1; /* mpeg2 */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[3 ] = 1; /* divx */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[4 ] = 1; /* h263 */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[5 ] = 1; /* avc */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[8 ] = 1; /* vc1 */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[9 ] = 1; /* vc1sm */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[10] = 1; /* divx311 */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[11] = 1; /* avs */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[12] = 1; /* rv40 */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[13] = 1; /* vp6 */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[14] = 1; /* vp7 */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[15] = 1; /* vp8 */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[17] = 1; /* spark */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[19] = 1; /* hevc */
    pMemConfigSettings->stillDecoder[0].avc51Supported = 0;
#endif /* if NEXUS_NUM_STILL_DECODES*/

#if NEXUS_HAS_DISPLAY && NEXUS_NUM_VIDEO_WINDOWS
    pMemConfigSettings->display[0].maxFormat = HEVC_MAX_RES; /* 1080p */
    pMemConfigSettings->display[0].window[0].used = 1;
    pMemConfigSettings->display[0].window[0].convertAnyFrameRate = 1;
    pMemConfigSettings->display[0].window[0].precisionLipSync    = 1;
    pMemConfigSettings->display[0].window[0].capture             = 1;
    pMemConfigSettings->display[0].window[0].deinterlacer        = 1;
    pMemConfigSettings->display[0].window[0].userCaptureBufferCount = 0;
    pMemConfigSettings->display[1].maxFormat = 13; /* pal */
    pMemConfigSettings->display[1].window[0].used = 1;
    pMemConfigSettings->display[1].window[0].convertAnyFrameRate = 1;
    pMemConfigSettings->display[1].window[0].precisionLipSync    = 1;
    pMemConfigSettings->display[1].window[0].capture             = 1;
    pMemConfigSettings->display[1].window[0].deinterlacer        = 0;
    pMemConfigSettings->display[1].window[0].userCaptureBufferCount = 0;
#endif /* if NEXUS_HAS_DISPLAY && NEXUS_NUM_VIDEO_WINDOWS */

    pPlatformSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
    pPlatformSettings->heap[NEXUS_MEMC0_MAIN_HEAP].size =-1; /* Main */
    pPlatformSettings->heap[NEXUS_MEMC0_MAIN_HEAP].subIndex =0;
    pPlatformSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;

    pPlatformSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memcIndex = 0;
    pPlatformSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size =  -1;//0*1024*1024; /* Picture */;
    pPlatformSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 0;
    pPlatformSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

    pPlatformSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memcIndex = 0;
    pPlatformSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size =  40*1024*1024; /* Graphic*/;
    pPlatformSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].subIndex = 1;
    pPlatformSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* unmapped */

} /* mem_usage */
#endif

#if defined(CONFIG_FRONTKEY_GPIO)
/*
	s_GPIO_KeyConfig ??��ü?? di_config.c?? s_BoardFrontKeyMapping?? ?????? index?? ?????? ?Ѵ?.
	1 : DI_KEY_FRONT_PWR = GPIO_ID_KEY_STBY
	2 ~ n : ???? GPIO KEY ??�� ??
*/

GPIO_KeyMappingTable_t s_GPIO_KeyConfig[] =
{
	{GPIO_ID_KEY_STBY, "STANDBY", 	1, 	GPIO_INVAILD, 	DI_KEY_RELEASED},
/* ???? GPIO KEY ??�� ???? */
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
		DI_KEY_FRONT_RESERVED_01,		/* KeyID 0 */
		DI_KEY_FRONT_PWR,				/* KeyID 1 */
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
#if defined(CONFIG_CHANNEL)
const DI_CH_TunerCapacity_t s_SatTunerCapacity[NUM_OF_CHANNEL] =
{
	/* Tuner 0*/
	{
		{DI_CH_RF_INPUT1,DI_CH_RF_INPUT_NONE,DI_CH_RF_INPUT_NONE,DI_CH_RF_INPUT_NONE},
		1,
		false
	}
};


HUINT32 DRV_CFG_GetMaxChannel(void)
{
	return (sizeof(s_ChConfig)/sizeof(DRV_CH_CAPABILITY));
}
#endif

#define DEBUG_MODULE	TRACE_MODULE_DI_INIT

DRV_Error DRV_CFG_PlatformInit(void)
{

	NEXUS_PlatformSettings platformSettings;
	NEXUS_Error	eNError		= NEXUS_SUCCESS;
	NEXUS_MemoryConfigurationSettings memConfigSettings;
#if defined(CONFIG_DIRECTFB)
	NEXUS_PlatformStartServerSettings platformStartServerSettings;
	DFB_PlatformSettings dfbSettings;
	DFB_PlatformResult eDfbError	= DFB_PLATFORM_OK;
	int i=0;
	NEXUS_HeapHandle * pHeap = NULL;
	bool initOrJoinNexus = false;
#endif
	NEXUS_Platform_GetDefaultSettings(&platformSettings);

	NEXUS_GetDefaultMemoryConfigurationSettings(&memConfigSettings);

#if defined(CONFIG_BMEM_OPTIMIZE)
	mem_usage(&platformSettings, &memConfigSettings);
#endif

#if defined(CONFIG_DIRECTFB)

		platformSettings.openFrontend = true;
		platformSettings.openFpga = false;

		eNError = NEXUS_Platform_MemConfigInit(&platformSettings, &memConfigSettings);
		if(eNError != NEXUS_SUCCESS)
		{
			BKNI_Printf("%s->NEXUS_Platform_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eNError, __LINE__, __FILE__);
			return DRV_ERR;
		}
		else
		{
			BKNI_Printf("%s->NEXUS_Platform_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
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
			BKNI_Printf("%s->NEXUS_Platform_StartSever(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eNError, __LINE__, __FILE__);
			return DRV_ERR;
		}
		else
		{
			BKNI_Printf("%s->NEXUS_Platform_StartServer(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		}
		//DFB_Platform_GetDefaultSettings(DFB_PlatformClientType_eMasterNexusInitialized, &dfbSettings);
		//DFB_Platform_GetDefaultSettings(DFB_PlatformClientType_eNexusMaster, &dfbSettings);

		//* Now we can initialise DFB platform code telling it that we have already initialised Nexus as a master. */
		initOrJoinNexus = false;
		DFB_Platform_GetDefaultSettingsExtended(DFB_PlatformClientType_eNexusMaster, &dfbSettings, initOrJoinNexus);


		dfbSettings.displayOutput[0].init = false;
		dfbSettings.displayWindow[2].init= false;
		dfbSettings.displayWindow[3].init= false;

		for(i=0;i<DFB_PLATFORM_NUM_DISPLAY_WINDOWS_MAX;i++){
		BKNI_Printf(" dfbSettings.displayWindow[%d].init : %d \n", i, dfbSettings.displayWindow[i].init );
		BKNI_Printf(" dfbSettings.displayWindow[%d].handle : %x \n", i, dfbSettings.displayWindow[i].handle );
		BKNI_Printf(" dfbSettings.displayWindow[%d].initSettings.displayIndex : %d \n", i, dfbSettings.displayWindow[i].initSettings.displayIndex );
		BKNI_Printf(" dfbSettings.displayWindow[%d].initSettings.windowIndex : %d \n", i, dfbSettings.displayWindow[i].initSettings.windowIndex );


			}
		eDfbError=DFB_Platform_Init(&dfbSettings);
		if(eDfbError != DFB_PLATFORM_OK)
		{
			BKNI_Printf("%s->DFB_Platform_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDfbError, __LINE__, __FILE__);
			return DRV_ERR;
		}
		else
		{
			BKNI_Printf("%s->DFB_Platform_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		}

#else /* CONFIG_DIRECTFB */

	platformSettings.openFpga = false;
#if defined (CONFIG_DEMOD_INTERNAL)
	platformSettings.openFrontend = true;
#else
	platformSettings.openFrontend = false;
#endif
	//platformSettings.sharedAnalogMemory = false;	//default : phase 6.5 ¿¡´A ?�c¶oAø member ?I¸c, default °ª?I¶o ¸·?½.
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

#if defined(CONFIG_OS_UCOS)
	NEXUS_SetEnv("custom_arc","y");
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].memcIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = -1; //40*1024*1024; /* Graphic*/;
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].subIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* unmapped */

	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memcIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size =  -1;//0*1024*1024; /* Picture */;
	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].size =-1; /* Main */
    platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].subIndex =1; 
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;	
#endif

	eNError = NEXUS_Platform_MemConfigInit(&platformSettings, &memConfigSettings);
	if(eNError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_Platform_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eNError, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->NEXUS_Platform_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

	return DRV_OK;
}

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
	125	/* SHUTDOWN_THRESHOLD */
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

#if defined(CONFIG_LED_CAPABILITY_MODEL) /* LED_CAPABILITY in di_config.c */
/*
LED_NUM8 : Standby
*/
HUINT32 LED_CAPABILITY = (DI_LED_NUM8);
#endif

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
const DRV_HDMI_Preemp_Setting_t CustomPreempTable[] =
{
	{NEXUS_VideoFormat_e1080p50hz, NEXUS_HdmiColorDepth_e8bit, 0x3a},
	{NEXUS_VideoFormat_eMax, 0, 0},
};
#endif

#if defined(CONFIG_OS_UCOS)
const Loader_ImageInfo_t loaderImageInfoTable[] =
{
/* 	Image_Usage_t/ulStoredAddress/ulLoadingAddress /ulSignatureCheckAddress/ulRamdiskRootfsLoadingAddress/ulEntryAddress/defaultImageSize/Image_Type/Image_OS/Flash_Type/bFeature_SecureExec/bFeature_DivideSig/bFeature_EncryptImg */
	{
/* Image_Usage_t */				Image_Usage_NormalBoot,
								{
/* ulStoredAddress */				(N_PART_IMAGE0_SIZE+N_IMAGE_KERNEL_OFFSET),
/* ulLoadingAddress */				0,
/* ulSignatureCheckAddress */		0,
/* ulRamdiskRootfsLoadingAddress */	0,
/* ulEntryAddress */				0,
/* defaultImageSize */				N_IMAGE_KERNEL_SIZE,
/* Image_Type */					Image_Type_BINARY_ZIPPED,
/* Image_OS */					Image_OS_Linux,
/* Flash_Type */					Flash_Type_eMMC,
/* Secure Execution */				FALSE,
/* Divide Signature */				FALSE,
/* Encrypt Image */				FALSE
								}
	},
	{
/* Image_Usage_t */				Image_Usage_FacotryAppBoot,
								{
/* ulStoredAddress */				0,
/* ulLoadingAddress */				INITRD_ROOT_RAM_ADDR,
/* ulSignatureCheckAddress */		0,
/* ulRamdiskRootfsLoadingAddress */	0,
/* ulEntryAddress */				INITRD_ROOT_RAM_ADDR,
/* defaultImageSize */				INITRD_ROOT_RAM_SIZE,
/* Image_Type */					Image_Type_BINARY_ZIPPED,
/* Image_OS */					Image_OS_Linux,
/* Flash_Type */					Flash_Type_eMMC,
/* Secure Execution */				FALSE,
/* Divide Signature */				FALSE,
/* Encrypt Image */				FALSE
								}
	},
	{
/* Image_Usage_t */				Image_Usage_LoaderInfo,
								{
/* ulStoredAddress */				0,
/* ulLoadingAddress */				0,
/* ulSignatureCheckAddress */		0,
/* ulRamdiskRootfsLoadingAddress */	0,
/* ulEntryAddress */				0,
/* defaultImageSize */				N_PART_LOADER_SIZE,
/* Image_Type */					Image_Type_BINARY,
/* Image_OS */					Image_OS_MAX,
/* Flash_Type */					Flash_Type_NOR,
/* Secure Execution */				FALSE,
/* Divide Signature */				FALSE,
/* Encrypt Image */				FALSE
								}
	},
	{
/* Image_Usage_t */				Image_Usage_RootfsInfo,
								{
/* ulStoredAddress */				(N_PART_IMAGE0_OFFSET+N_IMAGE_ROOTFS_OFFSET),
/* ulLoadingAddress */				0,
/* ulSignatureCheckAddress */		0,
/* ulRamdiskRootfsLoadingAddress */	0,
/* ulEntryAddress */				0,
/* defaultImageSize */				N_IMAGE_ROOTFS_SIZE,
/* Image_Type */					Image_Type_BINARY_ZIPPED,
/* Image_OS */					Image_OS_Linux_Ramdisk,
/* Flash_Type */					Flash_Type_eMMC,
/* Secure Execution */				FALSE,
/* Divide Signature */				FALSE,
/* Encrypt Image */				FALSE
								}
	},
	{
/* Image_Usage_t */				Image_Usage_CrytKernelInfo,
								{
/* ulStoredAddress */				(N_PART_IMAGE0_OFFSET+N_IMAGE_KERNEL_OFFSET),
/* ulLoadingAddress */				0,//DECRYPT_RAM_ADDR,
/* ulSignatureCheckAddress */		KERNEL_RAM_CACHED_ADDR,
/* ulRamdiskRootfsLoadingAddress */	0,
/* ulEntryAddress */				CRYPT_KERNEL_RAM_ADDR,
/* defaultImageSize */				N_IMAGE_KERNEL_SIZE,
/* Image_Type */					Image_Type_BINARY_ZIPPED,
/* Image_OS */					Image_OS_Linux_Ramdisk,
/* Flash_Type */					Flash_Type_eMMC,
/* Secure Execution */				FALSE,
/* Divide Signature */				FALSE,
/* Encrypt Image */					FALSE
								}
	},
	{
/* Image_Usage_t */				Image_Usage_CrytRootfsInfo,
								{
/* ulStoredAddress */				(N_PART_IMAGE0_OFFSET+N_IMAGE_ROOTFS_OFFSET),
/* ulLoadingAddress */				0,//INITRD_ROOT_RAM_ADDR,
/* ulSignatureCheckAddress */		ROOTFS_RAM_CACHED_ADDR,
/* ulRamdiskRootfsLoadingAddress */	0,
/* ulEntryAddress */				0,
/* defaultImageSize */				0,//INITRD_ROOT_RAM_SIZE,
/* Image_Type */					Image_Type_BINARY_ZIPPED,
/* Image_OS */					Image_OS_Linux_Ramdisk,
/* Flash_Type */					Flash_Type_eMMC,
/* Secure Execution */				FALSE,
/* Divide Signature */				FALSE,
/* Encrypt Image */					FALSE
								}
	},
	{
/* Image_Usage_t */				Image_Usage_DecompHdfBufInfo,
								{
/* ulStoredAddress */				0,
/* ulLoadingAddress */				HDF_DECOMPRESSED_RAM_ADDR,
/* ulSignatureCheckAddress */		0,
/* ulRamdiskRootfsLoadingAddress */	0,
/* ulEntryAddress */				0,
/* defaultImageSize */				HDF_DECOMPRESSED_RAM_SIZE,
/* Image_Type */					Image_Type_HDF,
/* Image_OS */					Image_OS_MAX,
/* Flash_Type */					Flash_Type_MAX,
/* Secure Execution */				FALSE,
/* Divide Signature */				FALSE,
/* Encrypt Image */				FALSE
								}
	},
	{
/* Image_Usage_t */				Image_Usage_CompHdfBufInfo,
								{
/* ulStoredAddress */				0,
/* ulLoadingAddress */				HDF_COMPRESSED_RAM_ADDR,
/* ulSignatureCheckAddress */		0,
/* ulRamdiskRootfsLoadingAddress */	0,
/* ulEntryAddress */				0,
/* defaultImageSize */				HDF_COMPRESSED_RAM_SIZE,
/* Image_Type */					Image_Type_HDF,
/* Image_OS */					Image_OS_MAX,
/* Flash_Type */					Flash_Type_MAX,
/* Secure Execution */				FALSE,
/* Divide Signature */				FALSE,
/* Encrypt Image */				FALSE
								}
	},
	{
/* Image_Usage_t */				Image_Usage_UpgraderBoot,
								{
/* ulStoredAddress */				(N_PART_OTA_OFFSET+N_IMAGE_OTALDR_OFFSET),
/* ulLoadingAddress */				0,//DECRYPT_RAM_ADDR,
/* ulSignatureCheckAddress */		0,
/* ulRamdiskRootfsLoadingAddress */	0,
/* ulEntryAddress */				0,
/* defaultImageSize */				N_IMAGE_OTALDR_SIZE,
/* Image_Type */					Image_Type_ELF,
/* Image_OS */					Image_OS_Linux_Ramdisk,
/* Flash_Type */					Flash_Type_eMMC,
/* Secure Execution */				FALSE,
/* Divide Signature */				FALSE,
/* Encrypt Image */				FALSE
								}
	},
};

HUINT32 DRV_CFG_GetLoaderImageInfoMax(void)
{
        return (sizeof(loaderImageInfoTable)/sizeof(Loader_ImageInfo_t));
}
#endif


#if defined(CONFIG_LED_REFACTORING)


static int set_led(struct led_setting_t *pSetting)
{
    int rc = -1;
    DRV_Error eDrvErr;

    DONE_ERR((!pSetting));
    
    switch(pSetting->func) {
    case LED_FUNC_WATCHING:
		eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_HI);
		eDrvErr |= DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);
       	DONE_ERR((eDrvErr!=DRV_OK));
        break;
    case LED_FUNC_STANDBY:
    case LED_FUNC_RECORDING:
    case LED_FUNC_NORMAL:
	if(pSetting->id == DI_LED_NUM0)
	{
        	eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);
        	eDrvErr |= DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);
        	DONE_ERR((eDrvErr!=DRV_OK));
	}
	else if(pSetting->id == DI_LED_NUM1)
	{
        	eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);
        	eDrvErr |= DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);
        	DONE_ERR((eDrvErr!=DRV_OK));
	}
	else if(pSetting->id == DI_LED_NUM2)
	{
        	eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);
        	eDrvErr |= DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_HI);
        	DONE_ERR((eDrvErr!=DRV_OK));
	}
	else if(pSetting->id == DI_LED_NUM3)
	{
        	eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);
        	eDrvErr |= DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_HI);
        	DONE_ERR((eDrvErr!=DRV_OK));
	}
        break;

    case LED_FUNC_NONE:
    default:
        break;
    }

    rc = 0;
done:
    return rc;
}

DRV_Error DRV_CFG_LED_PlatformDrvInit(struct led_platform_driver_t *tLedDrv)
{
    DRV_Error eDrvErr = DRV_ERR;
    DONE_ERR((!tLedDrv));

    tLedDrv->led_cap = 0xFFFFFFFF;
    tLedDrv->use_dimming = false;
    tLedDrv->set = set_led;

    eDrvErr = DRV_OK;
done:
    return eDrvErr;
}
#endif



#endif
