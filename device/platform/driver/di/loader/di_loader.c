/*
* $Header:   $
*/
/*********************************************************************
* $Workfile:   di_loader.c  $
* $Modtime:   Jul 16 2007 01:07:04  $
*
* Author:
* Description:
*
*                                 Copyright (c) 2008 HUMAX Co., Ltd.
*                                               All rights reserved.
********************************************************************/

/*********************************************************************
* Header Files
*********************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
	#include <stdio.h>
	#include <string.h>
#endif

#include "linden_trace.h"

#include "htype.h"
#include "di_err.h"
#include "di_uart.h"
#include "vkernel.h"
#include "di_loader.h"
#include "di_power.h"

#include "drv_micom.h"
#include <drv_err.h>
#include <drv_bootloader.h>

#if defined(CONFIG_OS_UCOS)
#include "cmdtool.h"
#endif
#include "ram_map.h"

#include "nexus_platform.h"
#include "bchp_aon_ctrl.h"

/*---------------------------------------------
[Scratch Register - AON_SCRATCH_PTR1]

  4     4    4    4   4    4   4   4   (bit)
--------------------------------------
| A  |  B | rsv |  OTA   | wakup | C |
|    |    |     |  type  | reason|   |
--------------------------------------
---------------------------------------------*/

#if defined(CONFIG_ARM) && defined(BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_BASE)

#define AON_SCRATCH_PTR1	(BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_BASE+(0xd * 0x4))
#define SCRATCH_PTR1		(BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_BASE+(0xe * 0x4))
#define SCRATCH_PTR2		(BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_BASE+(0xf * 0x4))

#elif defined(CONFIG_MIPS) && defined(BCHP_AON_CTRL_UNCLEARED_SCRATCH)

#include "bchp_sun_top_ctrl.h"
#include "bchp_mem_dma_0.h"

#define AON_SCRATCH_PTR1    BCHP_AON_CTRL_UNCLEARED_SCRATCH
#define SCRATCH_PTR1        BCHP_SUN_TOP_CTRL_UNCLEARED_SCRATCH
#define SCRATCH_PTR2        BCHP_MEM_DMA_0_SCRATCH /* or BCHP_SUN_GISB_ARB_
																	 SCRATCH */
#endif    /* CONFIG_ARM */

#define HUMAX_SCRATCH_MAGIC_NUM 0xAB00000C
#define HUMAX_SCRATCH_MAGIC_NUM_MASK	0xFF00000F
#define AON_SCRATCH_PTR1_OTA_MASK	0x000FF000
#define AON_SCRATCH_PTR1_OTA_SHIFT	12
#define AON_SCRATCH_WAKEUP_REASON_MASK    0x00000FF0
#define AON_SCRATCH_WAKEUP_REASON_SHIFT    4

#define SCRATCH_KEY1		0x00000000
#define SCRATCH_KEY2		0xAAAAAAAA
#define SCRATCH_KEY3		0x55555555

#define DEBUG_MODULE        TRACE_MODULE_DI_FLASH


extern Loader_ImageInfo_t loaderImageInfoTable[];

extern HUINT32 DRV_CFG_GetLoaderImageInfoMax(void);

DI_ERR_CODE DI_LOADER_GetImageInfo(Image_Usage_t etImageUsage, ImageInfo_t *pImageInfo)
{
	HUINT32 tableSize = 0;
	HUINT32 i = 0;

	VK_memset(pImageInfo, 0, sizeof(ImageInfo_t));

	tableSize = DRV_CFG_GetLoaderImageInfoMax();

	for(i=0; i<tableSize; i++)
	{
		if(etImageUsage == loaderImageInfoTable[i].etImageUsage)
		{
			VK_MEM_Memcpy(pImageInfo, &loaderImageInfoTable[i].etImageInfo, sizeof(ImageInfo_t));
			return DI_ERR_OK;
		}
	}

	DI_UART_Print("Error!!! Can not find ImageInfo_t for Image_Usage_t(%d)\n", etImageUsage);
	return DI_ERR_ERROR;
}

