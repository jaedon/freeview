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

#if defined(CONFIG_BMEM_OPTIMIZE)
#include "di_config.h"
#endif

/* GPIO Configuration */

#define SIZE_ALIGNED(size, align)	((((size)+((1<<align)-1)) >> align) << align)

const GPIO_ConfigInfo_t GPIO_ConfigInfoTable[] =
{
    /* GPIO_ID      		GPIO_HANDLER    GPIOTYPE						GPIO_NUMBER      GPIO_MODE      	GPIO_INT_MODE      			GPIO_INIT_STATE */
    {GPIO_ID_HW_MUTE,    	NULL,   NEXUS_GpioType_eStandard, 		65,   GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_INVAILD},

    {GPIO_ID_nCI_CD1,    	NULL,  	NEXUS_GpioType_eAonStandard, 	5,    GPIO_MODE_INTERRUPT,	GPIO_INT_MODE_BOTH_EDGES,		GPIO_INVAILD},
    {GPIO_ID_nCI_CD2,    	NULL,  	NEXUS_GpioType_eAonStandard, 	6,    GPIO_MODE_READ,    	GPIO_INT_MODE_DISABLED,  		GPIO_INVAILD},
    {GPIO_ID_CI_RST,    	NULL,  	NEXUS_GpioType_eAonStandard,	4,    GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,  		GPIO_LOW},
    {GPIO_ID_CI_PWR,    	NULL,   NEXUS_GpioType_eAonStandard, 	1,    GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
    {GPIO_ID_nCI_OUTEN,    	NULL,   NEXUS_GpioType_eStandard,		28,   GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_HI},
    {GPIO_ID_FAN_SPEED,    	NULL,   NEXUS_GpioType_eStandard, 		27,   GPIO_MODE_INTERRUPT,	GPIO_INT_MODE_FALLING_EDGE,		GPIO_INVAILD},
    {GPIO_ID_NVRAM_WP,    	NULL,	NEXUS_GpioType_eStandard, 		16,   GPIO_MODE_WRITE,		GPIO_INT_MODE_DISABLED,    		GPIO_HI},
    {GPIO_ID_HDD_PWR_CTRL,	NULL,   NEXUS_GpioType_eStandard, 		61,   GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_LOW},

    {GPIO_ID_HW_REV0,    	NULL,  	NEXUS_GpioType_eAonStandard, 	7,    GPIO_MODE_READ,    	GPIO_INT_MODE_DISABLED,    		GPIO_INVAILD},
    {GPIO_ID_HW_REV1,    	NULL,  	NEXUS_GpioType_eAonStandard, 	8,    GPIO_MODE_READ,   	GPIO_INT_MODE_DISABLED,   		GPIO_INVAILD},
    {GPIO_ID_HW_REV2,    	NULL,  	NEXUS_GpioType_eAonStandard, 	9,    GPIO_MODE_READ,    	GPIO_INT_MODE_DISABLED,    		GPIO_INVAILD},

    {GPIO_ID_nLNBP0_RST,    NULL,  	NEXUS_GpioType_eStandard, 		2,    GPIO_MODE_WRITE,    	GPIO_INT_MODE_DISABLED,    		GPIO_LOW},
    {GPIO_ID_LOOPTHROUGH,        NULL,   NEXUS_GpioType_eStandard,   32,     GPIO_MODE_WRITE,         GPIO_INT_MODE_DISABLED, GPIO_HI}
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
		DI_KEY_FRONT_CH_UP,				/* Micom KeyID 2 */
		DI_KEY_FRONT_CH_DOWN,			/* Micom KeyID 3 */
		DI_KEY_FRONT_VOL_UP,			/* Micom KeyID 4 */
		DI_KEY_FRONT_VOL_DOWN, 			/* Micom KeyID 5 */
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
		{NEXUS_InputBand_e8, 0},
		{NEXUS_InputBand_e9, 1},
};

const DRV_CI_REMUX_PATH_ID s_RemuxIds[] =
{
		{NEXUS_InputBand_e10},
		{NEXUS_InputBand_e10},
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
};

const DRV_CH_TSOUTPUTMODE s_TSConfig[] =
{
	{0x1, CH_TS_MODE_PARALLEL},

	{0x1, CH_TS_MODE_PARALLEL},
};

#if defined(CONFIG_SUPPORT_SELECTABLE_RF)
const DI_CH_TunerCapacity_t s_SatTunerCapacity[NUM_OF_CHANNEL] =
{
	/* Tuner 0*/
	{
		{DI_CH_RF_INPUT1,DI_CH_RF_INPUT_NONE,DI_CH_RF_INPUT_NONE,DI_CH_RF_INPUT_NONE},
		1,
		FALSE
	},

	/* Tuner 1*/
	{
		{DI_CH_RF_INPUT1,DI_CH_RF_INPUT2,DI_CH_RF_INPUT_NONE,DI_CH_RF_INPUT_NONE},
		2,
		FALSE
	}
};
#endif

HUINT32 DRV_CFG_GetMaxChannel(void)
{
	return (sizeof(s_ChConfig)/sizeof(DRV_CH_CAPABILITY));
}

#define DEBUG_MODULE	TRACE_MODULE_DI_INIT

enum BSPCFG_P_HeapType
{
	BSPCFG_P_HeapType_General = 0,
	BSPCFG_P_HeapType_Surface = 1,
	BSPCFG_P_HeapType_Xvd_General = 2,
	BSPCFG_P_HeapType_Xvd_Picture = 3,
	BSPCFG_P_HeapType_Ragga = 4,
	BSPCFG_P_HeapType_Vdc = 5,
	BSPCFG_P_HeapType_Xpt = 6,
	BSPCFG_P_HeapType_Secure = 7,
	BSPCFG_P_HeapType_Last = 8
}BSPCFG_P_HeapType;

typedef struct BSPCFG_P_HeapInfo
{
	uint32_t		memcIndex;
	uint32_t		heapIndex;
	uint32_t		uSubIndex;	
	uint32_t		heapType;	
}BSPCFG_P_HeapInfo;

//#define HEAP_SEPERATE

static const BSPCFG_P_HeapInfo gHeapInfo[BSPCFG_P_HeapType_Last] = 
{
#if defined(HEAP_SEPERATE)
	{0, 0, 0, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_General = 0,
	{0, 1, 1, NEXUS_MemoryType_eApplication},	// BSPCFG_P_HeapType_Surface = 1,
	{0, 2, 1, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_Xvd_General = 2,
	{0, 3, 0, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_Xvd_Picture = 3,
	{0, 4, 1, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_Ragga = 4,
	{0, 5, 0, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_Vdc = 5,
	{0, 6, 0, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_Xpt = 6,
	{0, 7, 0, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_Secure = 7,
#else
	{0, NEXUS_MEMC0_MAIN_HEAP, 0, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_General = 0,
	{0, NEXUS_MEMC0_GRAPHICS_HEAP, 1, NEXUS_MemoryType_eApplication},	// BSPCFG_P_HeapType_Surface = 1,
	{0, NEXUS_MEMC0_MAIN_HEAP, 0, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_Xvd_General = 2,
	{0, NEXUS_MEMC0_MAIN_HEAP, 0, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_Xvd_Picture = 3,
	{0, NEXUS_MEMC0_MAIN_HEAP, 0, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_Ragga = 4,
	{0, NEXUS_MEMC0_MAIN_HEAP, 0, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_Vdc = 5,
	{0, NEXUS_MEMC0_MAIN_HEAP, 0, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_Xpt = 6,
	{0, NEXUS_MEMC0_MAIN_HEAP, 0, NEXUS_MemoryType_eFull},	// BSPCFG_P_HeapType_Secure = 7,
#endif
};

#define BSPCFG_P_DEMUX_IB_NUMBER					CONFIG_NUM_INPUT_BANDS

#define BSPCFG_P_DEMUX_XC_IB_BUFFER_NUMBER			16
	/*PB*/
#define BSPCFG_P_DEMUX_PB_NUMBER					(NUM_LIVE+NUM_PIP+NUM_TSR+NUM_REC+NUM_TRANSCODING+NUM_REMUX)

#define BSPCFG_P_DEMUX_XC_PB_BUFFER_NUMBER			14

/*Section*/
#define BSPCFG_P_PSI_BUFFER_SIZE					16*1024
#define BSPCFG_P_PES_BUFFER_SIZE					64*1024

/*Video*/
/*Common*/
/*decoder*/
#define BSPCFG_P_VIDEO_DECODE_CDB_SIZE					3*512*1024			/*1.5M*/
#define BSPCFG_P_VIDEO_DECODE_ITB_SIZE					256*1024			/*0.25M*/
/*still*/
#define BSPCFG_P_VDIDE_STILL_NUMBER_PER_DECODER 		1
#define BSPCFG_P_VIDEO_STILL_DECODE_CDB_SIZE			(6*1024*1024)/10	/*0.6M*/
/*mosaic*/
#define BSPCFG_P_VIDEO_MOSAIC_DECODE_NUMBER				0

/*decoder*/
#define BSPCFG_P_VIDEO_DECODE_NUMBER					(NUM_LIVE+NUM_PIP)

/*Audio*/
/*Common*/
#define BSPCFG_P_AUDIO_DECODE_CDB_SIZE					256*1024

#define BSPCFG_P_AUDIO_DECODE_NUMBER				3
#define BSPCFG_P_AUDIO_PRIMER_NUMBER					16
#define BSPCFG_P_AUDIO_MAX_TASKS_NUMBER				(BSPCFG_P_AUDIO_DECODE_NUMBER+1)*((NUM_TRANSCODING != 0) + 1)
#define BSPCFG_P_AUDIO_LOUDNESS_MODE				NEXUS_AudioLoudnessEquivalenceMode_eEbuR128


/*File*/
/*Playback*/
#define BSPCFG_P_FILE_PLAYBACK_NUMBER					BSPCFG_P_DEMUX_PB_NUMBER		
#define BSPCFG_P_FILE_PLAYBACK_FIFO_SIZE				((188/4)*4096)*8
/*Record*/
#define BSPCFG_P_FILE_RECORD_CDB_SIZE					((188/4)*4096)*8
#define BSPCFG_P_FILE_RECORD_ITB_SIZE					6*4*16*48
#define BSPCFG_P_FILE_RECORD_TSR_NUMBER 				(NUM_TSR)
#define BSPCFG_P_FILE_RECORD_ALLTS_NUMBER				(0)
#define BSPCFG_P_FILE_RECORD_PARTIAL_NUMBER 			(NUM_REC)

#define BSPCFG_P_FILE_RECORD_NUMBER						(BSPCFG_P_FILE_RECORD_TSR_NUMBER + BSPCFG_P_FILE_RECORD_ALLTS_NUMBER +\
														BSPCFG_P_FILE_RECORD_PARTIAL_NUMBER)


/*Osd*/
/*Common*/
#define BSPCFG_P_OSD_GFX_HEAP_SIZE					(20*1024*1024)	

/*Video Encoder*/
#define BSPCFG_P_ENCODE_NUMBER						NUM_TRANSCODING

/*Steam mux*/
#define BSPCFG_P_ENCODE_PLAYPUMP_FIFO_SIZE			16384
#define BSPCFG_P_ENCODE_PLAYPUMP_START_INDEX		BSPCFG_P_FILE_PLAYBACK_NUMBER
#define BSPCFG_P_ENCODE_PLAYPUMP_NUMBER				0
#define BSPCFG_P_ENCODE_RECORD_CDB_SIZE				BSPCFG_P_FILE_RECORD_CDB_SIZE
#define BSPCFG_P_ENCODE_RECORD_ITB_SIZE				BSPCFG_P_FILE_RECORD_ITB_SIZE
#define BSPCFG_P_ENCODE_RECORD_START_INDEX			BSPCFG_P_FILE_RECORD_NUMBER
#define BSPCFG_P_ENCODE_RECORD_NUMBER				BSPCFG_P_ENCODE_NUMBER

#define BSPCFG_P_ENCODE_PB_BITRATES 				25*1000*1000 /*Encode*/
#define BSPCFG_P_ENCODE_PB_RAVE_SUPPORTED			1
#define BSPCFG_P_ENCODE_PB_MSG_SUPPORTED			0
#define BSPCFG_P_ENCODE_PB_RMX0_SUPPORTED			0
#define BSPCFG_P_ENCODE_PB_RMX1_SUPPORTED			0


/***********************************************************************************************************************
	Get Platform Settings.
************************************************************************************************************************/

#define BSPCFG_P_HEAP_XVD_SECURE_SIZE				0

/*2 HD + SD Simultaneously. HD MAD. Sync lock*/	
#define BSPCFG_P_HEAP_VDC_FULL_HD_BUFFER_NUM		7
#define BSPCFG_P_HEAP_VDC_HD_BUFFER_NUM				1
#define BSPCFG_P_HEAP_VDC_SD_BUFFER_NUM				0
#define BSPCFG_P_HEAP_VDC_FULL_HD_PIP_BUFFER_NUM	4
#define BSPCFG_P_HEAP_VDC_HD_PIP_BUFFER_NUM			0
#define BSPCFG_P_HEAP_VDC_SD_PIP_BUFFER_NUM			0

#define BSPCFG_P_HEAP_RAGGA_SIZE					35*1024*1024 /*35M*/	// Kevin : Fixed


#define BSPCFG_P_PLAYPUMP_BUFF		((38*1024*1024/10)*CONFIG_NUM_PLAYPUMPS)

static void sBSPCFG_SetHeapSettings(NEXUS_PlatformSettings *platformSettings)
{
	HUINT32	heapInfoIndex;

	/*Set all zero of heap params*/
	BKNI_Memset(platformSettings->heap, 0x0, sizeof(platformSettings->heap));
	
	/****************************************************************************************************************
	General Heap: Framebuffer. Misc

	*Framebufer*
	HdFrameBuffer:			1920*1080*4 = 8M 
	SdFrameBuffer:			720*576*4 = 1.58

	MISC HEAP: 8M
	Hsm, Drc, grc packet, vbi encoder

	1Mbytes align
	General Heap = 9.58M + 15M = 24.58M => 25M
	****************************************************************************************************************/
	{
#define HD_FRAMEBUFFER 1920*1080*4
#define SD_FRAMEBUFFER 720*576*4

#define VBI_TT_BUFFER (110*1024)	// VBI buffer size
#if defined(CONFIG_DIRECTFB)
#define DFB_SURFACE_BUFFER	(3686408*2+1658888)
#else
#define DFB_SURFACE_BUFFER	0
#endif
#if defined(CONFIG_DIRECTFB_MASTER)
#define MISC_HEAP_SIZE ((CONFIG_NUM_PLAYPUMPS*16*1024*1024/10) + VBI_TT_BUFFER + DFB_SURFACE_BUFFER + BSPCFG_P_PLAYPUMP_BUFF)		// Kevin
#else
#define MISC_HEAP_SIZE ((CONFIG_NUM_PLAYPUMPS*16*1024*1024/10) + VBI_TT_BUFFER + DFB_SURFACE_BUFFER + 0                     )		// Kevin
#endif

		heapInfoIndex = BSPCFG_P_HeapType_General;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memcIndex = gHeapInfo[heapInfoIndex].memcIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].subIndex = gHeapInfo[heapInfoIndex].uSubIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memoryType = gHeapInfo[heapInfoIndex].heapType;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].size += SIZE_ALIGNED((HD_FRAMEBUFFER + SD_FRAMEBUFFER) + MISC_HEAP_SIZE, 20);
	}

	/****************************************************************************************************************
	VDC Heap: 

	FULLHD_SIZE:		1920*1080*2
	FULLHD_PIP_SIZE:	FULLHD_SIZE/4
	HD_SIZE:			(1920*1080*2)/2
	HD_PIP_SIZE:		FULLHD_SIZE/4
	SD_SIZE:			(720*576*2)/2
	SD_PIP_SIZE:		SD_SIZE/4

	Capture_Surface: 1HD = 2M

	1Mbytes Algine:

	****************************************************************************************************************/
	heapInfoIndex = BSPCFG_P_HeapType_Vdc;
	platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memcIndex = gHeapInfo[heapInfoIndex].memcIndex;
	platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].subIndex = gHeapInfo[heapInfoIndex].uSubIndex;
	/*Vdc Heap calcuation*/
	{
#define FULLHD_SIZE 1920*1080*2
#define FULLHD_PIP_SIZE FULLHD_SIZE/4
#define HD_SIZE (1920*1080*2)/2
#define HD_PIP_SIZE HD_SIZE/4
#define SD_SIZE (720*576*2)/2
#define SD_PIP_SIZE SD_SIZE/4
#define CAPTURE_SURFACE HD_SIZE

		NEXUS_DisplayModuleSettings *pDisplaySettings = &(platformSettings->displayModuleSettings);

		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].size += SIZE_ALIGNED((
			(pDisplaySettings->displayHeapSettings[gHeapInfo[heapInfoIndex].heapIndex].fullHdBuffers.count*FULLHD_SIZE +
			pDisplaySettings->displayHeapSettings[gHeapInfo[heapInfoIndex].heapIndex].fullHdBuffers.pipCount*FULLHD_PIP_SIZE +
			pDisplaySettings->displayHeapSettings[gHeapInfo[heapInfoIndex].heapIndex].hdBuffers.count*HD_SIZE +
			pDisplaySettings->displayHeapSettings[gHeapInfo[heapInfoIndex].heapIndex].hdBuffers.pipCount*HD_PIP_SIZE +
			pDisplaySettings->displayHeapSettings[gHeapInfo[heapInfoIndex].heapIndex].sdBuffers.count*SD_SIZE +
			pDisplaySettings->displayHeapSettings[gHeapInfo[heapInfoIndex].heapIndex].sdBuffers.pipCount*SD_PIP_SIZE + CAPTURE_SURFACE)), 20);
	}
	platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memoryType = gHeapInfo[heapInfoIndex].heapType;

/****************************************************************************************************************
XVD general Heap: 

FW size: BSPCFG_P_HEAP_XVD_FW_SIZE
Xvd_General Heap size: BSPCFG_P_HEAP_XVD_GENERAL_SIZE

1Mbytes Algine:
****************************************************************************************************************/
	{
#define BSPCFG_P_HEAP_XVD_FW_SIZE					2*1024*1024 /*2M*/
#define BSPCFG_P_HEAP_XVD_GENERAL_SIZE				(10*1024*1024 + SUPPORT_VIDEO_CODEC_SVC*15*1024*1024)					/*25M*2*/	

#define XVD_GENERAL_HEAP_SIZE (BSPCFG_P_HEAP_XVD_FW_SIZE + BSPCFG_P_HEAP_XVD_GENERAL_SIZE)

		heapInfoIndex = BSPCFG_P_HeapType_Xvd_General;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memcIndex = gHeapInfo[heapInfoIndex].memcIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].subIndex = gHeapInfo[heapInfoIndex].uSubIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memoryType = gHeapInfo[heapInfoIndex].heapType;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].size += SIZE_ALIGNED(XVD_GENERAL_HEAP_SIZE, 20);
	}

/****************************************************************************************************************
XVD Picturel Heap: 

BSPCFG_P_HEAP_XVD_PICTURE_SIZE:  BSPCFG_P_HEAP_XVD_PICTURE_SIZE


1Mbytes Algine:

****************************************************************************************************************/
#define BSPCFG_P_HEAP_XVD_PICTURE_SIZE				(25*1024*1024 + SUPPORT_VIDEO_CODEC_SVC*35*1024*1024)
#define BSPCFG_P_HEAP_XVD_PICTURE_HEAP_SIZE			1*1024*1024 /*1M*2*/
	{
		heapInfoIndex = BSPCFG_P_HeapType_Xvd_Picture;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memcIndex = gHeapInfo[heapInfoIndex].memcIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].subIndex = gHeapInfo[heapInfoIndex].uSubIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memoryType = gHeapInfo[heapInfoIndex].heapType;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].size += SIZE_ALIGNED(BSPCFG_P_HEAP_XVD_PICTURE_SIZE+BSPCFG_P_HEAP_XVD_PICTURE_HEAP_SIZE, 20);
	}

/****************************************************************************************************************
Ragga Heap:

BSPCFG_P_HEAP_RAGGA_SIZE:

1Mbytes Algine:

****************************************************************************************************************/
	{
		heapInfoIndex = BSPCFG_P_HeapType_Ragga;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memcIndex = gHeapInfo[heapInfoIndex].memcIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].subIndex = gHeapInfo[heapInfoIndex].uSubIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memoryType = gHeapInfo[heapInfoIndex].heapType;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].size += SIZE_ALIGNED(BSPCFG_P_HEAP_RAGGA_SIZE, 20);
	}

/****************************************************************************************************************
Gfx Heap and(Vram Heap): Vram, Backbuffer, StillPlane, StillSurface. This heap is separated from general heap.

Dest_Resize_Surface(Driver):			1920*1080*4(Max size)  = 8M
Src_Surface(Driver):					1920*1080*4(Max size)  = 8M
Dest_Surface(Driver):					1920*1080*4(Max size)  = 8M

Allocated Surface(App): 

1Mbytes align
Total Gfx heap:

****************************************************************************************************************/
#if 0
#define DEST_RESIZE_SURFACE_SIZE 1920*1080*4
#define SRC_SURFACE_SIZE 1280*720*4
#define DEST_SURFACE_SIZE 1280*720*4

	{
		heapInfoIndex = BSPCFG_P_HeapType_Surface;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memcIndex = gHeapInfo[heapInfoIndex].memcIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].subIndex = gHeapInfo[heapInfoIndex].uSubIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memoryType = gHeapInfo[heapInfoIndex].heapType;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].size += SIZE_ALIGNED((DEST_RESIZE_SURFACE_SIZE + SRC_SURFACE_SIZE + DEST_SURFACE_SIZE + BSPCFG_P_OSD_GFX_HEAP_SIZE), 20);
	}
#else
#define SURFACE_SIZE 92*1024*1024
	{
		heapInfoIndex = BSPCFG_P_HeapType_Surface;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memcIndex = gHeapInfo[heapInfoIndex].memcIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].subIndex = gHeapInfo[heapInfoIndex].uSubIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memoryType = gHeapInfo[heapInfoIndex].heapType;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].size += SIZE_ALIGNED((SURFACE_SIZE), 20);
	}
