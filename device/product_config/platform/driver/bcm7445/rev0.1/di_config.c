#ifndef __BOARD_CFE_H__
#define __BOARD_CFE_H__

#include "drv_gpio.h"
#include "drv_tspath.h"
#include "drv_channel_config.h"
#include "di_key.h"
#include "di_led.h"
#include "di_system.h"
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

#if defined(CONFIG_CRYPTO_NEW_SLOT_NUM)
#include "bchp_sun_top_ctrl.h"
#endif

/* GPIO Configuration */

const GPIO_ConfigInfo_t GPIO_ConfigInfoTable[] =
{
    /* GPIO_ID      GPIO_HANDLER      GPIOTYPE	GPIO_NUMBER      GPIO_MODE      GPIO_INT_MODE      GPIO_INIT_STATE */
 	{GPIO_ID_FAN_SPEED, 	NULL,	NEXUS_GpioType_eStandard,		71, 	GPIO_MODE_INTERRUPT,			GPIO_INT_MODE_RISING_EDGE, 	GPIO_INVAILD},
	{GPIO_ID_HDD_PWR_CTRL, 	NULL,	NEXUS_GpioType_eStandard,		101, 	GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED, 	GPIO_INVAILD},
	{GPIO_ID_HW_MUTE, 		NULL,	NEXUS_GpioType_eStandard,		104, 	GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED, 	GPIO_INVAILD}
};

HUINT32 DRV_CFG_GetGPIOMaxDevice(void)
{
	return (sizeof(GPIO_ConfigInfoTable)/sizeof(GPIO_ConfigInfo_t));
}

/* Front Key Mapping */
const DI_KEY_FRONT s_BoardFrontKeyMapping[] =
{
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 0 */
	DI_KEY_FRONT_PWR,                         /* Micom KeyID 1 */
	DI_KEY_FRONT_CH_DOWN,                     /* Micom KeyID 2 */
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 3 */
	DI_KEY_FRONT_CH_UP,                       /* Micom KeyID 4 */
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 5 */
	DI_KEY_FRONT_HIDDEN,                      /* Micom KeyID 6 */
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 7 */
	DI_KEY_FRONT_NETWORK,                     /* Micom KeyID 8 */
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 9 */
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 10 */
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 11 */
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 12 */
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 13 */
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 14 */
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 15 */
	DI_KEY_FRONT_WPS,                 		    /* Micom KeyID 16 */
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 17 */
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 18 */
	DI_KEY_FRONT_RESERVED_01,                 /* Micom KeyID 19 */
	DI_KEY_FRONT_RESERVED_01                  /* Micom KeyID 20 */
};


HUINT32 DRV_CFG_GetKeyMaxNum(void)
{
	return (sizeof(s_BoardFrontKeyMapping)/sizeof(DI_KEY_FRONT));
}

/* TS Path Configuration */
const DRV_TS_PATH_ID s_TsIds[] =
{
	{NEXUS_InputBand_e4, 0},
	{NEXUS_InputBand_e5, 1},
	{NEXUS_InputBand_e6, 2},
	{NEXUS_InputBand_e7, 3},
	
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
	{0, DI_CH_TYPE_CAB},
	{1, DI_CH_TYPE_CAB},
	{2, DI_CH_TYPE_CAB},
	{3, DI_CH_TYPE_CAB},	
};

const DRV_CH_TSOUTPUTMODE s_TSConfig[] =
{
	{0x1, CH_TS_MODE_SERIAL},
	{0x1, CH_TS_MODE_SERIAL},
	{0x1, CH_TS_MODE_SERIAL},
	{0x1, CH_TS_MODE_SERIAL},	
};


HUINT32 DRV_CFG_GetMaxChannel(void)
{
	return (sizeof(s_ChConfig)/sizeof(DRV_CH_CAPABILITY));
}

