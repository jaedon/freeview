#ifndef __BOARD_CFE_H__
#define __BOARD_CFE_H__

#if defined(CONFIG_BMEM_OPTIMIZE) && !defined(CONFIG_OS_UCOS)
#include "unistd.h"
#endif

#include "drv_gpio.h"
#include "drv_tspath.h"
#include "drv_channel_config.h"
#include "drv_fan.h"
#include "drv_key.h"
#include "drv_led.h"
#include "di_key.h"
#include "di_led.h"
#include "drv_network.h"
#include "drv_micom.h"
#include "linden_trace.h"
#if defined(CONFIG_DIRECTFB)
#include "nexus_platform_server.h"
#include "dfb_platform.h"
#endif
#include "drv_hdmi.h"
#include "flash_map.h"
#include "ram_map.h"
#include "di_loader.h"
#include "nexus_platform.h"

#define SIZE_ALIGNED(size, align_byte)	((((size)+(align_byte-1)) / align_byte) * align_byte)
#define DEBUG_MODULE	TRACE_MODULE_DI_INIT

/* GPIO Configuration */
const GPIO_ConfigInfo_t GPIO_ConfigInfoTable[] =
{
#if defined(CONFIG_STLED) && defined(CONFIG_STLED_MC2003_NEXUS_GPIO)
	/* STLED - MC2003 */
	{GPIO_ID_SPI_M_SS0,			NULL,   NEXUS_GpioType_eStandard,	37, GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,	  GPIO_HI},   /* STB */
	{GPIO_ID_SPI_M_CLK,			NULL,   NEXUS_GpioType_eStandard,	34, GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,	  GPIO_HI},   /* CLK */
	{GPIO_ID_SPI_M_MOSI,		NULL,   NEXUS_GpioType_eStandard,	36, GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,	  GPIO_HI},   /* DIN */
	{GPIO_ID_SPI_M_MISO,		NULL,   NEXUS_GpioType_eStandard,	35, GPIO_MODE_READ,			GPIO_INT_MODE_DISABLED,	  GPIO_INVAILD},   /* DOUT */
#elif defined(CONFIG_STLED) && defined(CONFIG_STLED_MC2003_NEXUS_SPI_USING_GPIO_FOR_STROBE)
	{GPIO_ID_SPI_M_SS0,			NULL,   NEXUS_GpioType_eStandard,	37, GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,	  GPIO_HI},   /* STB */
#endif
	{GPIO_ID_LED_STBY,			NULL,	NEXUS_GpioType_eAonStandard,    2,	GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,	GPIO_LOW},
#if defined(CONFIG_OS_UCOS)
	{GPIO_ID_CH_RST_N,			NULL,	NEXUS_GpioType_eAonStandard,	3,	GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,	GPIO_LOW},
#endif
	{GPIO_ID_WIFI_HOST_WAKE,	NULL,	NEXUS_GpioType_eAonStandard,	10,	GPIO_MODE_READ,			GPIO_INT_MODE_BOTH_EDGES,	GPIO_INVAILD},

#if defined(CONFIG_FRONTKEY_GPIO)
	{GPIO_ID_KEY_STBY,			NULL,	NEXUS_GpioType_eAonStandard,	17,	GPIO_MODE_INTERRUPT,	GPIO_INT_MODE_FALLING_EDGE,	GPIO_INVAILD},
#endif
};

HUINT32 DRV_CFG_GetGPIOMaxDevice(void)
{
	return (sizeof(GPIO_ConfigInfoTable)/sizeof(GPIO_ConfigInfo_t));
}

#if defined(CONFIG_FRONTKEY_GPIO)
GPIO_KeyMappingTable_t s_GPIO_KeyConfig[] =
{
	{GPIO_ID_KEY_STBY,		"STANDBY",		1,	GPIO_INVAILD,	DI_KEY_RELEASED}
};

HUINT32 DRV_CFG_GetGPIOFrontKeyMapMaxNum(void)
{
	return (sizeof(s_GPIO_KeyConfig)/sizeof(GPIO_KeyMappingTable_t));
}
#endif

