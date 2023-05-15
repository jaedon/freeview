#ifndef __BOARD_CFE_H__
#define __BOARD_CFE_H__

#if defined(CONFIG_BMEM_OPTIMIZE) && !defined(CONFIG_OS_UCOS)
#include "unistd.h"
#endif

#include "linden_trace.h"
#include "drv_gpio.h"
#include "drv_tspath.h"
#include "drv_channel_config.h"
#include "drv_led.h"
#include "drv_fan.h"
#include "drv_network.h"
#include "drv_hdmi.h"
#include "di_key.h"
#include "di_led.h"
#include "flash_map.h"
#include "ram_map.h"
#if defined(CONFIG_DIRECTFB)
#include "nexus_platform_server.h"
#include "dfb_platform.h"
#endif
#include "di_loader.h"
#include "nexus_platform.h"

#include "drv_csd.h"
#include "nexus_otpmsp.h"

#define SIZE_ALIGNED(size, align_byte)	((((size)+(align_byte-1)) / align_byte) * align_byte)
#define DEBUG_MODULE	TRACE_MODULE_DI_INIT

/* Conax Requirement Chipset OTP */
const DRV_CSD_ChipsetOTP ChipsetOTP[] =
{
	/* NEXUS OtpCmdMsp, Bit length(8bits), Bit Mask(32bits), Msp Data(32bits) */
	{NEXUS_OtpCmdMsp_eReserved149, 1, 0x00000001, 0x00000001}, /* Remux0Disable */
	{NEXUS_OtpCmdMsp_eReserved150, 1, 0x00000001, 0x00000001}, /* Remux1Disable */
	{NEXUS_OtpCmdMsp_eReserved14, 1, 0x00000001, 0x00000001}, /* ForceEncryptionRemux0Disable */
	{NEXUS_OtpCmdMsp_eReserved15, 1, 0x00000001, 0x00000001}, /* ForceEncryptionRemux1Disable */
/*	{NEXUS_OtpCmdMsp_eHostBootCodeDecryptionSelect, 4, 0x0000000F, 0x00000008},*/ /* NEXUS_OtpCmdMsp_eReserved80 */
/*	{NEXUS_OtpCmdMsp_eSystemEpoch, 8, 0x000000FF, 0x00000000},*/ /* NEXUS_OtpCmdMsp_eReserved87 */
/*	{NEXUS_OtpCmdMsp_eReserved520, 8, 0x000000FF, 0x00000000},*/ /* SystemEpoch1 */
/*	{NEXUS_OtpCmdMsp_eReserved519, 8, 0x000000FF, 0x00000000},*/ /* SystemEpoch2 */
/*	{NEXUS_OtpCmdMsp_eReserved518, 8, 0x000000FF, 0x00000000},*/ /* SystemEpoch3 */
	{NEXUS_OtpCmdMsp_eReserved147, 1, 0x00000001, 0x00000001}, /* CpDvbCsa3SoftwareKeyDisable */
	{NEXUS_OtpCmdMsp_eReserved148, 1, 0x00000001, 0x00000001}, /* CpDvbCsa2SoftwareKeyDisable */
	{NEXUS_OtpCmdMsp_eReserved78, 1, 0x00000001, 0x00000001}, /* RaveVerifyEnable */
	{NEXUS_OtpCmdMsp_eReserved76, 1, 0x00000001, 0x00000001}, /* RaagaAVerifyEnable */
	{NEXUS_OtpCmdMsp_eReserved193, 1, 0x00000001, 0x00000001}, /* RaagaBVerifyEnable */
	{NEXUS_OtpCmdMsp_eReserved77, 1, 0x00000001, 0x00000001}, /* VideoVerifyEnable */
	{NEXUS_OtpCmdMsp_eReserved131, 1, 0x00000001, 0x00000001}, /* ViceVerifyEnable */
	{NEXUS_OtpCmdMsp_eReserved142, 1, 0x00000001, 0x00000001}, /* SidVerifyEnable */
	{NEXUS_OtpCmdMsp_eReserved598, 1, 0x00000001, 0x00000001}, /* AvsVerifyEnable */
	{NEXUS_OtpCmdMsp_eReserved209, 1, 0x00000001, 0x00000001}, /* SageVerifyEnable */
	{NEXUS_OtpCmdMsp_eReserved95, 1, 0x00000001, 0x00000001}, /* MpodDisable */
	{NEXUS_OtpCmdMsp_eReserved7, 1, 0x00000001, 0x00000001}, /* PCIe0HostProtect */
	{NEXUS_OtpCmdMsp_eReserved8, 1, 0x00000001, 0x00000001}, /* PCIe0ClientProtect */
	{NEXUS_OtpCmdMsp_ePcieHostProtect, 1, 0x00000001, 0x00000001}, /* PCIe1HostProtect */
	{NEXUS_OtpCmdMsp_ePcieClientProtect, 1, 0x00000001, 0x00000001}, /* PCIe1ClientProtect */
	{NEXUS_OtpCmdMsp_eCaDvbCsa3SoftwareKeyDisable, 1, 0x00000001, 0x00000001}, /* CaDvbCsa3SoftwareKeyDisable */
	{NEXUS_OtpCmdMsp_eReserved143, 1, 0x00000001, 0x00000001} /* BgCheckResetEnable */
/*	{NEXUS_OtpCmdMsp_eReserved93, 8, 0x000000FF, 0x00000000}, */ /* BseckFwEpoch */
/*	{NEXUS_OtpCmdMsp_eReserved152, 1, 0x00000001, 0x00000001} */ /* DramScramblerKeyReuseDisable */
};

