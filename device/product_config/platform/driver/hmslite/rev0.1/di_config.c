#ifndef __BOARD_CFE_H__
#define __BOARD_CFE_H__

#include "drv_gpio.h"
#include "drv_tspath.h"
#include "drv_channel_config.h"
#include "di_key.h"
#include "drv_fan.h"

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

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
#include "drv_hdmi.h"
#endif

#define SIZE_ALIGNED(size, align)	((((size)+((1<<align)-1)) >> align) << align)

/* GPIO Configuration */

const GPIO_ConfigInfo_t GPIO_ConfigInfoTable[] =
{
    /* GPIO_ID      GPIO_HANDLER      GPIOTYPE					GPIO_NUMBER      GPIO_MODE      GPIO_INT_MODE      GPIO_INIT_STATE */
	{GPIO_ID_HW_MUTE,		NULL,	NEXUS_GpioType_eAonStandard,	4,	 GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED, 		GPIO_INVAILD},
	{GPIO_ID_nCI_CD1,    	NULL,  	NEXUS_GpioType_eStandard, 		57,    GPIO_MODE_INTERRUPT,	GPIO_INT_MODE_BOTH_EDGES,		GPIO_INVAILD},
	{GPIO_ID_nCI_CD2,    	NULL,  	NEXUS_GpioType_eStandard, 		30,    GPIO_MODE_READ,    	GPIO_INT_MODE_DISABLED,  		GPIO_INVAILD},
	{GPIO_ID_CI_RST,    	NULL,  	NEXUS_GpioType_eStandard,		34,    GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,  		GPIO_LOW},
	{GPIO_ID_CI_PWR,    	NULL,   NEXUS_GpioType_eAonStandard, 	11,    GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
	{GPIO_ID_nCI_OUTEN,    	NULL,   NEXUS_GpioType_eStandard,	58,    GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
//  {GPIO_ID_NVRAM_WP,    	NULL,	NEXUS_GpioType_eStandard, 		16,   GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,    		GPIO_HI},
    {GPIO_ID_HDD_PWR_CTRL,	NULL,   NEXUS_GpioType_eAonStandard, 	12,   GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_LOW},
    {GPIO_ID_nLNBP0_RST,    	NULL,  	NEXUS_GpioType_eStandard, 	70,   GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
    {GPIO_ID_HW_REV0,    	NULL,  	NEXUS_GpioType_eStandard, 	85,   GPIO_MODE_READ,    	GPIO_INT_MODE_DISABLED,    		GPIO_INVAILD},
    {GPIO_ID_HW_REV1,    	NULL,  	NEXUS_GpioType_eStandard, 	86,   GPIO_MODE_READ,   	GPIO_INT_MODE_DISABLED,   		GPIO_INVAILD},

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	//{GPIO_ID_CIMAX_INT,	NULL,	NEXUS_GpioType_eStandard, 	64,		GPIO_MODE_INTERRUPT,	GPIO_INT_MODE_FALLING_EDGE,	GPIO_INVAILD},
	//{GPIO_ID_CIMAX_RESET,	NULL,	NEXUS_GpioType_eStandard,	65,		GPIO_MODE_WRITE,	GPIO_INT_MODE_DISABLED,			GPIO_INVAILD},
#endif
    {GPIO_ID_nCH0_RST,    	NULL,  	NEXUS_GpioType_eStandard, 	75,   GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
  //  {GPIO_ID_nCH1_RST,    	NULL,  	NEXUS_GpioType_eAonStandard, 	2,   GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
#if defined(CONFIG_TS_MATRIX)
//	{GPIO_ID_TS_SEL0,       NULL,   NEXUS_GpioType_eStandard, 50, GPIO_MODE_WRITE, GPIO_INT_MODE_DISABLED, GPIO_INVAILD},
//	{GPIO_ID_TS_SEL2,       NULL,   NEXUS_GpioType_eStandard, 52, GPIO_MODE_WRITE, GPIO_INT_MODE_DISABLED, GPIO_INVAILD},
//	{GPIO_ID_TS_SEL10,      NULL,   NEXUS_GpioType_eStandard, 54, GPIO_MODE_WRITE, GPIO_INT_MODE_DISABLED, GPIO_INVAILD},
//	{GPIO_ID_TS_SEL11,      NULL,   NEXUS_GpioType_eStandard, 55, GPIO_MODE_WRITE, GPIO_INT_MODE_DISABLED, GPIO_INVAILD},
//	{GPIO_ID_TS_SEL12,      NULL,   NEXUS_GpioType_eStandard, 56, GPIO_MODE_WRITE, GPIO_INT_MODE_DISABLED, GPIO_INVAILD},
//	{GPIO_ID_TS_SEL13,      NULL,   NEXUS_GpioType_eStandard, 57, GPIO_MODE_WRITE, GPIO_INT_MODE_DISABLED, GPIO_INVAILD},
//	{GPIO_ID_TS_SEL14,      NULL,   NEXUS_GpioType_eStandard, 19, GPIO_MODE_WRITE, GPIO_INT_MODE_DISABLED, GPIO_INVAILD},
//	{GPIO_ID_TS_SEL15,      NULL,   NEXUS_GpioType_eStandard, 24, GPIO_MODE_WRITE, GPIO_INT_MODE_DISABLED, GPIO_INVAILD},
//	{GPIO_ID_TS_SEL16,      NULL,   NEXUS_GpioType_eStandard, 26, GPIO_MODE_WRITE, GPIO_INT_MODE_DISABLED, GPIO_INVAILD},
//	{GPIO_ID_TS_SEL17,      NULL,   NEXUS_GpioType_eStandard, 87, GPIO_MODE_WRITE, GPIO_INT_MODE_DISABLED, GPIO_INVAILD},
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
		DI_KEY_FRONT_RESERVED_01,				/* Micom KeyID 2 */
		DI_KEY_FRONT_RESERVED_01,				/* Micom KeyID 3 */
		DI_KEY_FRONT_RESERVED_01,			/* Micom KeyID 4 */
		DI_KEY_FRONT_RESERVED_01,				/* Micom KeyID 5 */
		DI_KEY_FRONT_RESERVED_01,				/* Micom KeyID 6 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 7 */
		DI_KEY_FRONT_RESERVED_01,				/* Micom KeyID 8 */
		DI_KEY_FRONT_RESERVED_01,				/* Micom KeyID 9 */
		DI_KEY_FRONT_RESERVED_01,				/* Micom KeyID 10 */
		DI_KEY_FRONT_RESERVED_01,				/* Micom KeyID 11 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 12 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 13 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 14 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 15 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 16 */
		DI_KEY_FRONT_RESERVED_01,				/* Micom KeyID 17 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 18 */
		DI_KEY_FRONT_RESERVED_01,		/* Micom KeyID 19 */
		DI_KEY_FRONT_RESERVED_01,				/* Micom KeyID 20 */
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
		{NEXUS_InputBand_e8},
		{NEXUS_InputBand_e8},
};

HUINT32 DRV_CFG_GetTSMaxDevice(void)
{
	return (sizeof(s_TsIds)/sizeof(DRV_TS_PATH_ID));
}

/* Frontend Configuration */
const DRV_CH_CAPABILITY s_ChConfig[] =
{
	{0, DI_CH_TYPE_SAT},
	{1, DI_CH_TYPE_SAT},
	{2, DI_CH_TYPE_SAT},
	{3, DI_CH_TYPE_SAT},
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

#define DEBUG_MODULE	TRACE_MODULE_DI_INIT
/*Heap*/

// for inputband
#define NUM_DEMOD			4
#define NUM_CI				1

// for parserband
#define NUM_LIVE			1
#define NUM_PIP				0
#define NUM_TSR				1 // 1 or 3
#define NUM_REC				4
#define NUM_TRANSCODING		2
#define NUM_REMUX			1 //not sure

// for pluypump

#define NUM_PLAYBACK					1
#define NUM_THUMNAIL_DECODE				1
#define NUM_AUDIO_PLAYBACK				1
#define NUM_PLAYPUMP_TRANSCODING		(NUM_TRANSCODING*4)

#define NUM_VIDEO_DECODER				(NUM_LIVE + NUM_PIP)
#define SUPPORT_VIDEO_CODEC_SVC			0

#define CONFIG_NUM_INPUT_BANDS				(NUM_DEMOD+NUM_CI)
#define CONFIG_NUM_PARSER_BANDS				(NUM_LIVE+NUM_PIP+NUM_TSR+NUM_REC+NUM_TRANSCODING+NUM_REMUX)
#define CONFIG_NUM_PLAYPUMPS				(NUM_PLAYBACK+NUM_THUMNAIL_DECODE+NUM_AUDIO_PLAYBACK+NUM_PLAYPUMP_TRANSCODING)

/*XVD heaps*/
#define BSPCFG_P_HEAP_SVD_MEMC0_GENERAL_SIZE				(10*1024*1024 + SUPPORT_VIDEO_CODEC_SVC*15*1024*1024)
#define BSPCFG_P_HEAP_SVD_MEMC1_SECURE_SIZE				0
#define BSPCFG_P_HEAP_SVD_MEMC1_PICTURE_SIZE				(25*1024*1024 + SUPPORT_VIDEO_CODEC_SVC*35*1024*1024)

#define BSPCFG_P_HEAP_AVD_MEMC0_GENERAL_SIZE				(NUM_TRANSCODING*18*1024*1024 + NUM_PIP*10*1024*1024)					/*25M*2*/
#define BSPCFG_P_HEAP_AVD_MEMC0_SECURE_SIZE				0
#define BSPCFG_P_HEAP_AVD_MEMC0_PICTURE_SIZE				(NUM_TRANSCODING*44*1024*1024 + NUM_PIP*25*1024*1024)

/* 	- Display buffers required for main memory heap on MEMC1
 *  - Except the main path, all other paths
 *    are allocated from the MEMC1 */

#define BSPCFG_P_DISPLAY_NUM_SD_BUFFERS_MEMC0              0
#define BSPCFG_P_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC0          0
#define BSPCFG_P_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC0        (8 + 1) /* 1080p3D C0V0 with ANR, 4 buffers for 3D 1 buffer for sync channel */
#define BSPCFG_P_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC0    (0)   /* C1V0, for 1080p24/25/30/ */
#define BSPCFG_P_DISPLAY_NUM_HD_BUFFERS_MEMC0              2    /* MCVP QM store */
#define BSPCFG_P_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC0          0

/*  - Buffers for the main path are
 *    allocated from VDC heap on MEMC0 when the window is created */
#define BSPCFG_P_DISPLAY_NUM_SD_BUFFERS_MEMC1              4    /* 1080p source SD PIP C1V1 capture buffer */
#define BSPCFG_P_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC1          0
#define BSPCFG_P_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1         8    /* HD xcode C3V0 with MADR */
#define BSPCFG_P_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC1    (0)   /* 1080p3D PIP C0V1 */
#define BSPCFG_P_DISPLAY_NUM_HD_BUFFERS_MEMC1  		    0
#define BSPCFG_P_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC1 	        0



#if !defined(CONFIG_SHARED_COMPILE)
HUINT32 DRV_CFG_PlatformInit(void)
{

	NEXUS_PlatformSettings platformSettings;
	NEXUS_Error	eNError		= NEXUS_SUCCESS;
	uint8_t uParserBandLoop = 0;
	int i=0;
#if defined(CONFIG_DIRECTFB)
	NEXUS_PlatformStartServerSettings platformStartServerSettings;
	DFB_PlatformSettings dfbSettings;
	DFB_PlatformResult eDfbError	= DFB_PLATFORM_OK;
	NEXUS_HeapHandle * pHeap = NULL;
#endif

	NEXUS_Platform_GetDefaultSettings(&platformSettings);

#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO)
	platformSettings.audioModuleSettings.maxPcmSampleRate = MAX_PCM_SAMPLE_RATE;
#endif

#if defined (CONFIG_DEMOD_INTERNAL)
	platformSettings.openFrontend = true;
#else
	platformSettings.openFrontend = false;
#endif

	NEXUS_TransportModuleSettings *tTransportModuleSettings = &(platformSettings.transportModuleSettings);
	NEXUS_VideoDecoderModuleSettings *tVideoDecoderModuleSettings = &(platformSettings.videoDecoderModuleSettings);

#if SUPPORT_VIDEO_CODEC_SVC	== 0
	tVideoDecoderModuleSettings->supportedCodecs[NEXUS_VideoCodec_eH264_Svc]  = NEXUS_VideoCodec_eNone;
	tVideoDecoderModuleSettings->supportedCodecs[NEXUS_VideoCodec_eH264_Mvc]  = NEXUS_VideoCodec_eNone;
#endif

	tVideoDecoderModuleSettings->heapSize[0].general = BSPCFG_P_HEAP_SVD_MEMC0_GENERAL_SIZE;
	tVideoDecoderModuleSettings->heapSize[0].secure = BSPCFG_P_HEAP_SVD_MEMC1_SECURE_SIZE;
	tVideoDecoderModuleSettings->heapSize[0].picture = BSPCFG_P_HEAP_SVD_MEMC1_PICTURE_SIZE;

	tVideoDecoderModuleSettings->heapSize[1].general = BSPCFG_P_HEAP_AVD_MEMC0_GENERAL_SIZE;
	tVideoDecoderModuleSettings->heapSize[1].secure = BSPCFG_P_HEAP_AVD_MEMC0_SECURE_SIZE;
	tVideoDecoderModuleSettings->heapSize[1].picture = BSPCFG_P_HEAP_AVD_MEMC0_PICTURE_SIZE;

	tVideoDecoderModuleSettings->hostAccessibleHeapIndex = NEXUS_MEMC0_DRIVER_HEAP;

	for(uParserBandLoop = 0; uParserBandLoop < NEXUS_NUM_PLAYPUMPS; uParserBandLoop++)
	{
		tTransportModuleSettings->clientEnabled.playback[uParserBandLoop].rave = false;
		tTransportModuleSettings->clientEnabled.playback[uParserBandLoop].message = false;
		tTransportModuleSettings->clientEnabled.playback[uParserBandLoop].remux[0] = false;
		tTransportModuleSettings->clientEnabled.playback[uParserBandLoop].remux[1] = false;
	}

	for(uParserBandLoop = 0; uParserBandLoop < CONFIG_NUM_PLAYPUMPS; uParserBandLoop++)
	{
		tTransportModuleSettings->clientEnabled.playback[uParserBandLoop].rave = true;
		tTransportModuleSettings->clientEnabled.playback[uParserBandLoop].message = true;
	}

	for(i=0;i<NEXUS_MAX_HEAPS;i++ ) {
        switch(i)
        {
        case NEXUS_MEMC0_PICTURE_BUFFER_HEAP:
            platformSettings.displayModuleSettings.displayHeapSettings[i].fullHdBuffers.count = BSPCFG_P_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC0;
            platformSettings.displayModuleSettings.displayHeapSettings[i].hdBuffers.count = BSPCFG_P_DISPLAY_NUM_HD_BUFFERS_MEMC0;
            platformSettings.displayModuleSettings.displayHeapSettings[i].sdBuffers.count = BSPCFG_P_DISPLAY_NUM_SD_BUFFERS_MEMC0;
            platformSettings.displayModuleSettings.displayHeapSettings[i].fullHdBuffers.pipCount = BSPCFG_P_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC0;
            platformSettings.displayModuleSettings.displayHeapSettings[i].hdBuffers.pipCount = BSPCFG_P_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC0;
            platformSettings.displayModuleSettings.displayHeapSettings[i].sdBuffers.pipCount = BSPCFG_P_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC0;
            break;
        case NEXUS_MEMC1_PICTURE_BUFFER_HEAP:
            platformSettings.displayModuleSettings.displayHeapSettings[i].fullHdBuffers.count = BSPCFG_P_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1;
			platformSettings.displayModuleSettings.displayHeapSettings[i].hdBuffers.count = BSPCFG_P_DISPLAY_NUM_HD_BUFFERS_MEMC1;
            platformSettings.displayModuleSettings.displayHeapSettings[i].sdBuffers.count = BSPCFG_P_DISPLAY_NUM_SD_BUFFERS_MEMC1;
            platformSettings.displayModuleSettings.displayHeapSettings[i].fullHdBuffers.pipCount = BSPCFG_P_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC1;
            platformSettings.displayModuleSettings.displayHeapSettings[i].hdBuffers.pipCount = BSPCFG_P_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC1;
            platformSettings.displayModuleSettings.displayHeapSettings[i].sdBuffers.pipCount = BSPCFG_P_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC1;
            break;
        default:
            platformSettings.displayModuleSettings.displayHeapSettings[i].fullHdBuffers.count = 0;
            platformSettings.displayModuleSettings.displayHeapSettings[i].hdBuffers.count = 0;
            platformSettings.displayModuleSettings.displayHeapSettings[i].sdBuffers.count = 0;
            platformSettings.displayModuleSettings.displayHeapSettings[i].fullHdBuffers.pipCount = 0;
            platformSettings.displayModuleSettings.displayHeapSettings[i].hdBuffers.pipCount = 0;
            platformSettings.displayModuleSettings.displayHeapSettings[i].sdBuffers.pipCount = 0;
            break;
        }
        platformSettings.displayModuleSettings.displayHeapSettings[i].fullHdBuffers.format = NEXUS_VideoFormat_e1080p;
        platformSettings.displayModuleSettings.displayHeapSettings[i].fullHdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
        platformSettings.displayModuleSettings.displayHeapSettings[i].hdBuffers.format = NEXUS_VideoFormat_e1080i;
        platformSettings.displayModuleSettings.displayHeapSettings[i].hdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
        platformSettings.displayModuleSettings.displayHeapSettings[i].sdBuffers.format = NEXUS_VideoFormat_ePalG;
        platformSettings.displayModuleSettings.displayHeapSettings[i].sdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
    }

	/* Enable Teletext function */
	platformSettings.displayModuleSettings.vbi.allowTeletext = true;
	platformSettings.displayModuleSettings.vbi.tteShiftDirMsb2Lsb = true;

#if defined(CONFIG_CRYPTO_NEW_SLOT_NUM)
	PrintError("Over B revision, Allocate slot type 2 for seperate IV keys\n");

    platformSettings.securitySettings.numKeySlotsForType[0] = 60;
    platformSettings.securitySettings.numKeySlotsForType[1] = 0;
	platformSettings.securitySettings.numKeySlotsForType[2] = 30;	/* Allocate slot type 2 for supporting seperate IV keys */
    platformSettings.securitySettings.numKeySlotsForType[3] = 0;
    platformSettings.securitySettings.numKeySlotsForType[4] = 0;
#endif

	/*heap setting*/
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].subIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].size = -1;
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;

#if defined(CONFIG_DIRECTFB)
#define TOTAL_SURFACE_SIZE 90*1024*1024 /* Temporarily, assigned 90M memory for DFB Surface memory. */
#else
#define DEST_RESIZE_SURFACE_SIZE 1920*1080*4
#define SRC_SURFACE_SIZE 1280*720*4
#define DEST_SURFACE_SIZE 1280*720*4
#endif

	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].memcIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].subIndex = 1;
#if defined(CONFIG_DIRECTFB)
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = TOTAL_SURFACE_SIZE;
#else
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = SIZE_ALIGNED((DEST_RESIZE_SURFACE_SIZE +
															SRC_SURFACE_SIZE +
															DEST_SURFACE_SIZE), 20);
#endif
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication;

	platformSettings.heap[NEXUS_MEMC1_GRAPHICS_HEAP].memcIndex = 1;
	platformSettings.heap[NEXUS_MEMC1_GRAPHICS_HEAP].subIndex = 0;
	platformSettings.heap[NEXUS_MEMC1_GRAPHICS_HEAP].size = 0;
	platformSettings.heap[NEXUS_MEMC1_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* cached only */

#define VIDEO_ENCODER_FW 2*1024*1024 //Encoder Firmware
#define VIDEO_ENCODER_FW_LOG 2*1024*1024 //Encoder Firmware debug log
	platformSettings.heap[NEXUS_MEMC1_MAIN_HEAP].memcIndex = 1;
	platformSettings.heap[NEXUS_MEMC1_MAIN_HEAP].subIndex = 0;
	platformSettings.heap[NEXUS_MEMC1_MAIN_HEAP].size = SIZE_ALIGNED((VIDEO_ENCODER_FW + VIDEO_ENCODER_FW_LOG), 20);
	platformSettings.heap[NEXUS_MEMC1_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull; /* cached only */

#define MEMC0_FULLHD_SIZE 1920*1080*2
#define MEMC0_FULLHD_TOTAL_SIZE MEMC0_FULLHD_SIZE*BSPCFG_P_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC0
#define MEMC0_HD_SIZE 1920*1080*2/2
#define MEMC0_HD_TOTAL_SIZE MEMC0_HD_SIZE*BSPCFG_P_DISPLAY_NUM_HD_BUFFERS_MEMC0

	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memcIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 1;
	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = SIZE_ALIGNED((BSPCFG_P_HEAP_AVD_MEMC0_PICTURE_SIZE +
																MEMC0_FULLHD_TOTAL_SIZE +
																MEMC0_HD_TOTAL_SIZE +
																10*1024*1024), 20);
	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* cached only */

#define MEMC1_VIDEO_ENCODER_PICTURE_SIZE NEXUS_VIDEO_ENCODER_ViCE_32MEMC1_PICTURE_HEAP_SIZE
#define MEMC1_FULLHD_SIZE 1920*1080*2
#define MEMC1_FULLHD_TOTAL_SIZE MEMC0_FULLHD_SIZE*BSPCFG_P_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1
#define MEMC1_SD_SIZE 720*576*2
#define MEMC1_SD_TOTAL_SIZE MEMC0_HD_SIZE*BSPCFG_P_DISPLAY_NUM_SD_BUFFERS_MEMC1

	platformSettings.heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].memcIndex = 1;
	platformSettings.heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].subIndex = 0;
	platformSettings.heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].size = SIZE_ALIGNED((BSPCFG_P_HEAP_SVD_MEMC1_PICTURE_SIZE +
																MEMC1_VIDEO_ENCODER_PICTURE_SIZE +
																MEMC1_FULLHD_TOTAL_SIZE +
																MEMC1_SD_TOTAL_SIZE +
																10*1024*1024), 20);
	platformSettings.heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* cached only */

	platformSettings.heap[NEXUS_MEMC0_DRIVER_HEAP].memcIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_DRIVER_HEAP].subIndex = 1;
	platformSettings.heap[NEXUS_MEMC0_DRIVER_HEAP].size = SIZE_ALIGNED((BSPCFG_P_HEAP_SVD_MEMC0_GENERAL_SIZE +
														BSPCFG_P_HEAP_AVD_MEMC0_GENERAL_SIZE +
														NEXUS_VIDEO_ENCODER_ViCE_32MEMC0_SECURE_HEAP_SIZE +
														10*1024*1024), 20);
	platformSettings.heap[NEXUS_MEMC0_DRIVER_HEAP].memoryType = NEXUS_MemoryType_eFull; /* cached only */

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
	//platformSettings.sharedAnalogMemory = false;	//default : phase 6.5 ¢¯¢®¢¥A ¡íc¢ÒoA©ª member ?I¢¬c, default ¡Æ¨£?I¢Òo ¢¬¡¤?¨ö.
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
#if 0
	NEXUS_DisplayModuleSettings *tDisplaySettings = &(platformSettings.displayModuleSettings);
	NEXUS_VideoDecoderModuleSettings *tVideoDecoderModuleSettings = &(platformSettings.videoDecoderModuleSettings);
