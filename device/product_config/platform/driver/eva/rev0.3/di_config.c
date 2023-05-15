
#ifndef __BOARD_CFE_H__
#define __BOARD_CFE_H__

#include "drv_gpio.h"
#include "drv_tspath.h"
#include "drv_channel_config.h"
#include "di_key.h"
#include "di_led.h"
#include "di_system.h"
#include "drv_fan.h"
#include "drv_led.h"
#include "flash_map.h"
#include "ram_map.h"
#include "di_loader.h"

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
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	/* H : Warm Boot  L : Cold Boot */
	{GPIO_ID_BOOT_MODE,	NULL,	NEXUS_GpioType_eAonStandard,		15,	GPIO_MODE_WRITE,	GPIO_INT_MODE_DISABLED,	GPIO_HI},
#endif

#if defined(CONFIG_OS_LINUX)
	/* LED */
	{GPIO_ID_LED_STBY,	NULL,	NEXUS_GpioType_eAonStandard,		17, 	GPIO_MODE_WRITE,	GPIO_INT_MODE_DISABLED,	GPIO_LOW},
	{GPIO_ID_LED_STBY2,	NULL,	NEXUS_GpioType_eStandard,		90,	GPIO_MODE_WRITE,	GPIO_INT_MODE_DISABLED, GPIO_LOW},
	{GPIO_ID_LED_POWER,	NULL,	NEXUS_GpioType_eStandard,		98,	GPIO_MODE_WRITE,	GPIO_INT_MODE_DISABLED, GPIO_LOW},
	{GPIO_ID_LED_POWER2,	NULL,	NEXUS_GpioType_eStandard,		102,	GPIO_MODE_WRITE,	GPIO_INT_MODE_DISABLED, GPIO_LOW},
#endif
};


//JAMES...MXL683 has TEST MODE which TS outputs pin's state are low when reset is released.
//TS output pins of main chipset MUST be PULL NONE to keep the pin's state to be driven by MXL.
#if defined(CONFIG_OS_LINUX)
const GPIO_TSConfigInfo_t GPIO_TSConfigInfoTable[] =
{
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_9,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_9_gpio_061_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_9_gpio_061_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_9_gpio_061_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_062_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_062_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_062_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_064_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_064_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_064_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_072_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_072_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_072_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_073_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_073_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_073_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_075_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_075_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10_gpio_075_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12_gpio_097_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12_gpio_097_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12_gpio_097_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12_gpio_100_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12_gpio_100_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12_gpio_100_pad_ctrl_PULL_NONE
	},
	{
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12_gpio_103_pad_ctrl_MASK,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12_gpio_103_pad_ctrl_SHIFT,
		BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_12_gpio_103_pad_ctrl_PULL_NONE
	}
};
#endif

HUINT32 DRV_CFG_GetGPIOMaxDevice(void)
{
	return (sizeof(GPIO_ConfigInfoTable)/sizeof(GPIO_ConfigInfo_t));
}

HUINT32 DRV_CFG_Get_Max_TS_OUTPUT_Number(void)
{
#if defined(CONFIG_OS_UCOS)	
	return 0;
#else
	return (sizeof(GPIO_TSConfigInfoTable)/sizeof(GPIO_TSConfigInfo_t));
#endif
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
	DI_KEY_FRONT_WPS,                 	  /* Micom KeyID 16 */
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
	{NEXUS_InputBand_e0, 0},/*NOT USED*/
	{NEXUS_InputBand_e1, 1},/*NOT USED*/
	{NEXUS_InputBand_e2, 2},/*NOT USED*/
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
	{0x1, CH_TS_MODE_MTSIF},
	{0x1, CH_TS_MODE_MTSIF},
	{0x1, CH_TS_MODE_MTSIF},
	{0x1, CH_TS_MODE_MTSIF},
	{0x1, CH_TS_MODE_SERIAL},	/* TER */
	{0x1, CH_TS_MODE_SERIAL},	/* TER */
	{0x1, CH_TS_MODE_SERIAL}	/* TER */
};

#if defined(CONFIG_SUPPORT_SELECTABLE_RF)
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
	}
};
#endif