HUINT32 DRV_CFG_GetChipsetOTPMaxNum(void)
{
	return (sizeof(ChipsetOTP)/sizeof(DRV_CSD_ChipsetOTP));
}

/* GPIO Configuration */
const GPIO_ConfigInfo_t GPIO_ConfigInfoTable[] =
{
	{GPIO_ID_HDD_PWR_CTRL,   NULL,    NEXUS_GpioType_eAonStandard,   20,    GPIO_MODE_WRITE,        GPIO_INT_MODE_DISABLED,      GPIO_LOW},
	{GPIO_ID_LED_STBY,       NULL,    NEXUS_GpioType_eAonStandard,   3,     GPIO_MODE_WRITE,        GPIO_INT_MODE_DISABLED,      GPIO_LOW},
	{GPIO_ID_LED_POWER,      NULL,    NEXUS_GpioType_eAonStandard,   4,     GPIO_MODE_WRITE,        GPIO_INT_MODE_DISABLED,      GPIO_LOW},
	{GPIO_ID_ENET_LED,       NULL,    NEXUS_GpioType_eStandard,      95,    GPIO_MODE_WRITE,        GPIO_INT_MODE_DISABLED,      GPIO_LOW},
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	{GPIO_ID_BOOT_MODE,      NULL,    NEXUS_GpioType_eAonStandard,   14,    GPIO_MODE_WRITE,        GPIO_INT_MODE_DISABLED,      GPIO_HI}, /* H : Warm Boot  L : Cold Boot */
#endif
#if defined(CONFIG_STLED)
	{GPIO_ID_SPI_M_SS0,      NULL,    NEXUS_GpioType_eAonStandard,   19,    GPIO_MODE_WRITE,        GPIO_INT_MODE_DISABLED,      GPIO_HI}
#endif
};

HUINT32 DRV_CFG_GetGPIOMaxDevice(void)
{
	return (sizeof(GPIO_ConfigInfoTable)/sizeof(GPIO_ConfigInfo_t));
}

#if defined(CONFIG_FRONTKEY_GPIO)
GPIO_KeyMappingTable_t s_GPIO_KeyConfig[] =
{
	{GPIO_ID_KEY_SW_RESET,	"SW_REBOOT",	0,	GPIO_INVAILD,	DI_KEY_RELEASED},
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
		DI_KEY_FRONT_SW_REBOOT,			/* Micom KeyID 0 */
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
	{NEXUS_InputBand_e0, 0},
	{NEXUS_InputBand_e1, 1},
	{NEXUS_InputBand_e2, 2},
	{NEXUS_InputBand_e3, 3},
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
	{0, DI_CH_TYPE_CAB},
	{1, DI_CH_TYPE_CAB},
	{2, DI_CH_TYPE_CAB},
	{3, DI_CH_TYPE_CAB},
};