/* Front Key Mapping */
const DI_KEY_FRONT s_BoardFrontKeyMapping[] =
{
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 0 */
		DI_KEY_FRONT_PWR,				/* Micom KeyID 1 */
		DI_KEY_FRONT_CH_UP,				/* Micom KeyID 2 */
		DI_KEY_FRONT_CH_DOWN,			/* Micom KeyID 3 */
		DI_KEY_FRONT_VOL_UP,			/* Micom KeyID 4 */
		DI_KEY_FRONT_VOL_DOWN,			/* Micom KeyID 5 */
		DI_KEY_FRONT_MENU,				/* Micom KeyID 6 */
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
	{NEXUS_InputBand_e0, 0},
	{NEXUS_InputBand_e1, 1},
	{NEXUS_InputBand_e2, 2},
	{NEXUS_InputBand_e3, 3},
};

const DRV_CI_REMUX_PATH_ID s_RemuxIds[] =
{
		{NEXUS_InputBand_e10},
		{NEXUS_InputBand_e10},
};

const NEXUS_InputBand s_ExtClockInputBand = NEXUS_InputBand_e2;

HUINT32 DRV_CFG_GetTSMaxDevice(void)
{
	return (sizeof(s_TsIds)/sizeof(DRV_TS_PATH_ID));
}

/* Frontend Configuration */
const DRV_CH_CAPABILITY s_ChConfig[] =
{
	{0, DI_CH_TYPE_CAB},
	{1, DI_CH_TYPE_CAB},
	{2, DI_CH_TYPE_CAB},
	{3, DI_CH_TYPE_CAB}
};

const DRV_CH_TSOUTPUTMODE s_TSConfig[] =
{
	{0x1, CH_TS_MODE_MTSIF},
	{0x1, CH_TS_MODE_MTSIF},
	{0x1, CH_TS_MODE_MTSIF},
	{0x1, CH_TS_MODE_MTSIF},
};

#if defined(CONFIG_CHANNEL)
const DI_CH_TunerCapacity_t s_SatTunerCapacity[NUM_OF_CHANNEL] =
{
	/* Tuner 0*/
	{
		{DI_CH_RF_INPUT1,DI_CH_RF_INPUT2,DI_CH_RF_INPUT_NONE,DI_CH_RF_INPUT_NONE},
		2,
		FALSE
	},

	/* Tuner 1*/
	{
		{DI_CH_RF_INPUT1,DI_CH_RF_INPUT2,DI_CH_RF_INPUT_NONE,DI_CH_RF_INPUT_NONE},
		2,
		FALSE
	},

	/* Tuner 2*/
	{
		{DI_CH_RF_INPUT1,DI_CH_RF_INPUT2,DI_CH_RF_INPUT_NONE,DI_CH_RF_INPUT_NONE},
		2,
		FALSE
	},

	/* Tuner 3*/
	{
		{DI_CH_RF_INPUT1,DI_CH_RF_INPUT2,DI_CH_RF_INPUT_NONE,DI_CH_RF_INPUT_NONE},
		2,
		FALSE
	}
};



HUINT32 DRV_CFG_GetMaxChannel(void)
{
	return (sizeof(s_ChConfig)/sizeof(DRV_CH_CAPABILITY));
}
#endif

