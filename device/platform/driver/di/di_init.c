/**
 * di_init.c
*/

/**
 * @defgroup		DI_INIT DI_INIT : Initialize DI module
 * @author			Jin-hong Kim
 * @note			DI_INIT APIs
 * @brief			Define DI_INIT APIs
 * @file 			di_init.c
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_DEBUG) && !defined(CONFIG_OS_UCOS)
#include <sys/resource.h>
#endif
#include <sys/reboot.h>
#include "linden_trace.h"
#include "linden_monitor.h"

#include "htype.h"
#include "di_err.h"
#include "vkernel.h"

#include "di_init.h"

#include "drv_err.h"
#include "drv_micom.h"

#include "nexus_base_types.h"
#include "nexus_platform.h"
#include "nexus_dma.h"

#include "berr.h"
#include "bstd_defs.h"
#include "bdbg.h"

#include "drv_stled.h"
#include "drv_gpio.h"
#include "drv_i2c.h"
#include "drv_scart.h"
#include "drv_hdmi.h"
#include "drv_nvram.h"
#include "drv_flash.h"
#include "di_audio.h"
#include "drv_audio.h"
#include "di_video.h"
#include "drv_video.h"
#include "drv_demux.h"
#if defined(CONFIG_MEDIA)
#if defined(CONFIG_DI10)
#include "drv_media_10.h"
#else
#include "drv_media.h"
#endif
#endif
#if defined(CONFIG_CALYPSO)
#include "drv_calypso.h"
#endif
#include "drv_remocon.h"
#include "drv_pvr.h"
#include "drv_osd.h"
#include "drv_key.h"
#include "drv_fan.h"
#include "drv_ci.h"
#include "drv_usb.h"
#include "drv_fs.h"
#include "di_channel.h"
#include "di_hdd.h"
#include "drv_hdd.h"
#include "drv_hotplug.h"
#include "drv_usb_wireless.h"

#if defined (CONFIG_SMARTCARD) || defined(CONFIG_SMARTCARD_SLOT1)
#include "drv_smartcard.h"
#endif
#if defined (CONFIG_DSC)
#include "drv_dsc.h"
#endif
#if defined (CONFIG_IRB)
#include "drv_irb.h"
#endif
#if	defined(CONFIG_MODEM)
#include "drv_modem.h"
#endif
#if defined(CONFIG_CABLE_MODEM)
#include "di_cm.h"
#if !defined(CONFIG_OS_UCOS)
#include "di_snmp.h"
#endif
#endif
#if defined(CONFIG_SYNC_CHANNEL) || defined(CONFIG_ASTM) || defined(CONFIG_SYNC_RECOVERY)
#include "drv_sync_common.h"
#endif
#if defined(CONFIG_SPI)
#include "drv_spi.h"
#endif
#include "drv_nvram.h"

#if defined(CONFIG_TRANSCODER)
#include "di_transcoder.h"
#include "drv_transcoder.h"
#endif

#if defined(CONFIG_RF4CE)
#include "drv_rf4ce.h"
#endif

#if defined(CONFIG_DINETWORK)
#if defined(CONFIG_ETHERNET)
#include "drv_network.h"
#endif

#if defined(CONFIG_WIFI)
#include "drv_wifi.h"
#endif
#if defined(CONFIG_BLUETOOTH)
#include "drv_bt.h"
#endif

#endif

#if defined(CONFIG_BCM_UART)
#include "drv_uart.h"
#endif

#if defined(CONFIG_MMC)
#include "drv_mmc.h"
#endif

#include "drv_rfvod.h"

#if defined(CONFIG_WATCHDOG)
#include "drv_watchdog.h"
#endif

#if (NEXUS_VERSION >= 1290) /* for playready */
#if defined (CONFIG_PLAYREADY)
/* MEDIA 1.0 에는 drv_drm.h 헤더 파일 존재하지 않음. */
#if defined (CONFIG_MEDIA20)
#include "drv_drm.h"
#include "iptuner/transport_tls.h"
#endif
#endif
#endif

#if defined (CONFIG_IPTUNER)
#include "di_iptuner.h"
#endif

#include "drv_crypt.h"

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
#include "drv_pm.h"
#endif

#if defined(CONFIG_MIC)
#include "drv_mic.h"
#endif

#include "drv_led.h"