#endif
	
/****************************************************************************************************************
XPT Heap: 

XPT_HEAP_SIZE: 
RS_BUFFER:
XC_BUFFER:
PSI filter size and PES filter size:

Playback fifo size: 
Record fifo size:

Decoder CDB size:
Decoder ITB size:
Still CDB size:
Audio CDB size:

1Mbytes Algine:
****************************************************************************************************************/
#define XPT_RS_BUFFER_SIZE				200*1024
#define XPT_XC_IB_BUFFER_SIZE			200*1024
#define XPT_XC_PB_BUFFER_SIZE			8*1024
#define XPT_XC_RS_BUFFER_SIZE 			(XPT_RS_BUFFER_SIZE*BSPCFG_P_DEMUX_IB_NUMBER +\
										XPT_XC_IB_BUFFER_SIZE*BSPCFG_P_DEMUX_XC_IB_BUFFER_NUMBER +\
										XPT_XC_PB_BUFFER_SIZE*BSPCFG_P_DEMUX_XC_PB_BUFFER_NUMBER)

/***********************************************************************************************************************
	Section************************************************************************************************************************/
#define AKYO_SECTION_NUMBER 				128	/*Total section message number*/
#define AKYO_SECTION_PES_NUMBER					5	/*Pes section message number*/
#define AKYO_SECTION_PSI_NUMBER 					(AKYO_SECTION_NUMBER - AKYO_SECTION_PES_NUMBER) /*Psi message number*/
#define AKYO_SECTION_DATA_BUFFER_NUMBER			512 /*Section data buffer number. Data buffer will be mallaced at every section data arrival*/

										
#define	XPT_PSI_PES_BUFFER_SIZE			(BSPCFG_P_PSI_BUFFER_SIZE*AKYO_SECTION_PSI_NUMBER +\
										BSPCFG_P_PES_BUFFER_SIZE*AKYO_SECTION_PES_NUMBER)	/* Kevin : drv_demux.h 와 연계하여 확인 필요. */