const DRV_CH_TSOUTPUTMODE s_TSConfig[] =
{
	{0x4, CH_TS_MODE_MTSIF},
	{0x4, CH_TS_MODE_MTSIF},
	{0x4, CH_TS_MODE_MTSIF},
	{0x4, CH_TS_MODE_MTSIF},
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

#if defined(CONFIG_BMEM_OPTIMIZE)
/* Generated by bmemconfig.html on 19700101-005620 */
/*     Platform: 97250 B0 */
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
    pMemConfigSettings->videoDecoder[0].maxFormat = 32; /* 1080p */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[1 ] = 1; /* mpeg1 */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[2 ] = 1; /* mpeg2 */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[3 ] = 1; /* divx */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[4 ] = 1; /* h263 */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[5 ] = 1; /* avc */
    pMemConfigSettings->videoDecoder[0].supportedCodecs[7 ] = 1; /* mvc */
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
    pMemConfigSettings->videoDecoder[0].colorDepth = 10;
    pMemConfigSettings->videoDecoder[0].mosaic.maxNumber = 0;
    pMemConfigSettings->videoDecoder[0].mosaic.maxWidth = 352;
    pMemConfigSettings->videoDecoder[0].mosaic.maxHeight = 288;
    pMemConfigSettings->videoDecoder[0].avc51Supported = 0;
    pMemConfigSettings->videoDecoder[1].used      = 0;
    memset( pMemConfigSettings->videoDecoder[1].supportedCodecs, 0, sizeof( pMemConfigSettings->videoDecoder[1].supportedCodecs ));
#endif /* if NEXUS_HAS_VIDEO_DECODER */

#if NEXUS_NUM_SID_VIDEO_DECODERS
    pMemConfigSettings->stillDecoder[0].used      = 1;
    memset( pMemConfigSettings->stillDecoder[0].supportedCodecs, 0, sizeof( pMemConfigSettings->stillDecoder[0].supportedCodecs ));
    pMemConfigSettings->stillDecoder[0].maxFormat = 32; /* 1080p */
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
#endif /* if NEXUS_NUM_SID_VIDEO_DECODERS */

#if NEXUS_HAS_DISPLAY && NEXUS_NUM_VIDEO_WINDOWS
    pMemConfigSettings->display[0].maxFormat = 32; /* 1080p */
    pMemConfigSettings->display[0].window[0].used = 1;
    pMemConfigSettings->display[0].window[0].convertAnyFrameRate = 1;
    pMemConfigSettings->display[0].window[0].precisionLipSync    = 1;
    pMemConfigSettings->display[0].window[0].capture             = 1;
    pMemConfigSettings->display[0].window[0].deinterlacer        = 1;
    pMemConfigSettings->display[0].window[0].userCaptureBufferCount = 0;
    pMemConfigSettings->display[0].window[1].used = 0;
    pMemConfigSettings->display[1].maxFormat = 0;
#endif /* if NEXUS_HAS_DISPLAY && NEXUS_NUM_VIDEO_WINDOWS */

    pPlatformSettings->heap[0].size = 125*1024*1024; /* {98MB + cdb(+13MB) + dmx(8MB), thumbnail 6MB} Main */

    pPlatformSettings->heap[1].size = 26*1024*1024; /* Graphics */

    pPlatformSettings->heap[3].size = 40*1024*1024; /* Secure */

    pPlatformSettings->heap[4].size = 0*1024*1024; /* Picture */
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
    platformSettings.securitySettings.numKeySlotsForType[0] = 50;	/* for ca, cacp, rmx */
    platformSettings.securitySettings.numKeySlotsForType[1] = 0;
	platformSettings.securitySettings.numKeySlotsForType[2] = 30;	/* Allocate slot type 2 for supporting seperate IV keys */
    platformSettings.securitySettings.numKeySlotsForType[3] = 15;	/* for m2m */
    platformSettings.securitySettings.numKeySlotsForType[4] = 0;
#endif

#if defined(CONFIG_OS_UCOS)
	NEXUS_GetDefaultMemoryConfigurationSettings(&memConfigSettings);

	/* Image Decryption ?? Á¦??????:
	 : Encrypted Image??Nexus Heap ???? alloc?? ?ø°£¿?? decryption ?????Ø¾???.
	   (alloc ???? ??Àº ?ø°£¿?? decryption ?????? BMRC_MONITOR???? 'violation access' ???? error ?ß»?.)
	 =>alloc???? ?Ê°? ?????Ï±? À§?Ø¼???'custom_arc'?? enable ??Å´. */
	NEXUS_SetEnv("custom_arc","y");

	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].size = 416*1024*1024; /* Main Heap */
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].subIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = 24*1024*1024; /* Graphics Heap */
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].subIndex = 1;
	platformSettings.heap[NEXUS_MEMC0_DRIVER_HEAP].size = 0*1024*1024; /* Driver Heap */
	platformSettings.heap[NEXUS_MEMC0_DRIVER_HEAP].subIndex = 0;
	platformSettings.heap[NEXUS_VIDEO_SECURE_HEAP].size = 36*1024*1024; /* Secure Heap */
	platformSettings.heap[NEXUS_VIDEO_SECURE_HEAP].subIndex = 1;
	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = 24*1024*1024; /* Picture Heap */
	platformSettings.heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 0;


	eNError = NEXUS_Platform_MemConfigInit(&platformSettings, &memConfigSettings);
	if(eNError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_Platform_MemConfigInit(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eNError, __LINE__, __FILE__);
		return DRV_ERR;
	}