#if defined(CONFIG_OS_UCOS)
#include "drv_bootloader.h"
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
#undef CONFIG_DIRECTFB
#endif

#define DEBUG_MODULE	TRACE_MODULE_DI_INIT

#if defined(CONFIG_DEBUG)
#define	BCM_MEMORY_DEBUG
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
NEXUS_DmaHandle g_ulDmaHandle = NULL;

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
#if defined (CONFIG_CABLE_MODEM)
#if !defined(CONFIG_OS_UCOS)
extern void SetSignalMask(void);
#endif
#endif
extern DRV_Error DRV_RF4CE_Init(void);
extern DRV_Error DRV_SYSTEM_Init(void);
extern DRV_Error DRV_CFG_PlatformInit(void);

#if defined (CONFIG_MEDIA20)
#if (NEXUS_VERSION >= 1290) /* for playready */
#if defined (CONFIG_PLAYREADY)
extern int TLS_Openssl_Init(void);
#endif
#endif
#endif
DRV_Error SHARED_DI_Init(void);

#if defined (CONFIG_NTFS_RW)
#define NTFS_MODULE 						"/lib/modules/drivers/fs/ntfs/tntfs.ko"
#endif
void P_Device_Insmod(void)
{
#if defined (CONFIG_NTFS_RW)
	char cmd[64];

	VK_snprintf(cmd, sizeof(cmd) - 1, "insmod %s", NTFS_MODULE);
	PrintDebug("[%d] [%s] [%s] cmd = %s\n", __LINE__, __FUNCTION__, __FILE__, cmd);
	VK_SYSTEM_Command(cmd);
#endif
}

void P_Device_Rmmod(void)
{
#if defined (CONFIG_NTFS_RW)
	char cmd[64];

	VK_snprintf(cmd, sizeof(cmd) - 1, "rmmod tntfs");
	PrintDebug("[%d] [%s] [%s] cmd = %s\n", __LINE__, __FUNCTION__, __FILE__, cmd);
	VK_SYSTEM_Command(cmd);
#endif
}

static void P_InitDeviceModules(void)
{
	P_Device_Insmod();
}