#define XPT_PLAYBACK_FIFO_SIZE			(BSPCFG_P_FILE_PLAYBACK_NUMBER*BSPCFG_P_FILE_PLAYBACK_FIFO_SIZE+\
										BSPCFG_P_ENCODE_PLAYPUMP_NUMBER*BSPCFG_P_ENCODE_PLAYPUMP_FIFO_SIZE)

#define XPT_RECORD_CDB_ITB_SIZE 		((BSPCFG_P_FILE_RECORD_NUMBER*(BSPCFG_P_FILE_RECORD_CDB_SIZE + BSPCFG_P_FILE_RECORD_ITB_SIZE))+\
										(BSPCFG_P_ENCODE_RECORD_NUMBER*(BSPCFG_P_ENCODE_RECORD_CDB_SIZE + BSPCFG_P_ENCODE_RECORD_ITB_SIZE)))
	
#define XPT_VIDEO_CDB_ITB_SIZE			BSPCFG_P_VIDEO_DECODE_NUMBER*(BSPCFG_P_VIDEO_DECODE_CDB_SIZE +\
										BSPCFG_P_VIDEO_DECODE_ITB_SIZE)
#define XPT_STILL_CDB_ITB_SIZE			BSPCFG_P_VDIDE_STILL_NUMBER_PER_DECODER*\
										(BSPCFG_P_VIDEO_STILL_DECODE_CDB_SIZE + (BSPCFG_P_VIDEO_STILL_DECODE_CDB_SIZE/10))