#else

	NEXUS_GetDefaultMemoryConfigurationSettings(&memConfigSettings);
#if defined(CONFIG_BMEM_OPTIMIZE)
	mem_usage(&platformSettings, &memConfigSettings);
#endif
#if defined(CONFIG_DIRECTFB)
	platformSettings.heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = 90*1024*1024; /* Graphics */
#endif
#if defined(CONFIG_DEVICE_MODE_USER)
	platformSettings.heap[0].size += 10*1024*1024;
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
	{NEXUS_VideoFormat_e1080p50hz, NEXUS_HdmiColorDepth_e8bit, 0x39},
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
/* ulLoadingAddress */				HDF_DECOMPRESSED_RAM_ADDR,
/* ulSignatureCheckAddress */		0,
/* ulRamdiskRootfsLoadingAddress */	0,
/* ulEntryAddress */				HDF_DECOMPRESSED_RAM_ADDR,
/* defaultImageSize */				HDF_DECOMPRESSED_RAM_SIZE,
/* Image_Type */					Image_Type_BINARY_ZIPPED,
/* Image_OS */					Image_OS_Linux_Ramdisk,
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
/* Secure Execution */				TRUE,
/* Divide Signature */				TRUE,
/* Encrypt Image */					TRUE
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
/* Secure Execution */				TRUE,
/* Divide Signature */				TRUE,
/* Encrypt Image */					TRUE
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
/* Secure Execution */				TRUE,
/* Divide Signature */				TRUE,
/* Encrypt Image */				TRUE
								}
	},
	{
/* Image_Usage_t */				Image_Usage_UpgraderBoot2nd,
								{
/* ulStoredAddress */				(N_PART_OTA_OFFSET+N_IMAGE_OTA2NDLDR_OFFSET),
/* ulLoadingAddress */				0,
/* ulSignatureCheckAddress */		0,
/* ulRamdiskRootfsLoadingAddress */	0,
/* ulEntryAddress */				0,
/* defaultImageSize */				N_IMAGE_OTA2NDLDR_SIZE,
/* Image_Type */					Image_Type_ELF,
/* Image_OS */					Image_OS_Linux_Ramdisk,
/* Flash_Type */					Flash_Type_eMMC,
/* Secure Execution */				TRUE,
/* Divide Signature */				TRUE,
/* Encrypt Image */				FALSE
								}
	},
};

HUINT32 DRV_CFG_GetLoaderImageInfoMax(void)
{
	return (sizeof(loaderImageInfoTable)/sizeof(Loader_ImageInfo_t));
}

#if defined(CONFIG_LED_REFACTORING)
static int set_led(struct led_setting_t *pSetting)
{
	int rc = -1;
	DRV_Error eDrvErr;
	static int curr_func_state = LED_FUNC_NONE;

	DONE_ERR((!pSetting));

	switch(pSetting->func) {
	case LED_FUNC_WATCHING:
		eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);
		DONE_ERR((eDrvErr!=DRV_OK));
		eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);
		DONE_ERR((eDrvErr!=DRV_OK));
		curr_func_state = pSetting->func;
		break;
	case LED_FUNC_STANDBY:
		eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_HI);
		DONE_ERR((eDrvErr!=DRV_OK));
		eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);
		DONE_ERR((eDrvErr!=DRV_OK));
		curr_func_state = pSetting->func;
		break;
	case LED_FUNC_NORMAL:
		if (pSetting->on == FALSE)
		{
			eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);
			DONE_ERR((eDrvErr!=DRV_OK));
			eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);
			DONE_ERR((eDrvErr!=DRV_OK));
		}
		curr_func_state = pSetting->func;
		break;
	case LED_FUNC_RCU_BLINKING:
		if (curr_func_state == LED_FUNC_WATCHING)
		{
			eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);
			DONE_ERR((eDrvErr!=DRV_OK));
			eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);
			DONE_ERR((eDrvErr!=DRV_OK));

			VK_TASK_Sleep(50);

			eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);
			DONE_ERR((eDrvErr!=DRV_OK));
			eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);
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

	tLedDrv->led_cap = LED_ID_POWER;
	tLedDrv->use_dimming = false;
	tLedDrv->set = set_led;

	eDrvErr = DRV_OK;
done:
	return eDrvErr;
}
#endif

#endif