const DRV_NETWORK_DEV_INFO_t s_stDevList[] =
{
	{0, "eth0", DI_NETWORK_DEV_LAN},
	{1, "wlan0", DI_NETWORK_DEV_WIFI}
};

HUINT32 DRV_CFG_GetNetMaxDevice(void)
{
	return (sizeof(s_stDevList)/sizeof(DRV_NETWORK_DEV_INFO_t));
}

#define DEBUG_MODULE	TRACE_MODULE_DI_INIT

#if !defined(CONFIG_SHARED_COMPILE)
/* If the user supplied video decoder settings not available,  then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_VideoDecoderSettings(NEXUS_VideoDecoderModuleSettings *pSettings)
{
    /* By convention the first XVD device is the SHVD0 decoder which has its picture
     * buffers on MEMC2 */
    pSettings->heapSize[0].general = NEXUS_VIDEO_DECODER_SHVD0_MEMC0_GENERAL_HEAP_SIZE;
    pSettings->heapSize[0].secure =  NEXUS_VIDEO_DECODER_SHVD0_MEMC0_SECURE_HEAP_SIZE;
    pSettings->heapSize[0].picture = NEXUS_VIDEO_DECODER_SHVD0_MEMC2_PICTURE_HEAP_SIZE;

    /* The second XVD device is the HVD1 decoder has its picture buffers on MEMC0 */
    pSettings->heapSize[1].general = NEXUS_VIDEO_DECODER_HVD1_MEMC0_GENERAL_HEAP_SIZE;
    pSettings->heapSize[1].secure =  NEXUS_VIDEO_DECODER_HVD1_MEMC0_SECURE_HEAP_SIZE;
    pSettings->heapSize[1].picture = NEXUS_VIDEO_DECODER_HVD1_MEMC0_PICTURE_HEAP_SIZE;

    /* The third XVD device is the HVD2 decoder has its picture buffers on MEMC0 */
    pSettings->heapSize[2].general = NEXUS_VIDEO_DECODER_HVD2_MEMC0_GENERAL_HEAP_SIZE;
    pSettings->heapSize[2].secure =  NEXUS_VIDEO_DECODER_HVD2_MEMC0_SECURE_HEAP_SIZE;
    pSettings->heapSize[2].picture = NEXUS_VIDEO_DECODER_HVD2_MEMC1_PICTURE_HEAP_SIZE;

    /* how the picture buffers are connected to the memory controller heaps for HEVC0,HVD1 & HVD2 */
    pSettings->avdHeapIndex[0] = NEXUS_MEMC2_PICTURE_BUFFER_HEAP; /* HEVC0 */
    pSettings->avdHeapIndex[1] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP; /* HVD1*/
    pSettings->avdHeapIndex[2] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP; /* HVD2 */

    /* [decoder] to mfd mapping, each hw decoder can do two decodes */
    pSettings->mfdMapping[0] = 0; /* 1st decoder is mapped to MPEG feeder 0 (SHVD0)*/
    pSettings->mfdMapping[1] = 1; /* 2nd decoder is mapped to MPEG feeder 1 (SHVD0)*/
    pSettings->mfdMapping[2] = 4; /* 3rd decoder is mapped to MPEG feeder 4 (HVD1)*/
    pSettings->mfdMapping[3] = 5; /* 4th decoder is mapped to MPEG feeder 5 (HVD1) */
    pSettings->mfdMapping[4] = 2; /* 5th decoder is mapped to MPEG feeder 2 (HVD2)*/
    pSettings->mfdMapping[5] = 3; /* 6th decoder is mapped to MPEG feeder 3 (HVD2)*/

    /* [decoder] to physical deocder instance SHVD0,HVD1,HVD2 mapping */
    pSettings->avdMapping[0] = 0; /* 1st decoder is mapped to SHVD0 */
    pSettings->avdMapping[1] = 0; /* 2nd decoder is mapped to SHVD0 */
    pSettings->avdMapping[2] = 1; /* 3rd decoder is mapped to HVD1 */
    pSettings->avdMapping[3] = 1; /* 4th decoder is mapped to HVD1 */
    pSettings->avdMapping[4] = 2; /* 5th decoder is mapped to HVD2 */
    pSettings->avdMapping[5] = 2; /* 6th decoder is mapped to HVD2 */

    /* set decoder general heap to heap 0 */
    pSettings->hostAccessibleHeapIndex = NEXUS_MEMC0_MAIN_HEAP;
    pSettings->supportedCodecs[NEXUS_VideoCodec_eH265] = NEXUS_VideoCodec_eH265;
}