DI_ERR_CODE DI_LOADER_GetNagraImageInfo(Image_Nagra_Info_t etImageUsage, ImageInfo_t *pImageInfo)
{
	HAPPY(etImageUsage);
	HAPPY(pImageInfo);

#if defined(CONFIG_CAS_NA)
	VK_memset(pImageInfo, 0, sizeof(ImageInfo_t));

	switch(etImageUsage)
	{
#if defined(CONFIG_CAS_NA_DA2_BOOTING)
		case Image_Nagra_Info_DA2:
			pImageInfo->ulStoredAddress		= NAGRA_DA2_LOGICAL_OFFSET;
			pImageInfo->defaultImageSize	= NAGRA_DA2_SIZE;
			pImageInfo->ulLoadingAddress 	= NAGRA_DA2_RAM_ADDR;
			break;
#endif
		case Image_Nagra_Info_Kernel:
			pImageInfo->ulStoredAddress 	= N_IMAGE_OTA_KERNEL_OFFSET;
			pImageInfo->ulLoadingAddress 	= CRYPT_KERNEL_RAM_ADDR;
			pImageInfo->ulSignatureCheckAddress 	= CRYPT_KERNEL_RAM_ADDR;
			pImageInfo->defaultImageSize 	= N_IMAGE_KERNEL_SIZE;
			pImageInfo->ulEntryAddress 		= CRYPT_KERNEL_RAM_ADDR;
			pImageInfo->ulLoadingAddress 	= CRYPT_KERNEL_RAM_ADDR;
			pImageInfo->Image_Type 			= Image_Type_BINARY_ZIPPED;
			pImageInfo->Image_OS 			= Image_OS_Linux_Ramdisk;
			break;

		default:
			break;
	}
#endif
	return DI_ERR_OK;
}


static DrvLOADER_IMAGE_Boot_e _di_loader_ConvImageBootDi2Drv(Image_Boot_t eImgBoot)
{
	switch(eImgBoot)
	{
	case Image_Boot_Normal: 			return eDrvLOADER_IMAGE_Boot_Normal;
	case Image_Boot_NormalBackup: 		return eDrvLOADER_IMAGE_Boot_NormalBackup;
	case Image_Boot_Factory:		 	return eDrvLOADER_IMAGE_Boot_Factory;
	case Image_Boot_Ota:	 			return eDrvLOADER_IMAGE_Boot_Ota;
	case Image_Boot_OtaBackup:		 	return eDrvLOADER_IMAGE_Boot_OtaBackup;
	case Image_Boot_NormalInFactory:	return eDrvLOADER_IMAGE_Boot_NormalInFactory;

	default:
		break;
	}

	return eDrvLOADER_IMAGE_Boot_MAX;
}