HUINT32 DRV_CFG_GetMaxChannel(void)
{
	return (sizeof(s_ChConfig)/sizeof(DRV_CH_CAPABILITY));
}

#if defined(CONFIG_DINETWORK)
const DRV_NETWORK_DEV_INFO_t s_stDevList[] =
{
	{0, "gphy", DI_NETWORK_DEV_LAN},
	{1, "eth1", DI_NETWORK_DEV_LAN},
	{2, "bcm0", DI_NETWORK_DEV_CABLEMODEM},
	{3, "wlan0", DI_NETWORK_DEV_WIFI}
};

HUINT32 DRV_CFG_GetNetMaxDevice(void)
{
	return (sizeof(s_stDevList)/sizeof(DRV_NETWORK_DEV_INFO_t));
}
#endif
#define DEBUG_MODULE	TRACE_MODULE_DI_INIT


#if !defined(CONFIG_SHARED_COMPILE)
HUINT32 DRV_CFG_PlatformInit(void)
{

	NEXUS_PlatformSettings platformSettings;
	NEXUS_Error	eNError		= NEXUS_SUCCESS;
#if defined(CONFIG_DIRECTFB)
	NEXUS_PlatformStartServerSettings platformStartServerSettings;
	DFB_PlatformSettings dfbSettings;
	DFB_PlatformResult eDfbError	= DFB_PLATFORM_OK;
	NEXUS_HeapHandle * pHeap = NULL;
#endif

#if !defined(CONFIG_OS_UCOS)
	NEXUS_MemoryConfigurationSettings memConfigSettings;
	int i=0;
#endif

	NEXUS_Platform_GetDefaultSettings(&platformSettings);
#if defined(CONFIG_OS_UCOS)
	platformSettings.openFrontend = false;

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
#else
	NEXUS_GetDefaultMemoryConfigurationSettings(&memConfigSettings);

	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].subIndex = 0;
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].size =  250*1024*1024;/*208*/ /*decoder FW+general,xpt playback,audio other general purpose */
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].alignment = 16*1024*1024;
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;


	for (i=NEXUS_ParserBand_e0; i<NEXUS_MAX_PARSER_BANDS; i++)
	{
		platformSettings.transportModuleSettings.maxDataRate.parserBand[i] = 54*1000*1000;
		platformSettings.transportModuleSettings.maxDataRate.playback[i] = 54*1000*1000;
		platformSettings.transportModuleSettings.clientEnabled.parserBand[i].rave = true;
		platformSettings.transportModuleSettings.clientEnabled.parserBand[i].message = true;
	}
	platformSettings.videoDecoderModuleSettings.maxStillDecodeFormat = NEXUS_VideoFormat_e1080p;

#if defined(CONFIG_CRYPTO) || defined(CONFIG_DSC)
	platformSettings.securitySettings.numKeySlotsForType[0] = 30;    /* keySlotEngine = ca */
	platformSettings.securitySettings.numKeySlotsForType[1] = 0;
	platformSettings.securitySettings.numKeySlotsForType[2] = 30;      /* Allocate slot type 2 for supporting seperate IV keys */
	platformSettings.securitySettings.numKeySlotsForType[3] = 30;	   /* keySlotEngine = m2m*/
	platformSettings.securitySettings.numKeySlotsForType[4] = 0;

	platformSettings.securitySettings.enableMulti2Key= true;
#endif

	platformSettings.openFpga = false;

#if defined (CONFIG_DEMOD_INTERNAL)
	platformSettings.openFrontend = true;
#else
	platformSettings.openFrontend = false;
#endif
	platformSettings.displayModuleSettings.vbi.tteShiftDirMsb2Lsb=true;

#if defined(CONFIG_PIP)
	memConfigSettings.stillDecoder[1].used = true;
	memConfigSettings.stillDecoder[1].maxFormat = NEXUS_VideoFormat_e1080p;
	memConfigSettings.stillDecoder[1].supportedCodecs[NEXUS_VideoCodec_eH265] = true;

	memConfigSettings.display[0].window[1].userCaptureBufferCount = 4;