/* If the user supplied video encoder settings not available,   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_VideoEncoderSettings(NEXUS_VideoEncoderModuleSettings *pSettings)
{
    /* According to vce memory spreadsheet, encoder has its picture buffers on MEMC1 */
    pSettings->heapSize[1].general = 0;/* encoder default: no use for current hw */
    pSettings->heapSize[1].secure =  NEXUS_VIDEO_ENCODER_ViCE_MEMC0_SECURE_HEAP_SIZE;
    pSettings->heapSize[1].picture = NEXUS_VIDEO_ENCODER_ViCE_MEMC1_PICTURE_HEAP_SIZE;

    pSettings->heapIndex[1].firmware[0] = NEXUS_MEMC1_GENERAL_HEAP;
    pSettings->heapIndex[1].firmware[1] = NEXUS_MEMC1_GENERAL_HEAP;
    pSettings->heapIndex[1].system  = NEXUS_MEMC1_GENERAL_HEAP; /* FW debug log buffer */
    pSettings->heapIndex[1].output = NEXUS_MEMC0_MAIN_HEAP; /* CPU accessible */
    pSettings->heapIndex[1].general = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* same as picture heap for UMA */
    pSettings->heapIndex[1].secure  = NEXUS_VIDEO_SECURE_HEAP; /* must be memc0 */
    pSettings->heapIndex[1].picture = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* unmapped */
    pSettings->heapSize[0].general = 0;/* encoder default: no use for current hw */
    pSettings->heapSize[0].secure =  NEXUS_VIDEO_ENCODER_ViCE_MEMC0_SECURE_HEAP_SIZE;
    pSettings->heapSize[0].picture = NEXUS_VIDEO_ENCODER_ViCE_MEMC0_PICTURE_HEAP_SIZE;

    pSettings->heapIndex[0].firmware[0] = NEXUS_MEMC0_MAIN_HEAP;
    pSettings->heapIndex[0].firmware[1] = NEXUS_MEMC0_MAIN_HEAP;
    pSettings->heapIndex[0].system  = NEXUS_MEMC0_MAIN_HEAP; /* FW debug log buffer */
    pSettings->heapIndex[0].output = NEXUS_MEMC0_MAIN_HEAP; /* CPU accessible */
    pSettings->heapIndex[0].general = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;/* same as picture heap for UMA */
    pSettings->heapIndex[0].secure  = NEXUS_VIDEO_SECURE_HEAP; /* must be memc0 */
    pSettings->heapIndex[0].picture = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;/* unmapped */
    pSettings->vceMapping[0].device = 0;
    pSettings->vceMapping[0].channel = 0;
    pSettings->vceMapping[1].device = 0;
    pSettings->vceMapping[1].channel = 1;
    pSettings->vceMapping[2].device = 1;
    pSettings->vceMapping[2].channel = 0;
    pSettings->vceMapping[3].device = 1;
    pSettings->vceMapping[3].channel = 1;

    return;
}