DI_ERR_CODE DI_LOADER_Jump(JumpInfo_t stJumpInfo)
{
//	HCHAR	szCmd[255];
	HAPPY(stJumpInfo);
	// 임시 구현.
	if (stJumpInfo.etImage_OS == Image_OS_Linux_Ramdisk)
	{
		DRV_BLOADER_Ramboot((void *)stJumpInfo.ulJumpAddress, stJumpInfo.ulRamdiskSize, _di_loader_ConvImageBootDi2Drv(stJumpInfo.etImage_Boot));
	}
	else
	{
		DRV_BLOADER_Autobooting(_di_loader_ConvImageBootDi2Drv(stJumpInfo.etImage_Boot));
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_LOADER_InitDevice(DI_LOADER_DEVICE_E type)
{
	DI_UART_Print("%s(%d) : Do not call this function, type = %d, This function was replaced to DI_Init()\n", __FUNCTION__, __LINE__, type);
	return DI_ERR_OK;
}

static DI_ERR_CODE _di_loader_ConvWakeupReasonDrv2Di(DRV_MICOM_WAKEUP_SRC nWakeupSrc, DI_POWER_REASON *pReason)
{
	switch(nWakeupSrc)  {
		case DRV_MICOM_WAKEUP_BY_KEY:				
			*pReason = FRONT_KEY;
		    break;
		case DRV_MICOM_WAKEUP_BY_RCU:				
			*pReason = IR_NEC_TYPE;
		    break;
		case DRV_MICOM_WAKEUP_BY_TIMER:				
			*pReason = REAL_TIME_CLOCK;
		    break;
		case DRV_MICOM_WAKEUP_BY_REBOOT:			
			*pReason = SYSTEM_REBOOT;
		    break;
		case DRV_MICOM_WAKEUP_BY_RF4CE_RCU:			
			*pReason = RF4CE_RCU;
		    break;
		case DRV_MICOM_WAKEUP_BY_FAN_LOCKED:		
			*pReason = FAN_LOCKED;
		    break;
		case DRV_MICOM_WAKEUP_BY_OVER_TEMPERATURE:	
			*pReason = OVER_TEMPERATURE;
		    break;
		case DRV_MICOM_WAKEUP_BY_WOL:				
			*pReason = WAKE_ON_LAN;
		    break;
		case DRV_MICOM_WAKEUP_BY_ACPOWER:		
		default:
			*pReason = AC_ON;
		    break;
	}

	return DI_ERR_OK;
}

static DI_LOADER_DOWNLOAD_TYPE_E s_eDnTYpe = DI_LOADER_DOWNLOAD_BY_USB;
DI_ERR_CODE DI_LOADER_GetScratch(DI_LOADER_SCRATCH_TYPE_E eSrType, HUINT32 *peDnType)
{
#if defined(AON_SCRATCH_PTR1)
	HUINT32 tmp_val = 0;
	switch (eSrType)
	{
		case DI_LOADER_SCRATCH_OTA:
			NEXUS_Platform_ReadRegister(AON_SCRATCH_PTR1, &tmp_val);
			*peDnType = ((tmp_val & AON_SCRATCH_PTR1_OTA_MASK) >> AON_SCRATCH_PTR1_OTA_SHIFT);
			break;
		case DI_LOADER_SCRATCH_SBP_RESULT1:
			NEXUS_Platform_ReadRegister(SCRATCH_PTR1, &tmp_val);
			*peDnType = (tmp_val ^ SCRATCH_KEY2);
			break;
		case DI_LOADER_SCRATCH_SBP_RESULT2:
			NEXUS_Platform_ReadRegister(SCRATCH_PTR2, &tmp_val);
			*peDnType = (tmp_val ^ SCRATCH_KEY3);
			break;
		case DI_LOADER_SCRATCH_WAKEUP_REASON:
			NEXUS_Platform_ReadRegister(AON_SCRATCH_PTR1, &tmp_val);
			tmp_val = ((tmp_val & AON_SCRATCH_WAKEUP_REASON_MASK) >> AON_SCRATCH_WAKEUP_REASON_SHIFT);
			_di_loader_ConvWakeupReasonDrv2Di((DRV_MICOM_WAKEUP_SRC)tmp_val, peDnType);
			break;
		default:
			/* PrintError("[DI_LOADER_GetScratch] invalid parameter!\n"); */
			return DI_ERR_INVALID_PARAM;
	}
#endif
	return DI_ERR_OK;
}

DI_ERR_CODE DI_LOADER_SetScratch(DI_LOADER_SCRATCH_TYPE_E eSrType, HUINT32 eDnType)
{
#if defined(AON_SCRATCH_PTR1)
	HUINT32 tmp_val = 0;
	switch (eSrType)
	{
		case DI_LOADER_SCRATCH_OTA:
			NEXUS_Platform_ReadRegister(AON_SCRATCH_PTR1, &tmp_val);
			tmp_val &= ~(HUMAX_SCRATCH_MAGIC_NUM_MASK | AON_SCRATCH_PTR1_OTA_MASK);
			tmp_val |= (HUMAX_SCRATCH_MAGIC_NUM | (eDnType << AON_SCRATCH_PTR1_OTA_SHIFT));
			NEXUS_Platform_WriteRegister(AON_SCRATCH_PTR1, tmp_val);
			break;
		case DI_LOADER_SCRATCH_SBP_RESULT1:
			tmp_val = eDnType ^ SCRATCH_KEY2;
			NEXUS_Platform_WriteRegister(SCRATCH_PTR1, tmp_val);
			break;
		case DI_LOADER_SCRATCH_SBP_RESULT2:
			tmp_val = eDnType ^ SCRATCH_KEY3;
			NEXUS_Platform_WriteRegister(SCRATCH_PTR2, tmp_val);
			break;
		default:
			/* PrintError("[DI_LOADER_SetScratch] invalid parameter!\n"); */
			return DI_ERR_INVALID_PARAM;
	}
#endif
	return DI_ERR_OK;
}

DI_ERR_CODE DI_LOADER_GetDownloadType(DI_LOADER_DOWNLOAD_TYPE_E *eDnType)
{
	*eDnType = s_eDnTYpe;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_LOADER_SetDownloadType(DI_LOADER_DOWNLOAD_TYPE_E eDnType)
{
	s_eDnTYpe = eDnType;
	return DI_ERR_OK;
}

void CMD_DI_LOADER_Init(void *arg)
{
	CMD_DRV_BLOADER_Init();

	(void)arg;
}



/*
* Please do not call DI_LOADER_Init in the DI_Init!
*/
void DI_LOADER_Init(void *arg)
{
	/*
	* This function to run before the Nexus Init.
	* The only run a single thread
	* Do not call the VK Function.
	*
	*
	* Nexus Init이 호출 되기 전에 실행되는 함수 입니다.
	* 싱글 스레드에서만 실행하세요!!!
	* VK 함수를 사용하지 마세요.
	*
	*/
	HAPPY(arg);
#if defined(CONFIG_MICOM_JP)
	DRV_uMICOM_UartInit(FALSE, 0);
#endif
}

#if defined(CONFIG_OS_UCOS)
STATIC DRV_LOADER_BootOption_e	_di_loader_ConvBootOptionDi2Drv(DI_LOADER_BootOption_e 	eOption)
{
	DRV_LOADER_BootOption_e eDrvOption = eDRV_BOOTOPTION_None;

	if(eDI_BOOTOPTION_CM_MACADDR & eOption)
	{
		eDrvOption |= eDRV_BOOTOPTION_CM_MACADDR;
	}

	if(eDI_BOOTOPTION_RSA_KEY & eOption)
	{
		eDrvOption |= eDRV_BOOTOPTION_RSA_KEY;
	}

	if(eDI_BOOTOPTION_EXT_APP & eOption)
	{
		eDrvOption |= eDRV_BOOTOPTION_EXT_APP;
	}

	/* ex */
	if(eDI_BOOTOPTION_xxxx3 & eOption)
	{
		eDrvOption |= eDRV_BOOTOPTION_xxxx3;
	}

	return eDrvOption;
}

DI_ERR_CODE DI_LOADER_SetBootOption(DI_LOADER_BootOption_t *pstOption)
{
	DI_ERR_CODE hErr = ERR_OK;
	DRV_Error 	drvErr = DRV_OK;

	DRV_LOADER_BootOption_t	stDrvBootOpt;

	if(NULL == pstOption)
	{
		PrintError("[%s:%d] Invalid Argument\r\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	VK_memset(&stDrvBootOpt, 0, sizeof(DRV_LOADER_BootOption_t));

	if(NULL != pstOption->ethernetName[0])
	{
		VK_strncpy((HCHAR*)stDrvBootOpt.ethernetName, (const HCHAR*)pstOption->ethernetName, DEF_MAX_ETHNAME_LENGTH);
	}

	if(NULL != pstOption->extAppName[0])
	{
		VK_strncpy((HCHAR*)stDrvBootOpt.extAppName, (const HCHAR*)pstOption->extAppName, DEF_MAX_EXTAPPNAME_LENGTH);
	}

	stDrvBootOpt.eOption = _di_loader_ConvBootOptionDi2Drv(pstOption->eOption);

	drvErr = DRV_BLOADER_SetBootOption(&stDrvBootOpt);
	if(DRV_OK != drvErr)
	{
		hErr = ERR_FAIL;
	}

	return hErr;
}

DI_ERR_CODE DI_LOADER_GetBootOption(DI_LOADER_BootOption_t *pstOption)
{
	DI_ERR_CODE hErr = ERR_OK;
	DRV_Error 	drvErr = DRV_OK;

	drvErr = DRV_BLOADER_GetBootOption((DRV_LOADER_BootOption_t*)pstOption);
	if(DRV_OK != drvErr)
	{
		hErr = ERR_FAIL;
	}

	return hErr;
}

HBOOL DI_LOADER_IsBootwait(void)
{
	return CMD_SetKeyPressBootWait(FALSE, TRUE);
}
#endif