#endif

	eNError = NEXUS_Platform_MemConfigInit(&platformSettings, &memConfigSettings);
	if(eNError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_Platform_MemConfigInit(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eNError, __LINE__, __FILE__);
		return DRV_ERR;
	}
	else
	{
		PrintDebug("%s->NEXUS_Platform_MemConfigInit(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
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
		{
			pHeap[i] = nexusPlatformConfig.heap[i];
		}
		pHeap[0] = nexusPlatformConfig.heap[NEXUS_PLATFORM_DEFAULT_HEAP];
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

	DFB_Platform_GetDefaultSettingsExtended(DFB_PlatformClientType_eNexusMaster, &dfbSettings, false);

#if defined(CONFIG_HDMI_REFACTORING)
	/* DirectFB uses HDMI 0 as displayOutput[0]. Do not add display output of HDMI. This will be done by di_hdmi_source.c */
	dfbSettings.displayOutput[0].init = false;
#endif

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

#endif /* CONFIG_DIRECTFB */

	return DRV_OK;
#endif
}
#endif

/************************************************************************************
  FAN
 *************************************************************************************/
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
	115	/* SHUTDOWN_THRESHOLD */
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
	115	/* SHUTDOWN_THRESHOLD */
};

const HUINT32 s_FanFreq = 0x0270;

#if defined(CONFIG_OS_LINUX)
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
#endif

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