#define XPT_AUDIO_CDB_ITB_SIZE			(BSPCFG_P_AUDIO_DECODE_CDB_SIZE*(BSPCFG_P_AUDIO_DECODE_NUMBER + BSPCFG_P_AUDIO_PRIMER_NUMBER) +\
										(BSPCFG_P_AUDIO_DECODE_CDB_SIZE*(BSPCFG_P_AUDIO_DECODE_NUMBER + BSPCFG_P_AUDIO_PRIMER_NUMBER))/2)

#define XPT_UNKNOWN_HEAP_SIZE			10*1024*1024
	
#if defined(CONFIG_DIRECTFB_MASTER)
#define XPT_HEAP_SIZE 					(XPT_XC_RS_BUFFER_SIZE + XPT_PSI_PES_BUFFER_SIZE +\
										0                      + XPT_RECORD_CDB_ITB_SIZE +\
										XPT_VIDEO_CDB_ITB_SIZE + XPT_STILL_CDB_ITB_SIZE + XPT_AUDIO_CDB_ITB_SIZE) +\
										XPT_UNKNOWN_HEAP_SIZE
#else
#define XPT_HEAP_SIZE 					(XPT_XC_RS_BUFFER_SIZE + XPT_PSI_PES_BUFFER_SIZE +\
										BSPCFG_P_PLAYPUMP_BUFF + XPT_RECORD_CDB_ITB_SIZE +\
										XPT_VIDEO_CDB_ITB_SIZE + XPT_STILL_CDB_ITB_SIZE + XPT_AUDIO_CDB_ITB_SIZE) +\
										XPT_UNKNOWN_HEAP_SIZE
