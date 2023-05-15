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

#if defined(CONFIG_CABLE_MODEM_LED_STATUS)
const HUINT32 g_ulCmLedId = DI_LED_NUM12;
#endif

/* GPIO Configuration */

const GPIO_ConfigInfo_t GPIO_ConfigInfoTable[] =
{
    /* GPIO_ID      GPIO_HANDLER      GPIOTYPE	GPIO_NUMBER      GPIO_MODE      GPIO_INT_MODE      GPIO_INIT_STATE */
 	{GPIO_ID_HW_MUTE,		NULL,   NEXUS_GpioType_eStandard,		33,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,		GPIO_INVAILD},
	{GPIO_ID_SC1_PWR_SEL,	NULL,	NEXUS_GpioType_eAonStandard,	01,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,		GPIO_INVAILD}, /*B-CAS*/
	{GPIO_ID_SC0_PWR_SEL,	NULL,	NEXUS_GpioType_eAonStandard,	00,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,		GPIO_INVAILD}, /*C-CAS*/
	{GPIO_ID_SC0_DIV,		NULL,	NEXUS_GpioType_eStandard,		83,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,		GPIO_INVAILD},    /*C-CAS*/
	{GPIO_ID_SC1_DIV,		NULL,	NEXUS_GpioType_eStandard,		88,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,		GPIO_INVAILD},/*B-CAS*/
	{GPIO_ID_ALIVE,			NULL,	NEXUS_GpioType_eStandard,		53,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,		GPIO_LOW},
	{GPIO_ID_HDD_PWR_CTRL, 	NULL,	NEXUS_GpioType_eStandard,		31, 	GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED, 	GPIO_LOW},
	{GPIO_ID_FAN_SPEED, 	NULL,	NEXUS_GpioType_eStandard,		36, 	GPIO_MODE_INTERRUPT,	GPIO_INT_MODE_RISING_EDGE,	GPIO_INVAILD},
	{GPIO_ID_FAN_CONTROL,	NULL,	NEXUS_GpioType_eStandard,		40,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,		GPIO_HI},
	{GPIO_ID_HW_REV0,		NULL,	NEXUS_GpioType_eAonStandard,	11,		GPIO_MODE_READ,			GPIO_INT_MODE_DISABLED,		GPIO_INVAILD},
	{GPIO_ID_HW_REV1,		NULL,	NEXUS_GpioType_eAonStandard,	12,		GPIO_MODE_READ,			GPIO_INT_MODE_DISABLED,		GPIO_INVAILD},
	{GPIO_ID_nCH0_RST,		NULL,	NEXUS_GpioType_eStandard,		47,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,		GPIO_INVAILD},
	{GPIO_ID_nCH1_RST,		NULL,	NEXUS_GpioType_eStandard,		48,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,		GPIO_INVAILD},
	{GPIO_ID_nCH2_RST,		NULL,	NEXUS_GpioType_eStandard,		49,		GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,		GPIO_INVAILD}
};

//JAMES...MXL683 has TEST MODE which TS outputs pin's state are low when reset is released.
//TS output pins of main chipset MUST be PULL NONE to keep the pin's state to be driven by MXL.
const GPIO_TSConfigInfo_t GPIO_TSConfigInfoTable[] =
{
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_5,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_5_gpio_003_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_5_gpio_003_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_5_gpio_003_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_5,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_5_gpio_004_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_5_gpio_004_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_5_gpio_004_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_5,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_5_gpio_006_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_5_gpio_006_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_5_gpio_006_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_073_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_073_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_073_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_074_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_074_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_074_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_075_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_075_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_075_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_076_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_076_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_076_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_078_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_078_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_078_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_079_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_079_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_079_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_081_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_081_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_081_pad_ctrl_PULL_NONE
	}
};

HUINT32 DRV_CFG_GetGPIOMaxDevice(void)
{
	return (sizeof(GPIO_ConfigInfoTable)/sizeof(GPIO_ConfigInfo_t));
}

HUINT32 DRV_CFG_Get_Max_TS_OUTPUT_Number(void)
{
	return (sizeof(GPIO_TSConfigInfoTable)/sizeof(GPIO_TSConfigInfo_t));
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
	{NEXUS_InputBand_e1, 0},
	{NEXUS_InputBand_e2, 1},
	{NEXUS_InputBand_e3, 2},
	{NEXUS_InputBand_e4, 3},
	{NEXUS_InputBand_e0, 4},
	{NEXUS_InputBand_e6, 5},
	{NEXUS_InputBand_e7, 6},

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
	{4, DI_CH_TYPE_TER},
	{5, DI_CH_TYPE_TER},
	{6, DI_CH_TYPE_TER}

};