static void P_InitTraceModules(void)
{

/* VK Module */
	TraceSetModule( TRACE_MODULE_VK);
	TraceSetLevel( TRACE_MODULE_VK,	TRACE_LEVEL_ERROR );

	TraceSetModule( TRACE_MODULE_VK_CLOCK);
	TraceSetLevel( TRACE_MODULE_VK_CLOCK, TRACE_LEVEL_ERROR );


	TraceSetModule( TRACE_MODULE_VK_EVENT);
	TraceSetLevel( TRACE_MODULE_VK_EVENT, TRACE_LEVEL_ERROR );


	TraceSetModule( TRACE_MODULE_VK_MEM);
	TraceSetLevel( TRACE_MODULE_VK_MEM, TRACE_LEVEL_ERROR );


	TraceSetModule( TRACE_MODULE_VK_MSG);
	TraceSetLevel( TRACE_MODULE_VK_MSG, TRACE_LEVEL_ERROR );


	TraceSetModule( TRACE_MODULE_VK_SEM);
	TraceSetLevel( TRACE_MODULE_VK_SEM, TRACE_LEVEL_ERROR );

	TraceSetModule( TRACE_MODULE_VK_TASK);
	TraceSetLevel( TRACE_MODULE_VK_TASK, TRACE_LEVEL_ERROR );

	TraceSetModule( TRACE_MODULE_VK_TIMER);
	TraceSetLevel( TRACE_MODULE_VK_TIMER, TRACE_LEVEL_ERROR );


/* DI Module	*/
	TraceSetModule( TRACE_MODULE_DI_AUDIO);
	TraceSetLevel( TRACE_MODULE_DI_AUDIO,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_CHANNEL);
	TraceSetLevel( TRACE_MODULE_DI_CHANNEL,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_CI);
	TraceSetLevel( TRACE_MODULE_DI_CI,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_DEMUX);
	TraceSetLevel( TRACE_MODULE_DI_DEMUX,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_FAN);
	TraceSetLevel( TRACE_MODULE_DI_FAN,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_FLASH);
	TraceSetLevel( TRACE_MODULE_DI_FLASH,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_HDMI);
	TraceSetLevel( TRACE_MODULE_DI_HDMI,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_KEY);
	TraceSetLevel( TRACE_MODULE_DI_KEY,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_LED);
	TraceSetLevel( TRACE_MODULE_DI_LED,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_OSD);
	TraceSetLevel( TRACE_MODULE_DI_OSD,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_PVR);
	TraceSetLevel( TRACE_MODULE_DI_PVR,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_SCART);
	TraceSetLevel( TRACE_MODULE_DI_SCART,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_SC);
	TraceSetLevel( TRACE_MODULE_DI_SC,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_VIDEO);
	TraceSetLevel( TRACE_MODULE_DI_VIDEO,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DI_INIT);
	TraceSetLevel( TRACE_MODULE_DI_INIT,	TRACE_LEVEL_ERROR);

/* DRV Module	*/
	TraceSetModule( TRACE_MODULE_DRV_GPIO);
	TraceSetLevel( TRACE_MODULE_DRV_GPIO,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_HDMI);
	TraceSetLevel( TRACE_MODULE_DRV_HDMI,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_I2C);
	TraceSetLevel( TRACE_MODULE_DRV_I2C,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_KEYPAD);
	TraceSetLevel( TRACE_MODULE_DRV_KEYPAD,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_LNB);
	TraceSetLevel( TRACE_MODULE_DRV_LNB,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_MICOM);
	TraceSetLevel( TRACE_MODULE_DRV_MICOM,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_PWM);
	TraceSetLevel( TRACE_MODULE_DRV_PWM,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_REMOCON);
	TraceSetLevel( TRACE_MODULE_DRV_REMOCON,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_SCART);
	TraceSetLevel( TRACE_MODULE_DRV_SCART,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_UART);
	TraceSetLevel( TRACE_MODULE_DRV_UART,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_MISC);
	TraceSetLevel( TRACE_MODULE_DRV_MISC,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_HOTPLUG);
	TraceSetLevel( TRACE_MODULE_DRV_HOTPLUG,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_SYNC);
	TraceSetLevel( TRACE_MODULE_DRV_SYNC,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_DRM);
	TraceSetLevel( TRACE_MODULE_DRV_DRM,	TRACE_LEVEL_ERROR);

	TraceSetModule( TRACE_MODULE_DRV_CSD);
	TraceSetLevel( TRACE_MODULE_DRV_CSD,	TRACE_LEVEL_ERROR);

/* MON Module - always info */
	TraceSetModule( TRACE_MODULE_MON);
	TraceSetLevel( TRACE_MODULE_MON,	TRACE_LEVEL_INFO);


}

//#define DEBUG_INIT_TIMER
#if defined(DEBUG_INIT_TIMER)
static void P_InitTimerCallback(unsigned long ulTimerId, void *pvParam)
{
	HAPPY(ulTimerId);
	HAPPY(pvParam);
	PrintError("[P_InitTimerCallback] reboot\n");
	VK_SYSTEM_Command("reboot");
}
#endif