#endif
	
	heapInfoIndex = BSPCFG_P_HeapType_Xpt;
	platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memcIndex = gHeapInfo[heapInfoIndex].memcIndex;
	platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].subIndex = gHeapInfo[heapInfoIndex].uSubIndex;
	platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memoryType = gHeapInfo[heapInfoIndex].heapType;
	platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].size += SIZE_ALIGNED(XPT_HEAP_SIZE, 20);
		
/****************************************************************************************************************
Secure Heap: 

1Mbytes Algine:

****************************************************************************************************************/
#define SECURE_HEAP_SIZE 0

	if (SECURE_HEAP_SIZE != 0)
	{
		heapInfoIndex = BSPCFG_P_HeapType_Secure;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memcIndex = gHeapInfo[heapInfoIndex].memcIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].subIndex = gHeapInfo[heapInfoIndex].uSubIndex;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].memoryType = gHeapInfo[heapInfoIndex].heapType;
		platformSettings->heap[gHeapInfo[heapInfoIndex].heapIndex].size += SIZE_ALIGNED(SECURE_HEAP_SIZE, 20);
	}


#if defined(HEAP_SEPERATE)
	int i;
	for (i=0; i<BSPCFG_P_HeapType_Last; i++)
	{
		platformSettings->heap[i].memoryType = NEXUS_MemoryType_eFull;
	}