const Loader_ImageInfo_t loaderImageInfoTable[] =
{
	/* 	Image_Usage_t/ulStoredAddress/ulLoadingAddress /ulSignatureCheckAddress/ulRamdiskRootfsLoadingAddress/ulEntryAddress/defaultImageSize/Image_Type/Image_OS/Flash_Type/bFeature_SecureExec/bFeature_DivideSig/bFeature_EncryptImg */
	{
		/* Image_Usage_t */				Image_Usage_NormalBoot,
		{
			/* ulStoredAddress */			(N_PART_IMAGE0_SIZE+N_IMAGE_KERNEL_OFFSET),
			/* ulLoadingAddress */			0, 												
			/* ulSignatureCheckAddress */		0, 
			/* ulRamdiskRootfsLoadingAddress */	0, 
			/* ulEntryAddress */			0, 
			/* defaultImageSize */			N_IMAGE_KERNEL_SIZE, 
			/* Image_Type */			Image_Type_BINARY_ZIPPED, 
			/* Image_OS */				Image_OS_Linux, 
			/* Flash_Type */			Flash_Type_eMMC,
			/* Secure Execution */			FALSE,
			/* Divide Signature */			FALSE,
			/* Encrypt Image */			FALSE
		}
	},
	{
		/* Image_Usage_t */				Image_Usage_FacotryAppBoot, 	
		{						
			/* ulStoredAddress */			0, 		
			/* ulLoadingAddress */			HDF_DECOMPRESSED_RAM_ADDR,
			/* ulSignatureCheckAddress */		0, 
			/* ulRamdiskRootfsLoadingAddress */	0, 
			/* ulEntryAddress */			HDF_DECOMPRESSED_RAM_ADDR, 
			/* defaultImageSize */			HDF_DECOMPRESSED_RAM_SIZE, 
			/* Image_Type */			Image_Type_BINARY_ZIPPED, 
			/* Image_OS */				Image_OS_Linux_Ramdisk, 
			/* Flash_Type */			Flash_Type_eMMC, 
			/* Secure Execution */			FALSE,
			/* Divide Signature */			FALSE,
			/* Encrypt Image */			FALSE
		}
	},
	{
		/* Image_Usage_t */				Image_Usage_LoaderInfo,
		{					
			/* ulStoredAddress */			0, 		
			/* ulLoadingAddress */			0, 												
			/* ulSignatureCheckAddress */		0, 
			/* ulRamdiskRootfsLoadingAddress */	0, 
			/* ulEntryAddress */			0, 
			/* defaultImageSize */			N_PART_LOADER_SIZE, 
			/* Image_Type */			Image_Type_BINARY, 
			/* Image_OS */				Image_OS_MAX, 
			/* Flash_Type */			Flash_Type_NOR, 
			/* Secure Execution */			FALSE,
			/* Divide Signature */			FALSE,
			/* Encrypt Image */			FALSE
		}
	},
	{
		/* Image_Usage_t */				Image_Usage_RootfsInfo,
		{ 							
			/* ulStoredAddress */			(N_PART_IMAGE0_OFFSET+N_IMAGE_ROOTFS_OFFSET), 		
			/* ulLoadingAddress */			0, 												
			/* ulSignatureCheckAddress */		0, 
			/* ulRamdiskRootfsLoadingAddress */	0, 
			/* ulEntryAddress */			0, 
			/* defaultImageSize */			N_IMAGE_ROOTFS_SIZE, 
			/* Image_Type */			Image_Type_BINARY_ZIPPED, 
			/* Image_OS */				Image_OS_Linux_Ramdisk, 
			/* Flash_Type */			Flash_Type_eMMC, 
			/* Secure Execution */			FALSE,
			/* Divide Signature */			FALSE,
			/* Encrypt Image */			FALSE
		}
	},
	{
		/* Image_Usage_t */				Image_Usage_CrytKernelInfo, 		
		{					
			/* ulStoredAddress */			(N_PART_IMAGE0_OFFSET+N_IMAGE_KERNEL_OFFSET), 		
			/* ulLoadingAddress */			0, 												
			/* ulSignatureCheckAddress */		KERNEL_RAM_CACHED_ADDR, 
			/* ulRamdiskRootfsLoadingAddress */	0, 
			/* ulEntryAddress */			CRYPT_KERNEL_RAM_ADDR, 
			/* defaultImageSize */			N_IMAGE_KERNEL_SIZE, 
			/* Image_Type */			Image_Type_BINARY_ZIPPED, 
			/* Image_OS */				Image_OS_Linux, 
			/* Flash_Type */			Flash_Type_eMMC, 
			/* Secure Execution */			TRUE,
			/* Divide Signature */			TRUE,
			/* Encrypt Image */			FALSE
		}
	},
	{
		/* Image_Usage_t */				Image_Usage_CrytRootfsInfo, 
		{							
			/* ulStoredAddress */			(N_PART_IMAGE0_OFFSET+N_IMAGE_ROOTFS_OFFSET), 		
			/* ulLoadingAddress */			0, 												
			/* ulSignatureCheckAddress */		ROOTFS_RAM_CACHED_ADDR, 
			/* ulRamdiskRootfsLoadingAddress */	0, 
			/* ulEntryAddress */			0, 
			/* defaultImageSize */			N_IMAGE_ROOTFS_SIZE, 
			/* Image_Type */			Image_Type_BINARY_ZIPPED, 
			/* Image_OS */				Image_OS_Linux, 
			/* Flash_Type */			Flash_Type_eMMC, 
			/* Secure Execution */			TRUE,
			/* Divide Signature */			TRUE,
			/* Encrypt Image */			FALSE
		}
	},
	{
		/* Image_Usage_t */				Image_Usage_DecompHdfBufInfo, 	
		{						
			/* ulStoredAddress */			0, 		
			/* ulLoadingAddress */			HDF_DECOMPRESSED_RAM_ADDR,
			/* ulSignatureCheckAddress */		0, 
			/* ulRamdiskRootfsLoadingAddress */	0, 
			/* ulEntryAddress */			0, 
			/* defaultImageSize */			HDF_DECOMPRESSED_RAM_SIZE, 
			/* Image_Type */			Image_Type_HDF, 
			/* Image_OS */				Image_OS_MAX, 
			/* Flash_Type */			Flash_Type_MAX, 
			/* Secure Execution */			FALSE,
			/* Divide Signature */			FALSE,
			/* Encrypt Image */			FALSE
		}
	},
	{
		/* Image_Usage_t */				Image_Usage_CompHdfBufInfo, 
		{							
			/* ulStoredAddress */			0, 		
			/* ulLoadingAddress */			HDF_COMPRESSED_RAM_ADDR,
			/* ulSignatureCheckAddress */		0, 
			/* ulRamdiskRootfsLoadingAddress */	0, 
			/* ulEntryAddress */			0, 
			/* defaultImageSize */			HDF_COMPRESSED_RAM_SIZE, 
			/* Image_Type */			Image_Type_HDF, 
			/* Image_OS */				Image_OS_MAX, 
			/* Flash_Type */			Flash_Type_MAX, 
			/* Secure Execution */			FALSE,
			/* Divide Signature */			FALSE,
			/* Encrypt Image */			FALSE
		}
	},
	{
		/* Image_Usage_t */				Image_Usage_UpgraderBoot, 		
		{					
			/* ulStoredAddress */			(N_PART_OTA_OFFSET+N_IMAGE_OTALDR_OFFSET), 		
			/* ulLoadingAddress */			0, 												
			/* ulSignatureCheckAddress */		0, 
			/* ulRamdiskRootfsLoadingAddress */	0, 
			/* ulEntryAddress */			0, 
			/* defaultImageSize */			N_IMAGE_OTALDR_SIZE, 
			/* Image_Type */			Image_Type_ELF,
			/* Image_OS */				Image_OS_Linux_Ramdisk, 
			/* Flash_Type */			Flash_Type_eMMC, 
			/* Secure Execution */			TRUE,
			/* Divide Signature */			TRUE,
			/* Encrypt Image */			FALSE
		}
	},
};