DI_ERR_CODE DI_Init(void)
{
//	NEXUS_PlatformSettings platformSettings;

//	NEXUS_Error	eNError		= NEXUS_SUCCESS;
	DRV_Error	eDrvError	= DRV_OK;
#if defined (CONFIG_IPTUNER)
	DI_ERR_CODE eDiError = DI_ERR_OK;
#endif

#if defined(CONFIG_CABLE_MODEM)
#if !defined(CONFIG_OS_UCOS)
	//VK_printf("SetSignalMask!!\n");
	SetSignalMask();
#endif
#endif

#if defined(CONFIG_DEBUG)
#if defined(CONFIG_OS_LINUX)
	{
		struct rlimit rlim;
		rlim.rlim_cur = RLIM_INFINITY;
		rlim.rlim_max = RLIM_INFINITY;
		setrlimit(RLIMIT_CORE, &rlim);
	}
#endif
#endif

	eDrvError = VK_Init();
	if(eDrvError != VK_OK)
	{
		PrintError("%s->VK_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->VK_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	/* Initialize Trace */
	TraceInit();
	PrintDebug("%s->TraceInit(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);

	P_InitTraceModules();
	PrintDebug("%s->MAIN_SetTraceModules(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);

	/* Initialize Monitor */
	/* Init & Register Monitor Menu Modules */
	lmInit_Monitor();
#ifdef DEBUG_MONITOR
	lmRegister_Monitor( "TRACE",	gaTraceMonitorMenu );
#endif
	PrintDebug("%s->MON_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);

#if defined(CONFIG_OS_UCOS)
	eDrvError=DRV_BLOADER_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_BLOADER_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_BLOADER_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	eDrvError=DRV_FS_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_FS_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_FS_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

#endif

#if !defined(CONFIG_DI_SHARED_LIB) && defined(CONFIG_OS_LINUX)
	eDrvError=DRV_HOTPLUG_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_HOTPLUG_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_HOTPLUG_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
	(void)NEXUS_Platform_Join();
#else
#if defined(CONFIG_DIRECTFB_MASTER)
#else
	eDrvError=DRV_CFG_PlatformInit();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_CFG_PlatformInit(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_CFG_PlatformInit(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#if defined (CONFIG_MEDIA20)
#if (NEXUS_VERSION >= 1290) /* for playready */
#if defined (CONFIG_PLAYREADY)
	(void)TLS_Openssl_Init();
	DRV_DRM_RegisterPostCallback();
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	DRV_DRM_RegisterSecureClockCallback();
#endif
#endif
#endif
#endif

#endif
#if defined(CONFIG_DIRECTFB)
	eDrvError  = DRV_OSD_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError( "%s->DRV_OSD_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_OSD_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	DRV_PM_Init();
#endif

//	BDBG_SetModuleLevel("BSCD", BDBG_eMsg);
//	BDBG_SetModuleLevel("BHSM", BDBG_eMsg);
//	BDBG_SetModuleLevel("nexus_astm", BDBG_eMsg);
//	BDBG_SetLevel(BDBG_eErr);

	g_ulDmaHandle = NEXUS_Dma_Open(0, NULL);
	if(g_ulDmaHandle == NULL)
	{
		PrintError("%s->NEXUS_Dma_Open(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->NEXUS_Dma_Open(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	eDrvError = DRV_CRYPT_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_CRYPT_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_CRYPT_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	eDrvError = DRV_GPIO_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_GPIO_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_GPIO_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	eDrvError = DRV_LED_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_LED_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_LED_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

#if defined(CONFIG_BCM_UART)
	eDrvError = DRV_UART_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_UART_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_UART_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

	eDrvError = DRV_I2c_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_I2c_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_I2c_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	eDrvError = DRV_MICOM_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_MICOM_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_MICOM_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

#if defined(CONFIG_STLED)
	eDrvError = DRV_STLED_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_FRONTPANEL_PANEL_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_FRONTPANEL_PANEL_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_CHANNEL)
	eDrvError = MAIN_CH_InitChannel();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->MAIN_CH_InitChannel(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->MAIN_CH_InitChannel(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#if defined(CONFIG_NUM_OF_SAT_CHANNEL) && defined(CONFIG_WORKAROUND_CHANNEL_INPUT_SOURCE)
	DI_CH_S_SetInputSource(1, DI_CH_RF_INPUT1);
#endif
#endif

#if defined(CONFIG_EEPROM)
	eDrvError = DRV_EEPROM_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_EEPROM_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_EEPROM_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_NOR_FLASH)
	eDrvError = DRV_FLASH_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_FLASH_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_FLASH_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#if defined(CONFIG_NAND_FLASH)
	eDrvError = DRV_NANDFLASH_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_NANDFLASH_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_NANDFLASH_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#if defined(CONFIG_EMMC_FLASH)
	eDrvError = DRV_EMMC_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_EMMC_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_EMMC_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

	eDrvError = DRV_NVRAM_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_NVRAM_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_NVRAM_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

#if !defined(CONFIG_LAUNCHER) || defined(CONFIG_UOCTO_DOWNLOADER /*For the case of Hx Loader 3.2 */)
	eDrvError  = DRV_DEMUX_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_DEMUX_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_DEMUX_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

	eDrvError  = DRV_RFVOD_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_RFVOD_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_RFVOD_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

#if defined(CONFIG_AUDIO_DECODER)
	eDrvError  = DRV_AUDIO_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DI_AUDIO_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DI_AUDIO_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

	eDrvError  = DRV_VIDEO_Init(0);
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DI_VIDEO_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DI_VIDEO_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

#if defined(CONFIG_PIP)
	eDrvError  = DRV_VIDEO_Init(1);
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DI_VIDEO_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DI_VIDEO_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_SYNC_CHANNEL) || defined(CONFIG_ASTM) || defined(CONFIG_SYNC_RECOVERY)
	eDrvError  = DRV_SYNC_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DI_SYNC_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DI_SYNC_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

	eDrvError = DRV_HDMI_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DI_HDMI_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DI_HDMI_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}


	P_InitDeviceModules();

#if defined(CONFIG_DINETWORK)
#if defined(CONFIG_WIFI)
#if defined(CONFIG_DI_SHARED_LIB)
	// do not init Wi-Fi
#elif defined(CONFIG_DI_MODULE_WIFI_SHARED_LIB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	// do not init Wi-Fi
#elif defined(CONFIG_SUPPORT_SECDMA) && defined(CONFIG_WIFI_SCRIPT_TEST)
	// do not init Wi-Fi
#else
	eDrvError= DRV_WIFI_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_WIFI_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_WIFI_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#endif
#endif

#if	defined(CONFIG_HDD_SMART)
#if defined(CONFIG_DI_SHARED_LIB)
	// do not init usb
#elif defined(CONFIG_DI_MODULE_HDD_SHARED_LIB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	// do not init usb
#else
	eDrvError = DRV_HDD_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_HDD_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_HDD_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#endif
#if	defined(CONFIG_RECORD)
	eDrvError = DRV_PVR_REC_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_PVR_REC_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_PVR_REC_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if	defined(CONFIG_PLAYBACK) || defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
	eDrvError = DRV_PVR_PLAY_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_PVR_PLAY_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_PVR_PLAY_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_PVR_EDIT)
	eDrvError = DRV_PVR_EDIT_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_PVR_EDIT_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_PVR_EDIT_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if !defined(CONFIG_DIRECTFB)
	eDrvError  = DRV_OSD_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError( "%s->DRV_OSD_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DRV_OSD_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_SCART)
	eDrvError = DRV_SCART_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DI_Scart_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		goto errr_reboot;
	}
	else
	{
		PrintDebug("%s->DI_Scart_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_MEDIA)
	eDrvError =  DRV_MEDIA_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_MEDIA_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_MEDIA_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_CALYPSO)
	eDrvError =  DRV_CSO_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_CSO_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_CSO_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if !defined(CONFIG_PRODUCT_MULTI_PROCESS)
	eDrvError = DRV_KEY_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DI_KEY_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DI_KEY_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_RCU_INTERNAL)
	eDrvError = DRV_REMOCON_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_REMOCON_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_REMOCON_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#elif defined(CONFIG_RCU_MICOM)
	PrintDebug("%s->RCU_MICOM(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
#else
#error CONFIG_RCU_INTERNAL or CONFIG_RCU_MICOM is not defined.
#endif/*CONFIG_RCU_INTERNAL*/

#if defined(CONFIG_RF4CE)
	eDrvError=DRV_RF4CE_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_RF4CE_Init() : Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_RF4CE_Init() : OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#if	defined(CONFIG_IRB)
	eDrvError = DRV_IRB_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_IRB_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_IRB_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if	defined(CONFIG_MODEM)
	eDrvError = DRV_MODEM_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_MODEM_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_MODEM_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if	defined(CONFIG_FAN) && !defined(CONFIG_DI_SHARED_LIB)
	eDrvError = DRV_FAN_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_FAN_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_FAN_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_SPI)
	eDrvError = DRV_SPI_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_SPI_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_SPI_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_SMARTCARD_SLOT1)	// Only use slot1. Don.t support multi smartcard.
	eDrvError = DRV_SC_Init(1);
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_SC_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_SC_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#else
#if defined (CONFIG_SMARTCARD)
	eDrvError = DRV_SC_Init(0);
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_SC_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_SC_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_MULTI_SMARTCARD)
	eDrvError = DRV_SC_Init(1);
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_SC_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_SC_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#endif

#if defined(CONFIG_CI)
	eDrvError = DRV_CI_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_CI_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_CI_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined (CONFIG_DSC)
	eDrvError = DRV_DSC_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_DSC_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_DSC_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_CABLE_MODEM)
	eDrvError = DI_CM_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DI_CM_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DI_CM_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

#if !defined(CONFIG_OS_UCOS)
	eDrvError = DI_SNMP_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DI_SNMP_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DI_SNMP_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#endif

#if defined(CONFIG_MIC)
	eDrvError = DRV_MIC_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_MIC_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_MIC_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

#endif

#if defined(CONFIG_USB)
#if defined(CONFIG_DI_SHARED_LIB)
	// do not init usb
#elif defined(CONFIG_DI_MODULE_USB_SHARED_LIB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	// do not init usb
#else
	eDrvError = DRV_USB_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_USB_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_USB_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#endif

#if defined(CONFIG_USB_HID)
#if defined(CONFIG_DI_SHARED_LIB)
	// do not init usb hid
#elif defined(CONFIG_DI_MODULE_USB_SHARED_LIB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	// do not init usb hid
#else
	eDrvError = DRV_USB_HID_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_USB_HID_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_USB_HID_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#endif

#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
	extern DRV_Error DRV_FACTORY_Init (void);
	eDrvError = DRV_FACTORY_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_FAC_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_FAC_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

	eDrvError = DRV_SYSTEM_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_SYSTEM_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_SYSTEM_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

#if defined(CONFIG_WIRELESS_USB)
#if defined(CONFIG_DI_SHARED_LIB)
	// do not init usb wireless
#elif defined(CONFIG_DI_MODULE_USB_SHARED_LIB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	// do not init usb wireless
#elif !defined(CONFIG_DINETWORK)
	/* Should be removed after build up DI Network */
	eDrvError=DRV_USB_WIRELESS_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_USB_WIRELESS_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_USB_WIRELESS_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#endif

#if defined(CONFIG_TRANSCODER)
	eDrvError=DRV_TRANSCODER_Init(0);
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_TRANSCODER_Init(0) : Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_TRANSCODER_Init(0) : OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
	eDrvError=DRV_TRANSCODER_Init(1);
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_TRANSCODER(1): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_TRANSCODER(1): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_DINETWORK)
#if defined(CONFIG_DI_MODULE_NETWORK_SHARED_LIB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	// do not init usb
#elif defined(CONFIG_ETHERNET)
	eDrvError= DRV_NETWORK_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_NETWORK_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_NETWORK_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#if defined(CONFIG_BLUETOOTH)
#if defined(CONFIG_DI_SHARED_LIB)
	// do not init Bluetooth
#elif defined(CONFIG_DI_MODULE_BT_SHARED_LIB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	// do not init Bluetooth
#else
	eDrvError= DRV_BT_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_BT_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_BT_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#endif
#endif

#if !defined(CONFIG_DI_SHARED_LIB) && defined(CONFIG_OS_LINUX)
	eDrvError=DRV_HOTPLUG_ReadSysUsb();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_HOTPLUG_ReadSysUsb(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_HOTPLUG_ReadSysUsb(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_MMC)
#if defined(CONFIG_DI_SHARED_LIB)
	// do not init usb
#elif defined(CONFIG_DI_MODULE_MMC_SHARED_LIB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	// do not init usb
#else
	eDrvError = DRV_MMC_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DI_MMC_Init() : Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DI_MMC_Init() : OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#endif

#if defined(CONFIG_WATCHDOG)
	eDrvError = DRV_DOG_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_DOG_Init() : Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_DOG_Init() : OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#if defined(CONFIG_PRODUCT_IMAGE_DEBUG) || defined(CONFIG_DEVELOP)
	/* Disable watchdog when compiling debug mode. */
	VK_TASK_Sleep(100);
	eDrvError = DRV_DOG_Enable(0);
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_DOG_Enable() : Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_DOG_Enable() : OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_WATCHDOG_TIME) && (CONFIG_WATCHDOG_TIME >= 10)
	eDrvError = DRV_DOG_SetTimeout(CONFIG_WATCHDOG_TIME);
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_DOG_SetTimeout(%d) : Error Code(%d), Line(%d), %s\n", CONFIG_WATCHDOG_TIME, __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_DOG_SetTimeout(%d) : OK! Line(%d), %s\n", CONFIG_WATCHDOG_TIME, __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#endif
#if defined(CONFIG_HDMI_REFACTORING) && defined (CONFIG_HDMI_IN)
	eDrvError = DRV_HDMI_Sync_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_HDMI_Sync_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_HDMI_Sync_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_DI_SHARED_LIB) && defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	eDrvError = SHARED_DI_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->SHARED_DI_Init() : Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->SHARED_DI_Init() : OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined (CONFIG_IPTUNER)
	eDiError = DI_IPTUNER_Init();
	if(eDiError != DI_ERR_OK)
	{
		PrintError("%s->DI_IPTUNER_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDiError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DI_IPTUNER_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

	PrintError("[%s] End of DI_Init\n", __FUNCTION__);
#if defined(DEBUG_INIT_TIMER)
	{
		HUINT32 initTimerId;

		VK_TIMER_EventAfter(10*1000, P_InitTimerCallback, NULL, 0, (unsigned long *)&initTimerId);
	}
#endif

	errr_reboot:
	if(eDrvError != DRV_OK)
	{
		reboot(RB_AUTOBOOT);
	}

	return DI_ERR_OK;
}

#if defined(CONFIG_DI_SHARED_LIB) && defined(CONFIG_PRODUCT_IMAGE_HWTEST)
DRV_Error SHARED_DI_Init(void)
{
	DRV_Error	eDrvError	= DRV_OK;

#if defined(CONFIG_OS_LINUX)
	eDrvError=DRV_HOTPLUG_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_HOTPLUG_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_HOTPLUG_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#if	defined(CONFIG_RECORD) && defined(CONFIG_HDD_SMART)
	eDrvError = DRV_HDD_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_HDD_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_HDD_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
# if defined(CONFIG_FAN)
	eDrvError = DRV_FAN_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_FAN_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_FAN_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#if defined(CONFIG_USB)
	eDrvError = DRV_USB_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_USB_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_USB_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_USB_HID)
	eDrvError = DRV_USB_HID_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_USB_HID_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_USB_HID_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif
#if defined(CONFIG_WIRELESS_USB)
	/* Should be removed after build up DI Network */
	eDrvError=DRV_USB_WIRELESS_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_USB_WIRELESS_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_USB_WIRELESS_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_OS_LINUX)
	eDrvError=DRV_HOTPLUG_ReadSysUsb();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_HOTPLUG_ReadSysUsb(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_HOTPLUG_ReadSysUsb(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

#if defined(CONFIG_MMC)
	eDrvError = DRV_MMC_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DI_MMC_Init() : Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DI_MMC_Init() : OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
#endif

	return DRV_OK;
}
#endif

DI_ERR_CODE DI_PLATFORM_Init(void)
{
	DRV_Error	eDrvError		= DRV_OK;
	eDrvError=DRV_CFG_PlatformInit();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_CFG_PlatformInit(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		return DI_ERR_ERROR;
	}
	else
	{
		PrintDebug("%s->DRV_CFG_PlatformInit(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

#if defined (CONFIG_MEDIA20)
#if (NEXUS_VERSION >= 1290) /* for playready */
#if defined (CONFIG_PLAYREADY)
	(void)TLS_Openssl_Init();
	DRV_DRM_RegisterPostCallback();
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	DRV_DRM_RegisterSecureClockCallback();
#endif
#endif
#endif
#endif
	return DI_ERR_OK;
}

#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
DI_ERR_CODE DI_Join(void)
{
	NEXUS_Error	eNError		= NEXUS_SUCCESS;
	DRV_Error	eDrvError	= DRV_OK;
	NEXUS_PlatformSettings platformSettings;

	eDrvError = VK_Init();
	if(eDrvError != VK_OK)
	{
		PrintError("%s->VK_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		return DI_ERR_ERROR;
	}
	else
	{
		PrintDebug("%s->VK_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	/* Initialize Trace */
	TraceInit();
	PrintDebug("%s->TraceInit(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);

	P_InitTraceModules();
	PrintDebug("%s->MAIN_SetTraceModules(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);

	/* Initialize Monitor */
	/* Init & Register Monitor Menu Modules */
	lmInit_Monitor();
#ifdef DEBUG_MONITOR
	lmRegister_Monitor( "TRACE",	gaTraceMonitorMenu );
#endif
	PrintDebug("%s->MON_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);

	(void)NEXUS_Platform_Join();

	eDrvError  = DRV_OSD_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError( "%s->DRV_OSD_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
		return DI_ERR_ERROR;
	}
	else
	{
		PrintDebug("%s->DRV_OSD_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	eDrvError = DRV_KEY_Init();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DI_KEY_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DI_KEY_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	eDrvError = DRV_REMOCON_Join();
	if(eDrvError != DRV_OK)
	{
		PrintError("%s->DRV_REMOCON_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDrvError, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_REMOCON_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
}
#endif