#endif
	
/****************************************************************************************************************
Result:
****************************************************************************************************************/
	{
		uint8_t 	uHeapLoop, uSubLoop;
		uint32_t 	uTotalSize = 0, uSubSize[2] = {0, 0};
		uint32_t 	uSizeMbytes, uSizeKbytes;
		const char *cHeapName[] = {
			"Heap_General",
			"Heap_Gfx",
			"Heap_Avd_General",
			"Heap_Avd_Picture",
			"Heap_Ragga", 
			"Heap_Vdc",
			"Heap_Xpt", 
			"Heap_Secure", 
			"Heap_Total"
			};

		BKNI_Printf("\n");
		BKNI_Printf("##########################################################\n");
		for (uHeapLoop = 0; uHeapLoop < BSPCFG_P_HeapType_Last; uHeapLoop++) {
			uSizeMbytes =  platformSettings->heap[uHeapLoop].size>>20;
			uSizeKbytes =  (uint32_t)((platformSettings->heap[uHeapLoop].size - (uSizeMbytes<<20))*1000)>>20;
			BKNI_Printf("#%20s Size: %4d.%d%d%d Mbytes, Type: %d, Index : %d\n", cHeapName[uHeapLoop], uSizeMbytes, 
																	uSizeKbytes/100, (uSizeKbytes%100)/10, uSizeKbytes%10,
																	platformSettings->heap[uHeapLoop].memoryType, gHeapInfo[uHeapLoop].uSubIndex);
			uTotalSize += platformSettings->heap[uHeapLoop].size;
			uSubSize[gHeapInfo[uHeapLoop].uSubIndex] += platformSettings->heap[uHeapLoop].size;

		}	
		BKNI_Printf("#\n");
		for (uSubLoop = 0; uSubLoop < 2; uSubLoop++) {
			BKNI_Printf("#%20d Size: %4d\n", uSubLoop, uSubSize[uSubLoop]>>20);
		}
		BKNI_Printf("#\n");
		uSizeMbytes = uTotalSize>>20;
		uSizeKbytes =  (uint32_t)((uTotalSize - (uSizeMbytes<<20))*1000)>>20;
		BKNI_Printf("#\n");
		BKNI_Printf("#%20s size: %4d.%d%d%d Mbytes\n", cHeapName[BSPCFG_P_HeapType_Last], uSizeMbytes, 
													uSizeKbytes/100, (uSizeKbytes%100)/10, uSizeKbytes%10);
		BKNI_Printf("##########################################################\n");
	}
}