HUINT32 DRV_CFG_GetLoaderImageInfoMax(void)
{
	return (sizeof(loaderImageInfoTable)/sizeof(Loader_ImageInfo_t));
}

#if defined(CONFIG_LED_REFACTORING)
static HUINT32 LED_CAPABILITY = (DI_LED_NUM0|DI_LED_NUM1	// TV | RADIO
		|DI_LED_NUM2|DI_LED_NUM3);			// FULL | HD | DOLBI | +
#define POWER_LED_STANDBY	0x01
#define POWER_LED_LOGO_B	0x02
#define POWER_LED_LOGO_R	0x04
#define POWER_LED_LOGO_G	0x08

static int set_led(struct led_setting_t *pSetting)
{
	DRV_Error eDrvErr;
	int gpioId = -1;
	int gpioHiLow = -1;

	DONE_ERR((!pSetting));

	switch(pSetting->func) {
		case LED_FUNC_STANDBY:
			if(pSetting->id == LED_ID_POWER)
			{
				eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);
				DONE_ERR((eDrvErr!=DRV_OK)); 
			}
			break;
		case LED_FUNC_WATCHING:
			if(pSetting->id == LED_ID_POWER)
			{
				eDrvErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);
				DONE_ERR((eDrvErr!=DRV_OK)); 
			}
			break;
		case LED_FUNC_NORMAL:
			if(pSetting->id != LED_ID_POWER)
			{
				switch(pSetting->id) {
					case POWER_LED_STANDBY:
						gpioId = GPIO_ID_LED_STBY;
						break;
					case POWER_LED_LOGO_B:
						gpioId = GPIO_ID_LED_STBY2;
						break;
					case POWER_LED_LOGO_R:
						gpioId = GPIO_ID_LED_POWER;
						break;
					case POWER_LED_LOGO_G:
						gpioId = GPIO_ID_LED_POWER2;
						break;
					default:
						break;
				}
				if (pSetting->on == FALSE)
				{
					gpioHiLow = GPIO_LOW;
				}
				else
				{
					gpioHiLow = GPIO_HI;
				}
				eDrvErr = DRV_GPIO_Write(gpioId, gpioHiLow);
				DONE_ERR((eDrvErr!=DRV_OK)); 
				goto done;
			}

			break;
		case LED_FUNC_NONE:
		default:
			eDrvErr = DRV_ERR;
			break;
	}

done:
	return eDrvErr; 
}

DRV_Error DRV_CFG_LED_PlatformDrvInit(struct led_platform_driver_t *tLedDrv)
{
	DRV_Error eDrvErr = DRV_ERR;
	DONE_ERR((!tLedDrv));

	tLedDrv->led_cap = LED_CAPABILITY;
	tLedDrv->use_dimming = false;
	tLedDrv->set = set_led;

	eDrvErr = DRV_OK;
done:
	return eDrvErr;
}
#endif

#endif