//	NEXUS_SurfaceModuleSettings *tSurfaceModuleSettings = &(platformSettings.surfacePlatformSettings);
#endif

#if 0
	/*Should call NEXUS_Platform_GetFramebufferHeap for Framebuffer*/
//	tSurfaceModuleSettings->heapIndex = NEXUS_MEMC0_GRAPHICS_HEAP;
	tDisplaySettings->vbi.tteShiftDirMsb2Lsb = true;

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
	40,		/* INITIAL_THRESHOLD */
	35,		/* LOW_THRESHOLD */
	48,		/* LOW_MIDDLE_THRESHOLD */
	51,		/* MIDDLE_THRESHOLD */
	54,		/* MIDDLE_HIGH_THRESHOLD */
	57,		/* HIGH_THRESHOLD */
	68		/* SHUTDOWN_THRESHOLD */
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
	130		/* SHUTDOWN_THRESHOLD */
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
#if defined(CONFIG_DI_MODULE_NETWORK_SHARED_LIB) || defined(CONFIG_DI_MODULE_WIFI_SHARED_LIB)
#include "di_config_network.c"
#else
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

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
const DRV_HDMI_Preemp_Setting_t CustomPreempTable[] =
{
	{NEXUS_VideoFormat_e1080p50hz, NEXUS_HdmiColorDepth_e8bit, 0x39},
	{NEXUS_VideoFormat_eMax, 0, 0},
};
#endif

#endif