const DRV_CH_TSOUTPUTMODE s_TSConfig[] =
{
	{0x1, CH_TS_MODE_MTSIF},	/* CAB */
	{0x1, CH_TS_MODE_MTSIF},	/* CAB */
	{0x1, CH_TS_MODE_MTSIF},	/* CAB */
	{0x1, CH_TS_MODE_MTSIF},	/* CAB */
	{0x1, CH_TS_MODE_SERIAL},	/* TER */
	{0x1, CH_TS_MODE_SERIAL},	/* TER */
	{0x1, CH_TS_MODE_SERIAL}	/* TER */
};


HUINT32 DRV_CFG_GetMaxChannel(void)
{
	return (sizeof(s_ChConfig)/sizeof(DRV_CH_CAPABILITY));
}

const DRV_NETWORK_DEV_INFO_t s_stDevList[] =
{
	{0, "eth0", DI_NETWORK_DEV_LAN},
	{1, "eth1", DI_NETWORK_DEV_LAN},
	{2, "bcm0", DI_NETWORK_DEV_CABLEMODEM},
	{3, "wlan0", DI_NETWORK_DEV_WIFI}
};

HUINT32 DRV_CFG_GetNetMaxDevice(void)
{
	return (sizeof(s_stDevList)/sizeof(DRV_NETWORK_DEV_INFO_t));
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
#define BSPCFG_HEAP_VDC_NUM_FULL_HD_BUFFERS_MEMC0			6
#define BSPCFG_HEAP_VDC_NUM_HD_BUFFERS_MEMC0				1
#define BSPCFG_HEAP_VDC_NUM_SD_BUFFERS_MEMC0				0
#define BSPCFG_HEAP_VDC_NUM_FULL_HD_PIP_BUFFERS_MEMC0			4
#define BSPCFG_HEAP_VDC_NUM_HD_PIP_BUFFERS_MEMC0			0
#define BSPCFG_HEAP_VDC_NUM_SD_PIP_BUFFERS_MEMC0			0
#define BSPCFG_HEAP_VDC_HEAP_SIZE					32*1024*1024	/*Dont support PIP and DVI input ==> Picture Heap*/

#define BSPCFG_HEAP_RAP_HEAP_SIZE					11*1024*1024	/*====>Main Heap*/
#define BSPCFG_HEAP_XPT_HEAP_SIZE					25*1024*1024
#define BSPCFG_HEAP_XPT_ACTUAL_HEAP_SIZE				(BSPCFG_HEAP_XPT_HEAP_SIZE + 9*1024*1024	/*Rs buffer and Xc buffer size over 10M. It seems memoryworks sheedt issue*/)

#define BSPCFG_HEAP_NEXUS_HEAP_SIZE					26*1024*1024

/*GRC*/
#define BSPCFG_HEAP_GRC_FRAMEBUFFER_HEAP_SIZE				20*1024*1024	/*===> Main Heap*/
#define BSPCFG_HEAP_GRC_SURFACE_HEAP_SIZE				30*1024*1024	/*APP surface ===> Graphic Heap*/

#if defined(CONFIG_DEVICE_MODE_KERNEL)
#define BSPCFG_HEAP_MEMC0_MAIN_HEAP					(BSPCFG_HEAP_XVD_GENERAL_HEAP_SIZE + BSPCFG_HEAP_RAP_HEAP_SIZE + BSPCFG_HEAP_XPT_ACTUAL_HEAP_SIZE + BSPCFG_HEAP_GRC_FRAMEBUFFER_HEAP_SIZE + BSPCFG_HEAP_NEXUS_HEAP_SIZE)/*120M*/
#define BSPCFG_HEAP_PICTURE_BUFFER_HEAP					(BSPCFG_HEAP_XVD_PICTURE_HEAP_SIZE + BSPCFG_HEAP_VDC_HEAP_SIZE + BSPCFG_HEAP_GRC_SURFACE_HEAP_SIZE) /*124M*/
#define BSPCFG_HEAP_GRAPHICS_HEAP      					(30*1024*1024) /*30*/
#else
#define BSPCFG_HEAP_MEMC0_MAIN_HEAP					(BSPCFG_HEAP_XVD_GENERAL_HEAP_SIZE + BSPCFG_HEAP_RAP_HEAP_SIZE + BSPCFG_HEAP_XPT_ACTUAL_HEAP_SIZE + BSPCFG_HEAP_GRC_FRAMEBUFFER_HEAP_SIZE + BSPCFG_HEAP_NEXUS_HEAP_SIZE)/*120M*/
#define BSPCFG_HEAP_PICTURE_BUFFER_HEAP					(BSPCFG_HEAP_XVD_PICTURE_HEAP_SIZE + BSPCFG_HEAP_VDC_HEAP_SIZE) /*94M*/
#define BSPCFG_HEAP_GRAPHICS_HEAP      					(BSPCFG_HEAP_GRC_SURFACE_HEAP_SIZE) /*30M */
#endif

#if !defined(CONFIG_SHARED_COMPILE)
#if NEXUS_HAS_VIDEO_DECODER
/* If the user supplied video decoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_VideoDecoderSettings(NEXUS_VideoDecoderModuleSettings *pSettings)
{
    /* By convention the first XVD device is the SVD0 decoder which has its picture
     * buffers on MEMC1 */
    pSettings->heapSize[0].general = NEXUS_VIDEO_DECODER_SVD0_32MEMC0_GENERAL_HEAP_SIZE;
    pSettings->heapSize[0].secure =  NEXUS_VIDEO_DECODER_SVD0_32MEMC1_SECURE_HEAP_SIZE;
    pSettings->heapSize[0].picture = NEXUS_VIDEO_DECODER_SVD0_32MEMC1_PICTURE_HEAP_SIZE;
    pSettings->avdHeapIndex[0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
    pSettings->mfdMapping[0] = 0; /* 1st decoder is mapped to MPEG feeder 0 */
    pSettings->avdMapping[0] = 0; /* 1st decoder is mapped to SVD0 */

    /* The second XVD device is the AVD1 decoder has its picture
     * buffers on MEMC0 */
    pSettings->heapSize[1].general = NEXUS_VIDEO_DECODER_AVD1_32MEMC0_GENERAL_HEAP_SIZE;
    pSettings->heapSize[1].secure =  NEXUS_VIDEO_DECODER_AVD1_32MEMC0_SECURE_HEAP_SIZE;
    pSettings->heapSize[1].picture = NEXUS_VIDEO_DECODER_AVD1_32MEMC0_PICTURE_HEAP_SIZE;
    pSettings->avdHeapIndex[1] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;
    pSettings->mfdMapping[1] = 1; /* 2nd decoder is mapped to MPEG feeder 1 */
    pSettings->avdMapping[1] = 1; /* 2nd decoder is mapped to AVD1 */
    pSettings->mfdMapping[2] = 2; /* 3rd decoder is mapped to MPEG feeder 2 */
    pSettings->avdMapping[2] = 1; /* 3rd decoder is mapped to AVD1 */

    /* set decoder general heap to heap 5 */
    pSettings->hostAccessibleHeapIndex = NEXUS_MEMC0_DRIVER_HEAP;
}
#endif

#if NEXUS_HAS_VIDEO_ENCODER
/* If the user supplied video encoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_VideoEncoderSettings(NEXUS_VideoEncoderModuleSettings *pSettings)
{
    /* According to vce memory spreadsheet, encoder has its picture buffers on MEMC1 */
    pSettings->heapSize[0].general = 0;/* encoder default: no use for current hw */
    pSettings->heapSize[0].secure =  NEXUS_VIDEO_ENCODER_ViCE_32MEMC0_SECURE_HEAP_SIZE;
    pSettings->heapSize[0].picture = NEXUS_VIDEO_ENCODER_ViCE_32MEMC1_PICTURE_HEAP_SIZE;

    pSettings->heapIndex[0].firmware[0] = NEXUS_MEMC1_MAIN_HEAP;
    pSettings->heapIndex[0].firmware[1] = NEXUS_MEMC1_MAIN_HEAP;
    pSettings->heapIndex[0].system  = NEXUS_MEMC1_MAIN_HEAP; /* FW debug log buffer */
    pSettings->heapIndex[0].general = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* same as picture heap for UMA */
    pSettings->heapIndex[0].secure  = NEXUS_MEMC0_DRIVER_HEAP; /* mapped for CDB capture; must be memc0 */
    pSettings->heapIndex[0].picture = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* unmapped */
    pSettings->vceMapping[0].device = 0;
    pSettings->vceMapping[0].channel = 0;
    pSettings->vceMapping[1].device = 0;
    pSettings->vceMapping[1].channel = 1;
    return;
}
#endif

