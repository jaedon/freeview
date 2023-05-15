/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
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
#include "fdi_common.h"
#include "fdi_channel.h"
#include "fdi_uart.h"
#include "fdi_channel.h"
#include "fdi_demux.h"
#include "fdi_video.h"
#include "fdi_audio.h"
#include "fdi_osd.h"
#include "fdi_key.h"
#include "fdi_panel.h"
#include "fdi_led.h"
#include "fdi_system.h"
#include "fdi_eeprom.h"
#include "fdi_ethernet.h"
#include "fdi_hdd.h"
#include "fdi_modem.h"
#include "fdi_rf.h"
#include "fdi_sc.h"
#include "fdi_scart.h"
#include "fdi_usb.h"
#include "fdi_flash.h"
#include "fdi_hdmi.h"
#include "fdi_fan.h"

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
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MODULE_Init(void)
{
	FDI_ERR_CODE eFdiErr;
	FDI_CH_TYPE_e eType;	
	unsigned long ulNumOfDevice = 0;
	int i;
	
	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_MODULE_Init] : ++\n"));

	/* fdi uart open */
	eFdiErr = FDI_UART_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_UART_Init Error!!\n"));
	}
	
	/* fdi channel open */
	eFdiErr = FDI_CHANNEL_GetNumOfDevice(&ulNumOfDevice);
	if(eFdiErr != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_CHANNEL_GetNumOfDevice Error!!\n"));
	}

	for( i = 0; i < ulNumOfDevice; i++ )
	{
		/* get channel type */	
		eFdiErr = FDI_CHANNEL_GetCapability(i, &eType);
		
		/* open channel */
		if( eType&FDI_CH_TYPE_SAT )
		{
#if defined(FACTORY_USE_CHANNEL_SAT)
			eFdiErr = FDI_CHANNEL_S_Open(i);
#endif
		}
		else if( eType&FDI_CH_TYPE_CAB )
		{
#if defined(FACTORY_USE_CHANNEL_CAB)
			eFdiErr = FDI_CHANNEL_C_Open(i);
#endif
		}
		else if( eType&FDI_CH_TYPE_TER )
		{
#if defined(FACTORY_USE_CHANNEL_TER)
			eFdiErr = FDI_CHANNEL_T_Open(i);
#endif
		}
		if(eFdiErr != FDI_NO_ERR)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_CHANNEL_Open(%d) Error!!\n", i));
		}	
	}

	/* fdi demux open*/
	eFdiErr = FDI_DEMUX_GetNumOfDevice(&ulNumOfDevice);
	if(eFdiErr != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_DEMUX_GetNumOfDevice Error!!\n"));
	}
	
	for( i = 0; i < ulNumOfDevice; i++ )
	{
		eFdiErr = FDI_DEMUX_Open(i);
		if( eFdiErr != FDI_NO_ERR )
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_DEMUX_Open Error!!\n"));		
		}
	}

	/* fdi video open */
	eFdiErr = FDI_VIDEO_GetNumOfDevice(&ulNumOfDevice);
	if(eFdiErr != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_DEMUX_GetNumOfDevice Error!!\n"));
	}
	
	for( i = 0; i < ulNumOfDevice; i++ )
	{
		eFdiErr = FDI_VIDEO_Open(i);
		if( eFdiErr != FDI_NO_ERR )
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_VIDEO_Open Error!!\n"));		
		}
	}

	/* fdi audio open */
	eFdiErr = FDI_AUDIO_GetNumOfDevice(&ulNumOfDevice);
	if(eFdiErr != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_AUDIO_GetNumOfDevice Error!!\n"));
	}
	
	for( i = 0; i < ulNumOfDevice; i++ )
	{
		eFdiErr = FDI_AUDIO_Open(i);
		if( eFdiErr != FDI_NO_ERR )
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_AUDIO_Open Error!!\n"));		
		}
	}

	/* fdi osd open */
	eFdiErr = FDI_OSD_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_OSD_Init Error!!\n"));		
	}

	/* fdi key open */
	eFdiErr = FDI_KEY_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_KEY_Init Error!!\n"));		
	}

	/* fdi panel open */
	eFdiErr = FDI_PANEL_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_PANEL_Init Error!!\n")); 	
	}

	/* fdi led open */
	eFdiErr = FDI_LED_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_LED_Init Error!!\n"));		
	}

	/* fdi system open */
	eFdiErr = FDI_SYSTEM_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_SYSTEM_Init Error!!\n"));		
	}

#if defined(FACTORY_USE_EEPROM)
	eFdiErr = FDI_EEPROM_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_EEPROM_Init Error!!\n"));		
	}
#endif
#if defined(FACTORY_USE_ETHERNET)
	FDI_ETHERNET_GetDevCount(&ulNumOfDevice);
	for(i=0; i<ulNumOfDevice; i++)
	{
		eFdiErr = FDI_ETHERNET_Open(i);
		if( eFdiErr != FDI_NO_ERR )
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_ETHERNET_Open(%d) Error!!\n", i));
		}
	}
#endif
#if defined(FACTORY_USE_WIFI)
	eFdiErr = FDI_WIFI_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_WIFI_Open Error!!\n"));		
	}
#endif
#if defined(FACTORY_USE_HDD)
	eFdiErr = FDI_HDD_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_HDD_Init Error!!\n"));		
	}
#endif
#if defined(FACTORY_USE_MODEM)
	eFdiErr = FDI_MODEM_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_MODEM_Init Error!!\n"));		
	}
#endif
#if defined(FACTORY_USE_RF)
	eFdiErr = FDI_RF_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_RF_Init Error!!\n"));		
	}
#endif
#if defined(FACTORY_USE_SMARTCARD)
	/* fdi smart card open */
	eFdiErr = FDI_SC_GetCapability(&ulNumOfDevice);
	if(eFdiErr != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_SC_GetCapability Error!!\n"));
	}
	
	for( i = 0; i < ulNumOfDevice; i++ )
	{
		eFdiErr = FDI_SC_Open(i);
		if( eFdiErr != FDI_NO_ERR )
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_SC_Open(%d) Error!!\n", i));		
		}
	}

#endif
#if defined(FACTORY_USE_SCART)
	eFdiErr = FDI_SCART_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_SCART_Init Error!!\n"));		
	}
#endif
#if defined(FACTORY_USE_USB)
	eFdiErr = FDI_USB_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_USB_Init Error!!\n"));		
	}
#endif
#if defined(FACTORY_USE_FLASH)
#if !defined(FACTORY_USE_DI20) || (defined(FACTORY_USE_DI20) && defined(FACTORY_USE_NOR_FLASH))
	eFdiErr = FDI_FLASH_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_FLASH_Init Error!!\n"));		
	}
#endif	
#endif
#if defined(FACTORY_USE_HDMI)
	eFdiErr = FDI_HDMI_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_HDMI_Init Error!!\n"));		
	}
#endif
#if defined(FACTORY_USE_FAN)
	eFdiErr = FDI_FAN_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_FAN_Init Error!!\n"));		
	}
#endif
#if defined(FACTORY_USE_CABLEMODEM)
	eFdiErr = FDI_CABLEMODEM_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_CABLEMODEM_Init Error!!\n"));		
	}
#endif

#if defined(FACTORY_USE_RTC)
	eFdiErr = FDI_CLOCK_Open();
	if( eFdiErr != FDI_NO_ERR )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODULE_Init] : FDI_FAN_Init Error!!\n"));		
	}
#endif

	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_MODULE_Init] : --\n"));

	return FDI_NO_ERR;
}