#if defined(CONFIG_BMEM_OPTIMIZE)
/* Generated by bmemconfig.html on 19700101-005620 */
/*     Platform: 97439 B1 */
/*     Boxmode: 1, Display:HD/SD,Video:HD Main/PIP */
/*     File: /home2/dsshin/temp/nexus_patch/nexus-15.1/nexus/../BSEAV/tools/bmemconfig/bmemconfig.c */
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
    pMemConfigSettings->videoDecoder[0].maxFormat = 47; /* 3840x2160p60 */
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
    pMemConfigSettings->videoDecoder[0].supportedCodecs[16] = 1; /* vp9 */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[17] = 1; /* spark */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[19] = 1; /* hevc */
    pMemConfigSettings->videoDecoder[0].colorDepth = 10;
    pMemConfigSettings->videoDecoder[0].mosaic.maxNumber = 0;
    pMemConfigSettings->videoDecoder[0].mosaic.maxWidth = 0;
    pMemConfigSettings->videoDecoder[0].mosaic.maxHeight = 0;
    pMemConfigSettings->videoDecoder[0].avc51Supported = 0;
    pMemConfigSettings->videoDecoder[1].used      = 1;
    memset( pMemConfigSettings->videoDecoder[1].supportedCodecs, 0, sizeof( pMemConfigSettings->videoDecoder[1].supportedCodecs ));
    pMemConfigSettings->videoDecoder[1].maxFormat = 32; /* 1080p */
    pMemConfigSettings->videoDecoder[1].supportedCodecs[1 ] = 1; /* mpeg1 */
    pMemConfigSettings->videoDecoder[1].supportedCodecs[2 ] = 1; /* mpeg2 */
    pMemConfigSettings->videoDecoder[1].supportedCodecs[3 ] = 1; /* divx */
    pMemConfigSettings->videoDecoder[1].supportedCodecs[4 ] = 1; /* h263 */
    pMemConfigSettings->videoDecoder[1].supportedCodecs[5 ] = 1; /* avc */
    pMemConfigSettings->videoDecoder[1].supportedCodecs[8 ] = 1; /* vc1 */
    pMemConfigSettings->videoDecoder[1].supportedCodecs[9 ] = 1; /* vc1sm */
    pMemConfigSettings->videoDecoder[1].supportedCodecs[10] = 1; /* divx311 */
    pMemConfigSettings->videoDecoder[1].supportedCodecs[11] = 1; /* avs */
    pMemConfigSettings->videoDecoder[1].supportedCodecs[12] = 1; /* rv40 */
    pMemConfigSettings->videoDecoder[1].supportedCodecs[17] = 1; /* spark */
    pMemConfigSettings->videoDecoder[1].supportedCodecs[19] = 1; /* hevc */
    pMemConfigSettings->videoDecoder[1].colorDepth = 8;
    pMemConfigSettings->videoDecoder[1].mosaic.maxNumber = 0;
    pMemConfigSettings->videoDecoder[1].mosaic.maxWidth = 0;
    pMemConfigSettings->videoDecoder[1].mosaic.maxHeight = 0;
    pMemConfigSettings->videoDecoder[1].avc51Supported = 0;
    pMemConfigSettings->videoDecoder[2].used      = 1;
    memset( pMemConfigSettings->videoDecoder[2].supportedCodecs, 0, sizeof( pMemConfigSettings->videoDecoder[2].supportedCodecs ));
    pMemConfigSettings->videoDecoder[2].maxFormat = 31; /* 1080p50 */
    pMemConfigSettings->videoDecoder[2].supportedCodecs[1 ] = 1; /* mpeg1 */
    pMemConfigSettings->videoDecoder[2].supportedCodecs[2 ] = 1; /* mpeg2 */
    pMemConfigSettings->videoDecoder[2].supportedCodecs[3 ] = 1; /* divx */
    pMemConfigSettings->videoDecoder[2].supportedCodecs[4 ] = 1; /* h263 */
    pMemConfigSettings->videoDecoder[2].supportedCodecs[5 ] = 1; /* avc */
    pMemConfigSettings->videoDecoder[2].supportedCodecs[8 ] = 1; /* vc1 */
    pMemConfigSettings->videoDecoder[2].supportedCodecs[9 ] = 1; /* vc1sm */
    pMemConfigSettings->videoDecoder[2].supportedCodecs[10] = 1; /* divx311 */
    pMemConfigSettings->videoDecoder[2].supportedCodecs[11] = 1; /* avs */
    pMemConfigSettings->videoDecoder[2].supportedCodecs[12] = 1; /* rv40 */
    pMemConfigSettings->videoDecoder[2].supportedCodecs[17] = 1; /* spark */
    pMemConfigSettings->videoDecoder[2].supportedCodecs[19] = 1; /* hevc */
    pMemConfigSettings->videoDecoder[2].colorDepth = 10;
    pMemConfigSettings->videoDecoder[2].mosaic.maxNumber = 0;
    pMemConfigSettings->videoDecoder[2].mosaic.maxWidth = 0;
    pMemConfigSettings->videoDecoder[2].mosaic.maxHeight = 0;
    pMemConfigSettings->videoDecoder[2].avc51Supported = 0;
    pMemConfigSettings->videoDecoder[3].used      = 1;
    memset( pMemConfigSettings->videoDecoder[3].supportedCodecs, 0, sizeof( pMemConfigSettings->videoDecoder[3].supportedCodecs ));
    pMemConfigSettings->videoDecoder[3].maxFormat = 31; /* 1080p50 */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[1 ] = 1; /* mpeg1 */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[2 ] = 1; /* mpeg2 */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[3 ] = 1; /* divx */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[4 ] = 1; /* h263 */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[5 ] = 1; /* avc */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[8 ] = 1; /* vc1 */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[9 ] = 1; /* vc1sm */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[10] = 1; /* divx311 */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[11] = 1; /* avs */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[12] = 1; /* rv40 */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[13] = 1; /* vp6 */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[14] = 1; /* vp7 */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[15] = 1; /* vp8 */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[16] = 1; /* vp9 */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[17] = 1; /* spark */
    pMemConfigSettings->videoDecoder[3].supportedCodecs[19] = 1; /* hevc */
    pMemConfigSettings->videoDecoder[3].colorDepth = 10;
    pMemConfigSettings->videoDecoder[3].mosaic.maxNumber = 0;
    pMemConfigSettings->videoDecoder[3].mosaic.maxWidth = 0;
    pMemConfigSettings->videoDecoder[3].mosaic.maxHeight = 0;
    pMemConfigSettings->videoDecoder[3].avc51Supported = 0;