#if NEXUS_HAS_DISPLAY
/* If the user supplied video decoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
#ifdef NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1
#undef NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1         12    /* HD xcode C3V0 with MADR */ /*jmku */
#endif
static void NEXUS_Platform_P_DisplaySettings(NEXUS_DisplayModuleSettings *pSettings)
{
    unsigned i;

    pSettings->primaryDisplayHeapIndex = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;

    for(i=0;i<NEXUS_MAX_HEAPS;i++ ) {
        switch(i)
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
        default:
            pSettings->displayHeapSettings[i].fullHdBuffers.count = 0;
            pSettings->displayHeapSettings[i].hdBuffers.count = 0;
            pSettings->displayHeapSettings[i].sdBuffers.count = 0;
            pSettings->displayHeapSettings[i].fullHdBuffers.pipCount = 0;
            pSettings->displayHeapSettings[i].hdBuffers.pipCount = 0;
            pSettings->displayHeapSettings[i].sdBuffers.pipCount = 0;
            break;
        }
        pSettings->displayHeapSettings[i].fullHdBuffers.format = NEXUS_VideoFormat_e1080p;
        pSettings->displayHeapSettings[i].fullHdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
        pSettings->displayHeapSettings[i].hdBuffers.format = NEXUS_VideoFormat_e1080i;
        pSettings->displayHeapSettings[i].hdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
        pSettings->displayHeapSettings[i].sdBuffers.format = NEXUS_VideoFormat_ePalG;
        pSettings->displayHeapSettings[i].sdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
    }


   /*
    * Based on RTS settings for BCM97425
    */
    pSettings->videoWindowHeapIndex[0][0] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[0][1] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[1][0] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[1][1] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[2][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[2][1] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[3][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
    pSettings->encoderTgMapping[0] = 0;
    pSettings->encoderTgMapping[1] = 1;
}
#endif

static void NEXUS_Platform_P_GetDefaultSettings_by_Custom(NEXUS_PlatformSettings *pSettings)
{
    /* bmem=192M@64M bmem=458M@512M */

    /* heap[0] is nexus default heap, also used for SD2 FB */
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].size = -1;
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD) /*ADD Graphic memory for RTS */
    /* heap[1] Offscreen gfx surfaces & Main FB for displays 0 and 1. 256MB aligned for better V3D purpose. */
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].subIndex = 1;
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = 232 * 1024 * 1024; /* WA-8000 jmku */
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* cached only */
#else
    /* heap[2] Gfx surface for displays 2 and 3. 256MB aligned for better V3D purpose */
    pSettings->heap[NEXUS_MEMC1_GRAPHICS_HEAP].memcIndex = 1;
    pSettings->heap[NEXUS_MEMC1_GRAPHICS_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC1_GRAPHICS_HEAP].size = 256 * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC1_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* cached only */