void BSPCFG_GetPlatformSettings(NEXUS_PlatformSettings *tPlatformSettings)
{
	/* Set the video decoder configuration */
	{
		NEXUS_VideoDecoderModuleSettings *tVideoDecoderModuleSettings = &(tPlatformSettings->videoDecoderModuleSettings);
#if BSPCFG_P_HEAP_XVD_PICTURE_SIZE
		tVideoDecoderModuleSettings->avdHeapIndex[0] = gHeapInfo[BSPCFG_P_HeapType_Xvd_Picture].heapIndex;
		tVideoDecoderModuleSettings->avdHeapIndex[1] = gHeapInfo[BSPCFG_P_HeapType_Xvd_Picture].heapIndex;
#else
		tVideoDecoderModuleSettings->avdHeapIndex[0] = gHeapInfo[BSPCFG_P_HeapType_Xvd_General].heapIndex;
		tVideoDecoderModuleSettings->avdHeapIndex[1] = gHeapInfo[BSPCFG_P_HeapType_Xvd_General].heapIndex;
#endif
		tVideoDecoderModuleSettings->hostAccessibleHeapIndex = gHeapInfo[BSPCFG_P_HeapType_Xvd_General].heapIndex;
		/*tVideoDecoderModuleSettings->secureHeap = g_NexusCore.publicHandles.nexusHeap[BSPCFG_P_HeapType_Secure];*/
		/* Clear out the default video codecs */
#if 0
		BKNI_Memset(&(tVideoDecoderModuleSettings->supportedCodecs), 0, sizeof(tVideoDecoderModuleSettings->supportedCodecs));
		tVideoDecoderModuleSettings->supportedCodecs[0]  = NEXUS_VideoCodec_eMpeg2;
		tVideoDecoderModuleSettings->supportedCodecs[1]  = NEXUS_VideoCodec_eH264;
		tVideoDecoderModuleSettings->maxDecodeFormat	 = NEXUS_VideoFormat_e1080i;
		tVideoDecoderModuleSettings->maxStillDecodeFormat =  NEXUS_VideoFormat_e1080i;
#endif
#if SUPPORT_VIDEO_CODEC_SVC	== 0
		tVideoDecoderModuleSettings->supportedCodecs[NEXUS_VideoCodec_eH264_Svc]  = NEXUS_VideoCodec_eNone;
#endif
		
#if NUM_VIDEO_DECODER != 2
		tVideoDecoderModuleSettings->supportedCodecs[NEXUS_VideoCodec_eH264_Mvc]  = NEXUS_VideoCodec_eNone;
#endif

		tVideoDecoderModuleSettings->heapSize[0].general = BSPCFG_P_HEAP_XVD_GENERAL_SIZE;
		tVideoDecoderModuleSettings->heapSize[0].secure = BSPCFG_P_HEAP_XVD_SECURE_SIZE;
		tVideoDecoderModuleSettings->heapSize[0].picture = BSPCFG_P_HEAP_XVD_PICTURE_SIZE;

		tVideoDecoderModuleSettings->numDecodes 		= BSPCFG_P_VIDEO_DECODE_NUMBER;
		tVideoDecoderModuleSettings->numStillDecodes	= BSPCFG_P_VDIDE_STILL_NUMBER_PER_DECODER;
		tVideoDecoderModuleSettings->numMosaicDecodes	= BSPCFG_P_VIDEO_MOSAIC_DECODE_NUMBER;
	}
	
	/* Set the display configuration */
	{
		NEXUS_DisplayModuleSettings *tDisplaySettings = &(tPlatformSettings->displayModuleSettings);
		uint8_t uHeapLoop;

#if defined(CONFIG_VBI_TELETEXT)
		tDisplaySettings->vbi.allowTeletext = true;
		tDisplaySettings->vbi.tteShiftDirMsb2Lsb = true;
		tDisplaySettings->primaryDisplayHeapIndex = gHeapInfo[BSPCFG_P_HeapType_Vdc].heapIndex;
#endif
		
		for (uHeapLoop=0;uHeapLoop<NEXUS_MAX_HEAPS;uHeapLoop++ ) 
		{
		
			if (uHeapLoop == gHeapInfo[BSPCFG_P_HeapType_Vdc].heapIndex)
			{
				tDisplaySettings->displayHeapSettings[uHeapLoop].fullHdBuffers.count = BSPCFG_P_HEAP_VDC_FULL_HD_BUFFER_NUM;
				tDisplaySettings->displayHeapSettings[uHeapLoop].hdBuffers.count = BSPCFG_P_HEAP_VDC_HD_BUFFER_NUM;
				tDisplaySettings->displayHeapSettings[uHeapLoop].sdBuffers.count = BSPCFG_P_HEAP_VDC_SD_BUFFER_NUM;
				tDisplaySettings->displayHeapSettings[uHeapLoop].fullHdBuffers.pipCount = BSPCFG_P_HEAP_VDC_FULL_HD_PIP_BUFFER_NUM;
				tDisplaySettings->displayHeapSettings[uHeapLoop].hdBuffers.pipCount = BSPCFG_P_HEAP_VDC_HD_PIP_BUFFER_NUM;
				tDisplaySettings->displayHeapSettings[uHeapLoop].sdBuffers.pipCount = BSPCFG_P_HEAP_VDC_SD_PIP_BUFFER_NUM;
			}
			else
			{
				tDisplaySettings->displayHeapSettings[uHeapLoop].fullHdBuffers.count = 0;
				tDisplaySettings->displayHeapSettings[uHeapLoop].hdBuffers.count = 0;
				tDisplaySettings->displayHeapSettings[uHeapLoop].sdBuffers.count = 0;
				tDisplaySettings->displayHeapSettings[uHeapLoop].fullHdBuffers.pipCount = 0;
				tDisplaySettings->displayHeapSettings[uHeapLoop].hdBuffers.pipCount = 0;
				tDisplaySettings->displayHeapSettings[uHeapLoop].sdBuffers.pipCount = 0;
			}
			tDisplaySettings->displayHeapSettings[uHeapLoop].fullHdBuffers.format = NEXUS_VideoFormat_e1080p;
			tDisplaySettings->displayHeapSettings[uHeapLoop].fullHdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
			tDisplaySettings->displayHeapSettings[uHeapLoop].hdBuffers.format = NEXUS_VideoFormat_e1080i;
			tDisplaySettings->displayHeapSettings[uHeapLoop].hdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
			tDisplaySettings->displayHeapSettings[uHeapLoop].sdBuffers.format = NEXUS_VideoFormat_ePalG;
			tDisplaySettings->displayHeapSettings[uHeapLoop].sdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
		}
		
		/*Separate VDC buffer from general Heap because only device access only this buffer*/
		tDisplaySettings->videoWindowHeapIndex[0][0] = gHeapInfo[BSPCFG_P_HeapType_Vdc].heapIndex;
		tDisplaySettings->videoWindowHeapIndex[0][1] = gHeapInfo[BSPCFG_P_HeapType_Vdc].heapIndex;
		tDisplaySettings->videoWindowHeapIndex[1][0] = gHeapInfo[BSPCFG_P_HeapType_Vdc].heapIndex;
		tDisplaySettings->videoWindowHeapIndex[1][1] = gHeapInfo[BSPCFG_P_HeapType_Vdc].heapIndex;
	}

	/* Set the Transport configuration */
	{
		NEXUS_TransportModuleSettings *tTransportModuleSettings = &(tPlatformSettings->transportModuleSettings);
		uint8_t uParserBandLoop = 0; 

		tTransportModuleSettings->mainHeapIndex = gHeapInfo[BSPCFG_P_HeapType_Xpt].heapIndex;
		
		for(uParserBandLoop = NEXUS_ParserBand_e0; uParserBandLoop < NEXUS_NUM_PARSER_BANDS; uParserBandLoop++)
		{
			tTransportModuleSettings->clientEnabled.parserBand[uParserBandLoop].rave = false;
			tTransportModuleSettings->clientEnabled.parserBand[uParserBandLoop].message = false;
			tTransportModuleSettings->clientEnabled.parserBand[uParserBandLoop].remux[0] = false;
			tTransportModuleSettings->clientEnabled.parserBand[uParserBandLoop].remux[1] = false;
		}
		for(uParserBandLoop = NEXUS_ParserBand_e0; uParserBandLoop < CONFIG_NUM_PARSER_BANDS; uParserBandLoop++)
		{
			tTransportModuleSettings->clientEnabled.parserBand[uParserBandLoop].rave = true;
			tTransportModuleSettings->clientEnabled.parserBand[uParserBandLoop].message = true;
		}
		

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


#if defined(CONFIG_TRANSCODER)
		for(uParserBandLoop = BSPCFG_P_DEMUX_PB_NUMBER; 
			uParserBandLoop < (BSPCFG_P_DEMUX_PB_NUMBER + BSPCFG_P_ENCODE_PLAYPUMP_NUMBER);
			uParserBandLoop++)
		{
			tTransportModuleSettings->maxDataRate.playback[uParserBandLoop] = BSPCFG_P_ENCODE_PB_BITRATES;
			tTransportModuleSettings->clientEnabled.playback[uParserBandLoop].rave = BSPCFG_P_ENCODE_PB_RAVE_SUPPORTED;
			tTransportModuleSettings->clientEnabled.playback[uParserBandLoop].message = BSPCFG_P_ENCODE_PB_MSG_SUPPORTED;
			tTransportModuleSettings->clientEnabled.playback[uParserBandLoop].remux[0] = BSPCFG_P_ENCODE_PB_RMX0_SUPPORTED;
			tTransportModuleSettings->clientEnabled.playback[uParserBandLoop].remux[1] = BSPCFG_P_ENCODE_PB_RMX1_SUPPORTED;
		}
#endif

	}	

	/* Set the Audio configuration */
	{
		NEXUS_AudioModuleSettings *tAudioModuleSettings = &(tPlatformSettings->audioModuleSettings);

		tAudioModuleSettings->firmwareHeapIndex = gHeapInfo[BSPCFG_P_HeapType_Ragga].heapIndex;
		tAudioModuleSettings->maxAudioDspTasks = BSPCFG_P_AUDIO_MAX_TASKS_NUMBER; /* 3 decoders + 1 FW Mixer */
	//	tAudioModuleSettings->numPcmBuffers = 4; /* 5.1 + one stereo */
	//	tAudioModuleSettings->numCompressedBuffers = 2; /* For AC3+ passthrough we need one for SPDIF out (AC3) and one for HDMI (AC3+) */
	//	tAudioModuleSettings->loudnessMode = BSPCFG_P_AUDIO_LOUDNESS_MODE;
	}
	
	/* Set the Gfx configuration */
	{
		NEXUS_SurfaceModuleSettings *tSurfaceModuleSettings = &(tPlatformSettings->surfacePlatformSettings);

		tSurfaceModuleSettings->heapIndex = gHeapInfo[BSPCFG_P_HeapType_Surface].heapIndex;
	}
		
	sBSPCFG_SetHeapSettings(tPlatformSettings);

}