/* If the user supplied video decoder settings not available,   then use default  Video Buffers based on the chip usage modes */
#ifdef NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1
#undef NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1	8+4/* HD xcode C3V0 with MADR */ /*TRANSCODER + PIP*/
#endif
static void NEXUS_Platform_P_DisplaySettings(NEXUS_DisplayModuleSettings *pSettings)
{
    unsigned i;
    pSettings->primaryDisplayHeapIndex = NEXUS_MEMC2_PICTURE_BUFFER_HEAP;
    pSettings->rdcHeapIndex            = NEXUS_MEMC2_GENERAL_HEAP; /* memc2 and host accessible */

    for (i=0;i<NEXUS_MAX_HEAPS;i++ )
    {
        switch (i)
        {
        case NEXUS_MEMC0_PICTURE_BUFFER_HEAP:
            pSettings->displayHeapSettings[i].fullHdBuffers.count = NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC0;
            pSettings->displayHeapSettings[i].hdBuffers.count = NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC0;
            pSettings->displayHeapSettings[i].sdBuffers.count = NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC0;
            pSettings->displayHeapSettings[i].fullHdBuffers.pipCount = NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC0;
            pSettings->displayHeapSettings[i].hdBuffers.pipCount = NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC0;
            pSettings->displayHeapSettings[i].sdBuffers.pipCount = NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC0;
            break;
        case NEXUS_MEMC1_PICTURE_BUFFER_HEAP:
            pSettings->displayHeapSettings[i].fullHdBuffers.count = NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1;
            pSettings->displayHeapSettings[i].hdBuffers.count = NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC1;
            pSettings->displayHeapSettings[i].sdBuffers.count = NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC1;
            pSettings->displayHeapSettings[i].fullHdBuffers.pipCount = NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC1;
            pSettings->displayHeapSettings[i].hdBuffers.pipCount = NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC1;
            pSettings->displayHeapSettings[i].sdBuffers.pipCount = NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC1;

            break;
        case NEXUS_MEMC2_PICTURE_BUFFER_HEAP:
            pSettings->displayHeapSettings[i].quadHdBuffers.count = NEXUS_DISPLAY_NUM_QUAD_HD_BUFFERS_MEMC2;
            pSettings->displayHeapSettings[i].fullHdBuffers.count = NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC2;
            pSettings->displayHeapSettings[i].hdBuffers.count = NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC2;
            pSettings->displayHeapSettings[i].sdBuffers.count = NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC2;
             pSettings->displayHeapSettings[i].quadHdBuffers.pipCount = NEXUS_DISPLAY_NUM_QUAD_HD_PIP_BUFFERS_MEMC2;
            pSettings->displayHeapSettings[i].fullHdBuffers.pipCount = NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC2;
            pSettings->displayHeapSettings[i].hdBuffers.pipCount = NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC2;
            pSettings->displayHeapSettings[i].sdBuffers.pipCount = NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC2;
            break;
        default:
            pSettings->displayHeapSettings[i].fullHdBuffers.count = 0;
            pSettings->displayHeapSettings[i].hdBuffers.count = 0;
            pSettings->displayHeapSettings[i].sdBuffers.count = 0;
            pSettings->displayHeapSettings[i].fullHdBuffers.pipCount = 0;
            pSettings->displayHeapSettings[i].hdBuffers.pipCount = 0;
            pSettings->displayHeapSettings[i].sdBuffers.pipCount = 0;
            break;
        }
        pSettings->displayHeapSettings[i].quadHdBuffers.format = NEXUS_VideoFormat_e3840x2160p24hz;
        pSettings->displayHeapSettings[i].quadHdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
        pSettings->displayHeapSettings[i].fullHdBuffers.format = NEXUS_VideoFormat_e1080p;
        pSettings->displayHeapSettings[i].fullHdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
        pSettings->displayHeapSettings[i].hdBuffers.format = NEXUS_VideoFormat_e1080i;
        pSettings->displayHeapSettings[i].hdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
        pSettings->displayHeapSettings[i].sdBuffers.format = NEXUS_VideoFormat_ePalG;
        pSettings->displayHeapSettings[i].sdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
    }


   /*
    * Based on RTS settings
    */
   /*[display][window]*/
   pSettings->videoWindowHeapIndex[0][0] = NEXUS_MEMC2_PICTURE_BUFFER_HEAP;/* C0V0: local HD display main window */
   pSettings->videoWindowHeapIndex[0][1] = NEXUS_MEMC2_PICTURE_BUFFER_HEAP; /* TBD */
   pSettings->videoWindowHeapIndex[1][0] = NEXUS_MEMC2_PICTURE_BUFFER_HEAP;/* C2V2: local SD main */
   pSettings->videoWindowHeapIndex[1][1] = NEXUS_MEMC2_PICTURE_BUFFER_HEAP; /* TBD */
   pSettings->videoWindowHeapIndex[2][0] = NEXUS_MEMC2_PICTURE_BUFFER_HEAP; /* TBD */
   
   pSettings->videoWindowHeapIndex[3][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* xcode 3 */
   pSettings->videoWindowHeapIndex[4][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* xcode 2 */
   pSettings->videoWindowHeapIndex[5][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* xcode 1 */
   pSettings->videoWindowHeapIndex[6][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* xcode 0 */
   
   pSettings->encoderTgMapping[0] = 0;
   pSettings->encoderTgMapping[1] = 1;
   pSettings->encoderTgMapping[2] = 2;
   pSettings->encoderTgMapping[3] = 3;
}

#define PAGE_ALIGN(x) (((unsigned)(x)+0xfff)& ~0xfff)

static void NEXUS_Platform_P_GetDefaultSettings_by_Custom(NEXUS_PlatformSettings *pSettings)
{
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].size =  PAGE_ALIGN(192*1024*1024); /*decoder FW+general,xpt playback,audio other general purpose */
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;

    pSettings->heap[NEXUS_VIDEO_SECURE_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_VIDEO_SECURE_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_VIDEO_SECURE_HEAP].size = PAGE_ALIGN(148*1024 *1024); /* CABACs(28)for 3 decoders + RAVE CDB(6+15) */
    pSettings->heap[NEXUS_VIDEO_SECURE_HEAP].memoryType = NEXUS_MemoryType_eSecure;


    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 1;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = PAGE_ALIGN(193*1024 *1024);
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly;


    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].memcIndex = 1;
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].size = PAGE_ALIGN(193 * 1024 *1024);
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly;

    pSettings->heap[NEXUS_MEMC1_GENERAL_HEAP].memcIndex = 1;
    pSettings->heap[NEXUS_MEMC1_GENERAL_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC1_GENERAL_HEAP].size = PAGE_ALIGN(NEXUS_VIDEO_ENCODER_ViCE_MEMC0_SECURE_HEAP_SIZE); /* add some graphics buffers to 
                                                                                                         distribute m2mc load acrosss various memc */
    pSettings->heap[NEXUS_MEMC1_GENERAL_HEAP].memoryType = NEXUS_MemoryType_eFull;
/*
    pSettings->heap[NEXUS_MEMC2_GRAPHICS_HEAP].memcIndex = 2;
    pSettings->heap[NEXUS_MEMC2_GRAPHICS_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC2_GRAPHICS_HEAP].size = PAGE_ALIGN(512*1024*1024);
    pSettings->heap[NEXUS_MEMC2_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication;
*/
    pSettings->heap[NEXUS_MEMC2_PICTURE_BUFFER_HEAP].memcIndex = 2;
    pSettings->heap[NEXUS_MEMC2_PICTURE_BUFFER_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC2_PICTURE_BUFFER_HEAP].size = PAGE_ALIGN(NEXUS_VIDEO_DECODER_SHVD0_MEMC2_PICTURE_HEAP_SIZE + NEXUS_DISPLAY_MEMC2_HEAP_SIZE); /*(130+104)*1024*1024;*/
    pSettings->heap[NEXUS_MEMC2_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly;

    pSettings->heap[NEXUS_MEMC2_GENERAL_HEAP].memcIndex = 2;
    pSettings->heap[NEXUS_MEMC2_GENERAL_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC2_GENERAL_HEAP].size = PAGE_ALIGN(4*1024*1024);  /* RDC heap plus margin */
    pSettings->heap[NEXUS_MEMC2_GENERAL_HEAP].memoryType = NEXUS_MemoryType_eFull;

    NEXUS_Platform_P_VideoDecoderSettings(&pSettings->videoDecoderModuleSettings);
    NEXUS_Platform_P_VideoEncoderSettings(&pSettings->videoEncoderSettings);
    NEXUS_Platform_P_DisplaySettings(&pSettings->displayModuleSettings);
}

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
	NEXUS_Platform_P_GetDefaultSettings_by_Custom(&platformSettings);

	/* default setting override*/
    platformSettings.heap[NEXUS_MEMC2_GRAPHICS_HEAP].memcIndex = 2;
    platformSettings.heap[NEXUS_MEMC2_GRAPHICS_HEAP].subIndex = 0;
    platformSettings.heap[NEXUS_MEMC2_GRAPHICS_HEAP].size = PAGE_ALIGN(512*1024*1024);
    platformSettings.heap[NEXUS_MEMC2_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication;

#if defined(CONFIG_CRYPTO_NEW_SLOT_NUM)
	PrintError("Over B revision, Allocate slot type 2 for seperate IV keys\n");

    platformSettings.securitySettings.numKeySlotsForType[0] = 60;
    platformSettings.securitySettings.numKeySlotsForType[1] = 0;
	platformSettings.securitySettings.numKeySlotsForType[2] = 30;	/* Allocate slot type 2 for supporting seperate IV keys */
    platformSettings.securitySettings.numKeySlotsForType[3] = 0;
    platformSettings.securitySettings.numKeySlotsForType[4] = 0;
#endif
#if defined(CONFIG_SECURITY_MODULE)
	platformSettings.securitySettings.enableMulti2Key= true;
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
#if defined(CONFIG_DIRECTFB_MASTER)
	for(i = 0; i < DFB_PLATFORM_NUM_DISPLAYS_MAX; i++)
	{
		if(dfbSettings.display[i].handle != NULL)
		{
			DFB_Platform_RemoveDisplayOutputs(dfbSettings.display[i].handle);
		}
	}
#endif
#endif
#if 0
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

#if defined(CONFIG_CRYPTO_NEW_SLOT_NUM)
	PrintError("Over B revision, Allocate slot type 2 for seperate IV keys\n");

    platformSettings.securitySettings.numKeySlotsForType[0] = 60;
    platformSettings.securitySettings.numKeySlotsForType[1] = 0;
	platformSettings.securitySettings.numKeySlotsForType[2] = 30;	/* Allocate slot type 2 for supporting seperate IV keys */
    platformSettings.securitySettings.numKeySlotsForType[3] = 0;
    platformSettings.securitySettings.numKeySlotsForType[4] = 0;
#endif

	platformSettings.securitySettings.enableMulti2Key= true;

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
#endif
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
#if defined(CONFIG_LED_CAPABILITY_MODEL) /* LED_CAPABILITY in di_config.c */
/*
LED_NUM0 : Power LED RED
LED_NUM1 : Power LED BLUE
LED_NUM6 : Data
LED_NUM3 : Mail
LED_NUM4 : Red On
LED_NUM5 : Blue On
*/
HUINT32 LED_CAPABILITY = (DI_LED_NUM0|DI_LED_NUM1|DI_LED_NUM3|DI_LED_NUM4|DI_LED_NUM5|DI_LED_NUM6);
#endif

unsigned char dataSectionData[DI_SYS_OTP_DATA_SECTION_LENGTH] = 
{ 
	0x95,0x34,0x8a,0x0b,0xa9,0xa6,0xac,0x9e,
	0xf5,0x05,0xbf,0x37,0x92,0xaa,0x56,0x88,
	0x01,0xeb,0xf8,0xc0,0x85,0x23,0x9b,0x11,
	0x86,0xcd,0x57,0x1c,0xca,0x7e,0xdb,0xc5
};


#endif