#endif
    /* heap[3] used for VCE fw and debug buffer */
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].memcIndex = 1;
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].size = 4 * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;

    /* heap[4] MEMC0 device heap for picture buffers (AVD1+VDC).
     * TODO: optimize it to reduce device memory allocation!
     */
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 1;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TWO_TRANSCODER)
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = 152 * 1024 * 1024;
#else
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = 124 * 1024 * 1024;
#endif
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

    /* heap[5] MEMC1 device heap for picture buffers: video decoder, encoder and BVN.
     * TODO: optimize it to reduce device memory allocation!
     */
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].memcIndex = 1;
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].subIndex = 0;
    /* NOTE: we cannot use size = -1 on MEMC1 because the fake address scheme would run up to 0xFFFFFFFF, and that
    exposes logic errors in nexus/magnum. */
#if BCHP_VER >= BCHP_VER_B0 /* dual transcodes */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].size = 200 * 1024 * 1024; /* WA-8000 jmku */
#else
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].size = 174 * 1024 * 1024;
#endif
#else
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].size = 136 * 1024 * 1024;
#endif
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

    /* heap[6] used for XVD fw/general heap and VCE secure heap.
     * TODO: separate XVD fw heap from non-CPU accessible heap to optimize mappable size;
     * TODO: security support for VCE secure heap;
     */
    pSettings->heap[NEXUS_MEMC0_DRIVER_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_DRIVER_HEAP].subIndex = 1;
    pSettings->heap[NEXUS_MEMC0_DRIVER_HEAP].size = 80 * 1024 * 1024;		/* WA-8000 jmku */
    pSettings->heap[NEXUS_MEMC0_DRIVER_HEAP].memoryType = NEXUS_MemoryType_eFull;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_GRAPHICS3D)
	/* if size of mem0_driver_heap have to be modified, bmem size in kernel_config have to be modified also. */
	pSettings->heap[NEXUS_MEMC0_DRIVER_HEAP].size = (256 - CONFIG_RAM_ROOTFS_BMEM_SIZE) * 1024 * 1024;