#endif /* if NEXUS_HAS_VIDEO_DECODER */

#if NEXUS_NUM_STILL_DECODES
    pMemConfigSettings->stillDecoder[0].used      = 1;
    memset( pMemConfigSettings->stillDecoder[0].supportedCodecs, 0, sizeof( pMemConfigSettings->stillDecoder[0].supportedCodecs ));
    pMemConfigSettings->stillDecoder[0].maxFormat = 47; /* 3840x2160p60 */
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
    pMemConfigSettings->stillDecoder[0].supportedCodecs[16] = 1; /* vp9 */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[17] = 1; /* spark */
    pMemConfigSettings->stillDecoder[0].supportedCodecs[19] = 1; /* hevc */
    pMemConfigSettings->stillDecoder[0].avc51Supported = 0;
#endif /* if NEXUS_NUM_STILL_DECODES*/

#if NEXUS_HAS_DISPLAY && NEXUS_NUM_VIDEO_WINDOWS
    pMemConfigSettings->display[0].maxFormat = 47; /* 3840x2160p60 */
    pMemConfigSettings->display[0].window[0].used = 1;
    pMemConfigSettings->display[0].window[0].convertAnyFrameRate = 1;
    pMemConfigSettings->display[0].window[0].precisionLipSync    = 1;
    pMemConfigSettings->display[0].window[0].capture             = 1;
    pMemConfigSettings->display[0].window[0].deinterlacer        = 1;
    pMemConfigSettings->display[0].window[0].userCaptureBufferCount = 0;
    pMemConfigSettings->display[0].window[1].used = 0;
    pMemConfigSettings->display[1].maxFormat = 13; /* pal */
    pMemConfigSettings->display[1].window[0].used = 1;
    pMemConfigSettings->display[1].window[0].convertAnyFrameRate = 1;
    pMemConfigSettings->display[1].window[0].precisionLipSync    = 1;
    pMemConfigSettings->display[1].window[0].capture             = 1;
    pMemConfigSettings->display[1].window[0].deinterlacer        = 0;
    pMemConfigSettings->display[1].window[0].userCaptureBufferCount = 0;
    pMemConfigSettings->display[1].window[1].used = 0;
    pMemConfigSettings->display[2].maxFormat = 34; /* 720p */
    pMemConfigSettings->display[2].window[0].used = 1;
    pMemConfigSettings->display[2].window[0].convertAnyFrameRate = 1;
    pMemConfigSettings->display[2].window[0].precisionLipSync    = 0;
    pMemConfigSettings->display[2].window[0].capture             = 1;
    pMemConfigSettings->display[2].window[0].deinterlacer        = 1;
    pMemConfigSettings->display[2].window[0].userCaptureBufferCount = 0;
    pMemConfigSettings->display[2].window[1].used = 0;
    pMemConfigSettings->display[3].maxFormat = 34; /* 720p */
    pMemConfigSettings->display[3].window[0].used = 1;
    pMemConfigSettings->display[3].window[0].convertAnyFrameRate = 1;
    pMemConfigSettings->display[3].window[0].precisionLipSync    = 0;
    pMemConfigSettings->display[3].window[0].capture             = 1;
    pMemConfigSettings->display[3].window[0].deinterlacer        = 1;
    pMemConfigSettings->display[3].window[0].userCaptureBufferCount = 0;
    pMemConfigSettings->display[3].window[1].used = 0;
#endif /* if NEXUS_HAS_DISPLAY && NEXUS_NUM_VIDEO_WINDOWS */

#if NEXUS_HAS_VIDEO_ENCODER
    pMemConfigSettings->videoEncoder[0].used = 1;
    pMemConfigSettings->videoEncoder[1].used = 1;