HUINT32 DRV_CFG_PlatformInit(void)
{

	NEXUS_PlatformSettings platformSettings;
	NEXUS_Error	eNError		= NEXUS_SUCCESS;
	HUINT32						i;
#if defined(CONFIG_DIRECTFB)
	NEXUS_PlatformStartServerSettings platformStartServerSettings;
	DFB_PlatformSettings dfbSettings;
	DFB_PlatformResult eDfbError	= DFB_PLATFORM_OK;
	NEXUS_HeapHandle * pHeap = NULL;
#endif

	NEXUS_Platform_GetDefaultSettings(&platformSettings);
	platformSettings.openFpga = false;
#if defined (CONFIG_DEMOD_INTERNAL)
	platformSettings.openFrontend = true;
#else
	platformSettings.openFrontend = false;
#endif

#if	defined(BCM_MEMORY_DEBUG)
	NEXUS_SetEnv("debug_mem", "");
#endif

#if defined(BCM_HDMI_BYPASS_EDID)
	NEXUS_SetEnv("hdmi_bypass_edid", "");
#endif

#if defined(CONFIG_CONT_COUNT_IGNORE)
	NEXUS_SetEnv("cont_count_ignore","");
#endif


	BSPCFG_GetPlatformSettings(&platformSettings);
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
#if defined(CONFIG_DIRECTFB_VER_1_4_17)
	platformStartServerSettings.allowUnprotectedClientsToCrash = true;
	platformStartServerSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eUnprotected;
	pHeap = platformStartServerSettings.unauthenticatedConfiguration.heap;
	
	{
		NEXUS_PlatformConfiguration nexusPlatformConfig;
		NEXUS_Platform_GetConfiguration(&nexusPlatformConfig);

		for(i=0;i<NEXUS_MAX_HEAPS;i++)
			pHeap[i] = nexusPlatformConfig.heap[i];
		
	}
#else
	platformStartServerSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eUntrusted;
	platformStartServerSettings.unauthenticatedConfiguration.heap[0] = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
#endif
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

#if defined(CONFIG_FAN_SCENARIO_30)
/************************************************************************************
	FAN
*************************************************************************************/
/* FAN Execution Temperature */
const HUINT8 s_FanExecuteTemp[7] =
{
	55,	/* INITIAL_THRESHOLD */
	50,	/* LOW_THRESHOLD */
	53,	/* LOW_MIDDLE_THRESHOLD */
	55,	/* MIDDLE_THRESHOLD */
	58,	/* MIDDLE_HIGH_THRESHOLD */
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
		usOnInterval = 106;		/* Changed maximum voltage of FAN to 10V for thermal and acoustic condition */
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

#endif