#endif
#if NEXUS_HAS_VIDEO_DECODER
    NEXUS_Platform_P_VideoDecoderSettings(&pSettings->videoDecoderModuleSettings);
#endif
#if NEXUS_HAS_VIDEO_ENCODER
    NEXUS_Platform_P_VideoEncoderSettings(&pSettings->videoEncoderSettings);
#endif
#if NEXUS_HAS_DISPLAY
    NEXUS_Platform_P_DisplaySettings(&pSettings->displayModuleSettings);
#endif
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
	platformSettings.heap[NEXUS_MEMC1_GRAPHICS_HEAP].memcIndex = NULL;
	platformSettings.heap[NEXUS_MEMC1_GRAPHICS_HEAP].subIndex = NULL;
	platformSettings.heap[NEXUS_MEMC1_GRAPHICS_HEAP].size = NULL;
	platformSettings.heap[NEXUS_MEMC1_GRAPHICS_HEAP].memoryType = NULL; /* cached only */

#if defined(CONFIG_DIRECTFB)
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
	dfbSettings.display[0].initSettings.format = DFB_PlatformVideoFormat_e1080i;
	dfbSettings.display[1].initSettings.format = DFB_PlatformVideoFormat_eNtscJapan;
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
#if 0
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

	platformSettings.heap[NEXUS_MEMC1_GRAPHICS_HEAP].memcIndex = 0;
	platformSettings.heap[NEXUS_MEMC1_GRAPHICS_HEAP].subIndex = 1;
	platformSettings.heap[NEXUS_MEMC1_GRAPHICS_HEAP].size = BSPCFG_HEAP_GRAPHICS_HEAP;
	platformSettings.heap[NEXUS_MEMC1_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* cached only */

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
#endif /* End of CONFIG_DIRECTFB */
	return DRV_OK;
}
#endif

#if defined(CONFIG_JAPAN_SPEC)
/************************************************************************************
	FAN
*************************************************************************************/
/* FAN Execution Temperature */
const HUINT8 s_FanExecuteTemp[7] =
{
	50,		/* INITIAL_THRESHOLD */
	48,		/* LOW_THRESHOLD */
	48,		/* LOW_MIDDLE_THRESHOLD */
	48,		/* MIDDLE_THRESHOLD */
	48,		/* MIDDLE_HIGH_THRESHOLD */
	65,		/* HIGH_THRESHOLD */
	65		/* SHUTDOWN_THRESHOLD */
};

/* FAN Execution Temperature */
const HUINT8 s_FanExecuteCPUTemp[7] =
{
	90,	/* INITIAL_THRESHOLD */
	80,	/* LOW_THRESHOLD */
	80,	/* LOW_MIDDLE_THRESHOLD */
	80,	/* MIDDLE_THRESHOLD */
	80,	/* MIDDLE_HIGH_THRESHOLD */
	115,	/* HIGH_THRESHOLD */
	115		/* SHUTDOWN_THRESHOLD */
};

/* FAN Execution Temperature */
const HUINT8 s_FanExecuteCMTemp[7] =
{
	90,	/* INITIAL_THRESHOLD */
	80,	/* LOW_THRESHOLD */
	80,	/* LOW_MIDDLE_THRESHOLD */
	80,	/* MIDDLE_THRESHOLD */
	80,	/* MIDDLE_HIGH_THRESHOLD */
	115,	/* HIGH_THRESHOLD */
	115		/* SHUTDOWN_THRESHOLD */
};

