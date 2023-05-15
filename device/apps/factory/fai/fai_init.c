/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Interface
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fai_common.h"
#include "fai_channel.h"
#include "fai_audio.h"
#include "fai_video.h"
#include "fai_av.h"
#include "fai_scart.h"
#include "fai_hdmi.h"
#include "fai_rf.h"
#include "fai_flash.h"
#include "fai_ci.h"
#include "fai_sc.h"
#include "fai_ethernet.h"
#include "fai_hdd.h"
#include "fai_fan.h"
#include "fai_usb.h"
#include "fai_fkey.h"
#include "fai_panel.h"
#include "fai_irb.h"
#include "fai_mes.h"
#include "fai_system.h"
#include "fdi_init.h"
#include "fdi_panel.h"
#if defined(CONFIG_DI20)
#include "fai_bluetooth.h" //add junkh 2015.03.06
#include "fdi_bluetooth.h" //add junkh 2015.03.06
#endif

#include "fai_wifi.h" //add junkh 2015.03.06
#include "fdi_wifi.h" //add junkh 2015.03.06




/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

/*******************************************************************************
* function : FAI_Module_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_MODULE_Init()
{
	FDI_ERR_CODE eFdiErr;
	FAI_ERR_CODE eFaiErr;
	
	/* FDI_INIT : Must be initialized regardless of FAI */

#if 0//defined(CONFIG_DI10) /* only use KHD-1000T */
	eFaiErr = FAI_SYSTEM_WIFI_KO_Module_Load();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SYSTEM_WIFI_KO_Module_Load] : FAI_SYSTEM_WIFI_KO_Module_Load Error!!\n"));
	}
#endif
	eFdiErr = FDI_MODULE_Init();
	if( eFdiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_SYSTEM_Init Error!!\n"));
	}
	eFaiErr = FAI_SYSTEM_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_SYSTEM_Init Error!!\n"));
	}
	eFaiErr = FAI_CHANNEL_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_CHANNEL_Init Error!!\n"));
	}
	eFaiErr = FAI_FKEY_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_FKEY_Init Error!!\n"));
	}
	eFaiErr = FAI_RCU_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_RCU_Init Error!!\n"));
	}	
#if defined(FACTORY_USE_SMARTCARD)
	eFaiErr = FAI_SC_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_SC_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_USB)
	eFaiErr = FAI_USB_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_USB_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_FAN)
	eFaiErr = FAI_FAN_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_FAN_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_HDD)
	eFaiErr = FAI_HDD_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_HDD_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_HDMI)
	eFaiErr = FAI_HDMI_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_HDMI_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_FLASH)
	eFaiErr = FAI_FLASH_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_FLASH_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_RTC)
	eFaiErr = FAI_CLOCK_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_CLOCK_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_SCART)
	eFaiErr = FAI_SCART_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_SCART_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_ETHERNET)
	eFaiErr = FAI_ETHERNET_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_ETHERNET_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_WIFI)
	eFaiErr = FAI_WIFI_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_WIFI_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_BT)
	eFaiErr = FAI_BLUETOOTH_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_BLUETOOTH_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_CI)
	eFaiErr = FAI_CI_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_CI_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_MES)
	eFaiErr = FAI_MES_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_MES_Init Error!!\n"));
	}
#endif
	eFaiErr = FAI_AV_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_AV_Init Error!!\n"));
	}
	eFaiErr = FAI_PANEL_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_PANEL_Init Error!!\n"));
	}
	eFaiErr = FAI_VIDEO_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_VIDEO_Init Error!!\n"));
	}
	eFaiErr = FAI_AUDIO_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_AUDIO_Init Error!!\n"));
	}
		
#if defined(FACTORY_USE_MODEM)
	eFaiErr = FAI_MODEM_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_MODEM_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_RF)
	eFaiErr = FAI_RF_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_RF_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_EEPROM)
	eFaiErr = FAI_EEPROM_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_EEPROM_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_CABLEMODEM)
	eFaiErr = FAI_CABLEMODEM_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_Module_Init] : FAI_CABLEMODEM_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_IRB)
	eFaiErr = FAI_IRB_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_IRB_Init] : FAI_IRB_Init Error!!\n"));
	}
#endif
#if defined(FACTORY_USE_MMC)
	eFaiErr = FAI_MMC_Init();
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MMC_Init] : FAI_MMC_Init Error!!\n"));
	}
#endif

	FDI_PANEL_Display(FAI_TESTMODE_STR);
	
	return FAI_NO_ERR;
}