#endif /* if NEXUS_HAS_VIDEO_ENCODER */
    pPlatformSettings->heap[0].size = 213*1024*1024; /* NEXUS_MEMC0_MAIN_HEAP */

    pPlatformSettings->heap[3].size = 77*1024*1024; /* NEXUS_VIDEO_SECURE_HEAP */

    pPlatformSettings->heap[4].size = 0*1024*1024; /* NEXUS_MEMC0_PICTURE_BUFFER_HEAP */

    pPlatformSettings->heap[5].size = 0*1024*1024; /* NEXUS_MEMC1_PICTURE_BUFFER_HEAP */

    pPlatformSettings->heap[6].size = 90*1024*1024; /* NEXUS_MEMC1_GRAPHICS_HEAP */

    pPlatformSettings->heap[7].size = 0*1024*1024; /* NEXUS_MEMC1_DRIVER_HEAP */

    pPlatformSettings->heap[9].size = 0 * 1024 * 1024;
} /* mem_usage */
#endif

#if !defined(CONFIG_SHARED_COMPILE)
HUINT32 DRV_CFG_PlatformInit(void)
{
	NEXUS_PlatformSettings platformSettings;
	NEXUS_MemoryConfigurationSettings memConfigSettings;
	NEXUS_Error	eNError		= NEXUS_SUCCESS;

#if defined(CONFIG_DIRECTFB)
	NEXUS_PlatformStartServerSettings platformStartServerSettings;
	DFB_PlatformSettings dfbSettings;
	DFB_PlatformResult eDfbError	= DFB_PLATFORM_OK;
	int i=0;
	NEXUS_HeapHandle * pHeap = NULL;
	bool initOrJoinNexus = false;
#endif

	NEXUS_SetEnv("B_REFSW_BOXMODE", "1");
	NEXUS_Platform_GetDefaultSettings(&platformSettings);

#if defined (CONFIG_DEMOD_INTERNAL)
	platformSettings.openFrontend = true;
#else
	platformSettings.openFrontend = false;
#endif

#if defined(CONFIG_CRYPTO_NEW_SLOT_NUM)
	PrintError("Over B revision, Allocate slot type 2 for seperate IV keys\n");
	/* Size of the key table is 2048 64-bit entries. (4096 for B0 chips)
	   There are 5 different key slot types.
	   Type0 requires 34 entries per key slot. Type1 34, Type2 66, Type3 34, Type4 66.
	   The default type for NEXUS key slot allocation is Type0.
	   To support more than 10 Type0 slots, change the slot number
	   60*34 = 2040(OK) < 2048 */
    platformSettings.securitySettings.numKeySlotsForType[0] = 40;	/* for ca, cacp, rmx */
    platformSettings.securitySettings.numKeySlotsForType[1] = 0;
	platformSettings.securitySettings.numKeySlotsForType[2] = 20;	/* Allocate slot type 2 for supporting seperate IV keys */
    platformSettings.securitySettings.numKeySlotsForType[3] = 30;	/* for m2m */
    platformSettings.securitySettings.numKeySlotsForType[4] = 0;
#endif

#if defined(CONFIG_OS_UCOS)
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

#else

	NEXUS_GetDefaultMemoryConfigurationSettings(&memConfigSettings);
#if defined(CONFIG_BMEM_OPTIMIZE)
	mem_usage(&platformSettings, &memConfigSettings);
#endif
#if defined(CONFIG_DIRECTFB)
	platformSettings.heap[NEXUS_MEMC1_GRAPHICS_HEAP].size = 90*1024*1024; /* Graphics */
#endif

	/* Enable Teletext function */
	platformSettings.displayModuleSettings.vbi.allowTeletext = true;
	platformSettings.displayModuleSettings.vbi.tteShiftDirMsb2Lsb = true;

	//NEXUS_SetEnv("NEXUS_BASE_ONLY_INIT","y");
	eNError = NEXUS_Platform_MemConfigInit(&platformSettings, &memConfigSettings);
	if(eNError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_Platform_MemConfigInit(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eNError, __LINE__, __FILE__);
		return DRV_ERR;
	}

#if defined(CONFIG_DIRECTFB)
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

//	DFB_Platform_GetDefaultSettings(DFB_PlatformClientType_eMasterNexusInitialized, &dfbSettings);

	/* Now we can initialise DFB platform code telling it that we have already initialised Nexus as a master. */
	initOrJoinNexus = false;
	DFB_Platform_GetDefaultSettingsExtended(DFB_PlatformClientType_eNexusMaster, &dfbSettings, initOrJoinNexus);

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

#if	defined(BCM_MEMORY_DEBUG)
	NEXUS_SetEnv("debug_mem", "");
#endif

#if defined(BCM_HDMI_BYPASS_EDID)
	NEXUS_SetEnv("hdmi_bypass_edid", "");
#endif

#if defined(CONFIG_CONT_COUNT_IGNORE)
	NEXUS_SetEnv("cont_count_ignore","");
#endif
#endif /* End of CONFIG_DIRECTFB */
#endif /* CONFIG_OS_UCOS */
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

const HUINT32 s_FanFreq = 0x1860;

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
#if defined(CONFIG_DI_MODULE_NETWORK_SHARED_LIB) || defined(CONFIG_DI_MODULE_WIFI_SHARED_LIB) || defined(CONFIG_DI_MODULE_PPPOE_SHARED_LIB)
#include "di_config_network.c"
#else
const DRV_NETWORK_DEV_INFO_t s_stDevList[] =
{
	{0, "eth0", DI_NETWORK_DEV_LAN},
	{1, "wlan0", DI_NETWORK_DEV_WIFI},
};

HUINT32 DRV_CFG_GetNetMaxDevice(void)
{
	return (sizeof(s_stDevList)/sizeof(DRV_NETWORK_DEV_INFO_t));
}
#endif
#endif


#if defined(CONFIG_LED_CAPABILITY_MODEL) /* LED_CAPABILITY in di_config.c */
HUINT32 LED_CAPABILITY = (DI_LED_NUM0|DI_LED_NUM1|DI_LED_NUM2|DI_LED_NUM3); /* STANDBY, POWER, RECORD and NETWORK Status */
#endif

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
const DRV_HDMI_Preemp_Setting_t CustomPreempTable[] =
{
	//{NEXUS_VideoFormat_e1080p50hz, NEXUS_HdmiColorDepth_e8bit, 0x39},
	{NEXUS_VideoFormat_eMax, 0, 0},
};
#endif


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
/* ulLoadingAddress */				DECRYPT_RAM_ADDR,
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
/* ulLoadingAddress */				INITRD_ROOT_RAM_ADDR,
/* ulSignatureCheckAddress */		ROOTFS_RAM_CACHED_ADDR,
/* ulRamdiskRootfsLoadingAddress */	0,
/* ulEntryAddress */				0,
/* defaultImageSize */				INITRD_ROOT_RAM_SIZE,
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
/* ulLoadingAddress */				DECRYPT_RAM_ADDR,
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

#if defined(CONFIG_LED_REFACTORING)
#define POWER_BLUE_LED_ON       0x01
#define POWER_RED_LED_ON        0x00
#define POWER_LED_OFF           0x02


static int set_led(struct led_setting_t *pSetting)
{
    int rc = -1;
    DRV_Error eDrvErr;

    DONE_ERR((!pSetting));

    switch(pSetting->func) {
    case LED_FUNC_WATCHING:
       	eDrvErr = DRV_MICOM_FrontLedControl(POWER_BLUE_LED_ON);
       	DONE_ERR((eDrvErr!=DRV_OK));
        break;
    case LED_FUNC_STANDBY:
    case LED_FUNC_RECORDING:
        eDrvErr = DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);
        DONE_ERR((eDrvErr!=DRV_OK));
        break;
    case LED_FUNC_NORMAL:
        if (pSetting->id == LED_ID_POWER) // front led
        {
            if(pSetting->on == FALSE)
            {
                eDrvErr = DRV_MICOM_FrontLedControl(POWER_LED_OFF);
                DONE_ERR((eDrvErr!=DRV_OK));
            }
        }
        else // front panel
        {
            if(pSetting->on == TRUE)
            {
                eDrvErr = DRV_MICOM_SetLedControl(pSetting->id, TRUE);
                DONE_ERR((eDrvErr!=DRV_OK));
            }
            else
            {
                eDrvErr = DRV_MICOM_SetLedControl(pSetting->id, FALSE);
                DONE_ERR((eDrvErr!=DRV_OK));
            }
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

    tLedDrv->led_cap = LED_ID_POWER;
    tLedDrv->use_dimming = false;
    tLedDrv->set = set_led;

    eDrvErr = DRV_OK;
done:
    return eDrvErr;
}
#endif

#endif