const HUINT32 s_FanFreq = 0x0270;


void DRV_CFG_SetTSlineConfiguration(void)
{
	unsigned int nIdx;
	unsigned int nMaxCnt;
	unsigned int reg=0;

	nMaxCnt = DRV_CFG_Get_Max_TS_OUTPUT_Number();

	for(nIdx=0; nIdx<nMaxCnt; nIdx++)
	{
		NEXUS_Platform_Registers_Read_Access(GPIO_TSConfigInfoTable[nIdx].addr, &reg);
		reg &= ~(GPIO_TSConfigInfoTable[nIdx].mask);
		reg |= ( GPIO_TSConfigInfoTable[nIdx].value << (GPIO_TSConfigInfoTable[nIdx].shift));
		NEXUS_Platform_Registers_Write_Access(GPIO_TSConfigInfoTable[nIdx].addr,reg);
	}
}


HUINT32 DRV_CFG_GetFanInterval(HUINT32 ulVolt)
{
	HUINT32 usOnInterval;
	if( ulVolt >= DRV_FAN_CTRL_HIGH )
	{
		usOnInterval = 0;
	}
	else if( ulVolt >= DRV_FAN_CTRL_MIDDLE_HIGH )
	{
		usOnInterval = 0;
	}
	else if( ulVolt >= DRV_FAN_CTRL_MIDDLE )
	{
		usOnInterval = 0;
	}
	else if( ulVolt >= DRV_FAN_CTRL_LOW_MIDDLE )
	{
		usOnInterval = 0;
	}
	else if( ulVolt >= DRV_FAN_CTRL_LOW )
	{
		usOnInterval = 0;
	}
	else
	{
		usOnInterval = 255;
	}
	return usOnInterval;
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
	48,		/* LOW_MIDDLE_THRESHOLD */
	51,		/* MIDDLE_THRESHOLD */
	54,		/* MIDDLE_HIGH_THRESHOLD */
	57,		/* HIGH_THRESHOLD */
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
#if defined(CONFIG_LED_CAPABILITY_MODEL) /* LED_CAPABILITY in di_config.c */
/*
LED_NUM0 : Power LED ON(Red) / OFF(Green)
LED_NUM1 : HDD
LED_NUM2 : s1
LED_NUM3 : s10
LED_NUM4 : s2 = Play (High)
LED_NUM5 : s3 = Pause (High)
LED_NUM6 : LAN
LED_NUM7 : USB
LED_NUM8 : s4
LED_NUM9 : s11
LED_NUM10 : s5 = Play(Mid)
LED_NUM11 : s6 = Pause(Mid)
LED_NUM12 : Modem
LED_NUM13 : DLNA
LED_NUM14 : s7
LED_NUM15 : s12
LED_NUM16 : s8 = Play(Low)
LED_NUM17 : s9 = Pause(Low)
LED_NUM18 : VIDEO
LED_NUM19 : Terrestrial
LED_NUM20 : BS
LED_NUM21 : CATV
LED_NUM22 : ???
LED_NUM23 : Mail
*/
HUINT32 LED_CAPABILITY = (DI_LED_NUM0|DI_LED_NUM1|DI_LED_NUM2|DI_LED_NUM3|DI_LED_NUM4|DI_LED_NUM5|DI_LED_NUM6|DI_LED_NUM7|DI_LED_NUM8|DI_LED_NUM9
|DI_LED_NUM10|DI_LED_NUM11|DI_LED_NUM12|DI_LED_NUM13|DI_LED_NUM14|DI_LED_NUM15|DI_LED_NUM16|DI_LED_NUM17|DI_LED_NUM18|DI_LED_NUM19
|DI_LED_NUM20|DI_LED_NUM21|DI_LED_NUM22|DI_LED_NUM23);
#endif

unsigned char dataSectionData[DI_SYS_OTP_DATA_SECTION_LENGTH] =
{
	0x95,0x34,0x8a,0x0b,0xa9,0xa6,0xac,0x9e,
	0xf5,0x05,0xbf,0x37,0x92,0xaa,0x56,0x88,
	0x01,0xeb,0xf8,0xc0,0x85,0x23,0x9b,0x11,
	0x86,0xcd,0x57,0x1c,0xca,0x7e,0xdb,0xc5
};


#endif