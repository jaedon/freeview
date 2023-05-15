/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: drv_cm.c
// Original Author:
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.								*/
/* All rights reserved. 												*/
/*******************************************************************/

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "vkernel.h"
#include "taskdef.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <sys/ioctl.h>
#include <netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include <net/if.h>
#include <arpa/inet.h>	/* inet(3) functions */
#include <sys/socket.h>	/* basic socket definitions */
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

/* di headers */
#include "di_channel_priv.h"
#include "drv_cm_brpc.h"
#include "di_cm.h"
#include "drv_err.h"
#include "di_snmp.h"
#include "drv_network.h"


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define CM_UNUSED(x)  { volatile void *pvUnused; pvUnused = (void *)&(x); }

#define CM_REBOOT_MONITOR	500
#define DRV_CM_TASK_PRIORITY		USER_PRIORITY5
#define DRV_CM_TASK_STACK_SIZE		BASIC_TASK_STACK_SIZE


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
extern int show_di_cm;
extern int g_nCmRebootNotify;

extern DRV_NETWORK_DEV_INFO_t s_stDevList[];

extern DI_SNMP_Information_t g_stSnmpInfo;


/*******************************************************************/
/************************ Extern functions *************************/
/*******************************************************************/
extern HUINT32 DRV_CFG_GetNetMaxDevice(void);


/******************************************************************************
 * Local(static) Variables
 ******************************************************************************/
static int s_nCmRebootNotifyDone;
static DRV_CM_CALLBACK s_pfCmCallback[DRV_CM_EVENT_END];

static HUINT8 s_ulSTBResetStatus = 0;
static HUINT8 s_ulFactoryResetStatus = 0;
static HUINT8 s_ulStbChangeState = 0;

static HINT32 s_nCmUpgradeOk = 0;


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
static void P_CM_RebootMonitorTask(void* param)
{
	param=param;
	VK_TASK_Sleep(CM_REBOOT_MONITOR);

	while(1)
	{
		VK_TASK_Sleep(CM_REBOOT_MONITOR);

		if ((g_nCmRebootNotify == 1) && (s_nCmRebootNotifyDone == 0))
		{
			if (s_pfCmCallback[DRV_CM_EVENT_REBOOT] != NULL)
			{
				CM_DI_Print(0, ("[%s][%d] s_pfCmCallback[DRV_CM_EVENT_REBOOT] have to be called!! \n", __FUNCTION__, __LINE__));
				(*s_pfCmCallback[DRV_CM_EVENT_REBOOT])(NULL);
				s_nCmRebootNotifyDone = 1;
			}
			else
			{
				CM_DI_Print(0, ("[%s][%d] s_pfCmCallback[DRV_CM_EVENT_REBOOT] is NULL!! \n", __FUNCTION__, __LINE__));
			}
		}
	}
}


/*****************************************************************************
* function : DRV_CM_Brpc_GetEcmIp
* description : Get eCM IP address
* input :
*   pulEcmIp : pointer for storing eCM IP address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetEcmIp(HUINT32 *pulEcmIp)
{
	CM_UNUSED(pulEcmIp);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}


/*****************************************************************************
* function : DRV_CM_Brpc_GetEcmMac
* description : Get eCM MAC address
* input :
*   pucEcmMac : pointer for storing eCM MAC address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetEcmMac(HUINT8 *pucEcmMac)
{
	CM_UNUSED(pucEcmMac);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_SetEcmMac
* description : Set eCM MAC address
* input :
*   pucEcmMac : pointer for storing eCM MAC address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetEcmMac(HUINT8 *pucEcmMac)
{
	CM_UNUSED(pucEcmMac);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetNetmask
* description : Get eCM subnet mask
* input :
*   pulCmNetmask : pointer for storing eCM subnet mask
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetNetmask(HUINT32 *pulCmNetmask)
{
	CM_UNUSED(pulCmNetmask);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetGateway
* description : Get eCM gateway IP address
* input :
*   pulCmGateway : pointer for storing eCM gateway IP address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetGateway(HUINT32 *pulCmGateway)
{
	CM_UNUSED(pulCmGateway);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetSwVer
* description : Get eCM SW version
* input :
*   pucCmSwVer : pointer for storing eCM SW version
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetSwVer(HUINT8 *pucCmSwVer)
{
	CM_UNUSED(pucCmSwVer);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetCmRegStatus
* description : Get eCM registration status
* input :
*   pnRegStatus : pointer for storing eCM registration status
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetCmRegStatus(HINT32 *pnRegStatus)
{
	CM_UNUSED(pnRegStatus);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsFreq
* description : Get eCM downstream frequency
* input :
*   pnDsFreq : pointer for storing eCM downstream frequency
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsFreq(HINT32 nDsId, HINT32 *pnDsFreq)
{
	CM_UNUSED(nDsId);
	CM_UNUSED(pnDsFreq);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsPower
* description : Get eCM downstream RF power level
* input :
*   nDsId : downstream channel index
*   pnDsPower : pointer for storing eCM downstream RF power level
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsPower(HINT32 nDsId, HINT32 *pnDsPower)
{
	CM_UNUSED(nDsId);
	CM_UNUSED(pnDsPower);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsSnr
* description : Get eCM downstream SNR
* input :
*   nDsId : downstream channel index
*   pnDsSnr : pointer for storing eCM downstream SNR
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsSnr(HINT32 nDsId, HINT32 *pnDsSnr)
{
	CM_UNUSED(nDsId);
	CM_UNUSED(pnDsSnr);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsBer
* description : Get eCM downstream BER
* input :
*   nDsId : downstream channel index
*   pnDsBer : pointer for storing eCM downstream BER
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsBer(HINT32 nDsId, HINT32 *pnDsBer)
{
	CM_UNUSED(nDsId);
	CM_UNUSED(pnDsBer);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsChannelId
* description : Get eCM downstream channel ID
* input :
*   nDsId : downstream channel index
*   pnDsChId : pointer for storing eCM downstream channel ID
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsChannelId(HINT32 nDsId, HINT32 *pnDsChId)
{
	CM_UNUSED(nDsId);
	CM_UNUSED(pnDsChId);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsModulation
* description : Get eCM downstream modulation
* input :
*   nDsId : downstream channel index
*   pnDsMod : pointer for storing eCM downstream modulation
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsModulation(HINT32 nDsId, HINT32 *pnDsMod)
{
	CM_UNUSED(nDsId);
	CM_UNUSED(pnDsMod);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsCorrected
* description : Get eCM downstream corrected error count
* input :
*   nDsId : downstream channel index
*   pnDsCorrected : pointer for storing eCM downstream corrected error count
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsCorrected(HINT32 nDsId, HINT32 *pnDsCorrected)
{
	CM_UNUSED(nDsId);
	CM_UNUSED(pnDsCorrected);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsUncorrected
* description : Get eCM downstream uncorrected error count
* input :
*   nDsId : downstream channel index
*   pnDsUncorrected : pointer for storing eCM downstream uncorrected error count
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsUncorrected(HINT32 nDsId, HINT32 *pnDsUncorrected)
{
	CM_UNUSED(nDsId);
	CM_UNUSED(pnDsUncorrected);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsSignalInfo
* description : Get eCM downstream signal information
* input :
*   nDsId : downstream channel index
*   pstDsInfo : pointer for storing eCM downstream signal information
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsSignalInfo(HINT32 nDsId, DI_CM_DsSignalQuality_t *pstDsInfo)
{
	CM_UNUSED(nDsId);
	CM_UNUSED(pstDsInfo);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsFreq
* description : Get eCM upstream frequency
* input :
*   nUsId : upstream channel index
*   pnUsFreq : pointer for storing eCM upstream frequency
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsFreq(HINT32 nUsId, HINT32 *pnUsFreq)
{
	CM_UNUSED(nUsId);
	CM_UNUSED(pnUsFreq);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsPower
* description : Get eCM upstream RF power level
* input :
*   nUsId : upstream channel index
*   pnUsPower : pointer for storing eCM upstream RF power level
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsPower(HINT32 nUsId, HINT32 *pnUsPower)
{
	CM_UNUSED(nUsId);
	CM_UNUSED(pnUsPower);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsRangingStatus
* description : Get eCM upstream ranging status
* input :
*   nUsId : upstream channel index
*   pnUsRanging : pointer for storing eCM upstream ranging status
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsRangingStatus(HINT32 nUsId, HINT32 *pnUsRanging)
{
	CM_UNUSED(nUsId);
	CM_UNUSED(pnUsRanging);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsChannelId
* description : Get eCM upstream channel ID
* input :
*   nUsId : upstream channel index
*   pnUsChId : pointer for storing eCM upstream channel ID
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsChannelId(HINT32 nUsId, HINT32 *pnUsChId)
{
	CM_UNUSED(nUsId);
	CM_UNUSED(pnUsChId);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsModulation
* description : Get eCM upstream modulation
* input :
*   nUsId : upstream channel index
*   pnUsMod : pointer for storing eCM upstream modulation
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsModulation(HINT32 nUsId, HINT32 *pnUsMod)
{
	CM_UNUSED(nUsId);
	CM_UNUSED(pnUsMod);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsBandwidth
* description : Get eCM upstream bandwidth
* input :
*   nUsId : upstream channel index
*   pnUsBandwith : pointer for storing eCM upstream bandwidth
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsBandwidth(HINT32 nUsId, HINT32 *pnUsBandwith)
{
	CM_UNUSED(nUsId);
	CM_UNUSED(pnUsBandwith);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsSymbolrate
* description : Get eCM upstream symbol rate
* input :
*   nUsId : upstream channel index
*   pnUsSymbolrate : pointer for storing eCM upstream symbol rate
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsSymbolrate(HINT32 nUsId, HINT32 *pnUsSymbolrate)
{
	CM_UNUSED(nUsId);
	CM_UNUSED(pnUsSymbolrate);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsSignalInfo
* description : Get eCM upstream signal information
* input :
*   nUsId : upstream channel index
*   pstUsInfo : pointer for storing eCM upstream signal information
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsSignalInfo (HINT32 nUsId, DI_CM_UsSignalQuality_t *pstUsInfo)
{
	CM_UNUSED(nUsId);
	CM_UNUSED(pstUsInfo);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetScanDsFreq
* description : Get downstream frequency from list
* input :
*   pszDsFreq : pointer for storing downstream frequency
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetScanDsFreq(HCHAR *pszDsFreq)
{
	CM_UNUSED(pszDsFreq);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_SetScanDsFreq
* description : Set downstream frequency to list
* input :
*   pszDsFreq : pointer for storing downstream frequency
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetScanDsFreq(HUINT32 *pulDsFreq)
{
	CM_UNUSED(pulDsFreq);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetTemperature
* description : Get saved and current temperature of eCM
* input :
*   pnCmSavedTemp : pointer for storing saved temperature
*   pnCmCurrentTemp : pointer for storing current temperature
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetTemperature(HINT32 *pnCmSavedTemp, HINT32 *pnCmCurrentTemp)
{
	CM_UNUSED(pnCmSavedTemp);
	CM_UNUSED(pnCmCurrentTemp);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_SetTemperature
* description : Set saved temperature of eCM
* input :
*   nTempMode : temperature for saving eCM flash
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetTemperature(HINT32 nTempMode)
{
	CM_UNUSED(nTempMode);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}


/*****************************************************************************
* function : DRV_CM_Brpc_SetStandbyMode
* description : Set eCM LED
* input :
*   nStandbyCmLed : 0 - LED off (standby) / 1 - LED on (operation)
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetStandbyMode(HINT32 nStandbyCmLed)
{
	CM_UNUSED(nStandbyCmLed);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_SetFanMode
* description : Set fan mode controlled by eCM
* input :
*   nFanMode : 0 - fan off / 1 - fan low speed / 2 - fan high speed
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetFanMode(HINT32 nFanMode)
{
	CM_UNUSED(nFanMode);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_SetDefault
* description : Set eCM default settings
* input : none
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetDefault(void)
{
	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_SetSerialNumber
* description : Set eCM serial number
* input :
*   pszCmSerialNo : pointer for storing seriel number
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetSerialNumber(HCHAR *pszCmSerialNo)
{
	CM_UNUSED(pszCmSerialNo);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_SetDsgMode
* description : Set eCM DSG mode
* input :
*   mode : 0 - disable / 1 - basic / 2 - advanced
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetDsgMode(HINT32 mode)
{
	CM_UNUSED(mode);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_SetDsgScan
* description : Set eCM scan
* input :
*   ucIsStart : 0 - scan stop / 1 - scan start
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetDsgScan(HUINT8 ucIsStart)
{
	CM_UNUSED(ucIsStart);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}


/*****************************************************************************
* function : DRV_CM_Brpc_SetAdminStatus
* description : Set eCM docsDevSwAdminStatus
* input :
*   nMode : 1 - allowProvisioningUpgrade / 2 - ignoreProvisioningUpgrade
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetAdminStatus(HINT32 nMode)
{
	CM_UNUSED(nMode);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}


/*****************************************************************************
* function : DRV_CM_Brpc_GetSwOperStatus
* description : Get eCM docsDevSwOperStatus
* input : none
* return :
*    docsDevSwOperStatus
******************************************************************************/
HINT32 DRV_CM_Brpc_GetSwOperStatus(void)
{
	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_SwUpgradeTrigger
* description : Trigger eCM SW upgrade with canned-SNMP
* input : none
* return : none
******************************************************************************/
void DRV_CM_Brpc_SwUpgradeTrigger(void)
{
//	docsSwUgradebyCannedSNMP();
}


/*****************************************************************************
* function : DRV_CM_Brpc_SwUpgrade
* description : Start eCM SW upgrade
* input : none
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SwUpgrade(void)
{
	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}


/*****************************************************************************
* function : DRV_CM_Brpc_GetEstbIp
* description : Get eSTB IP address
* input :
*   pcEstbIp : pointer for storing eSTB IP address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetEstbIp(HCHAR *pcEstbIp)
{
	CM_UNUSED(pcEstbIp);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}

/*****************************************************************************
* function : DRV_CM_Brpc_GetEstbMac
* description : Get eSTB MAC address
* input :
*   pucEstbMac : pointer for storing eSTB MAC address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetEstbMac(HUINT8 *pucEstbMac)
{
	CM_UNUSED(pucEstbMac);

	CM_DI_Print(0,("[%s][%d]\n", __FUNCTION__, __LINE__));

	return DRV_OK;
}


/*****************************************************************************
* function: DRV_CM_Dsgcc_GetCmIp
* description : Get saved and current temperature of CM
* input :
*   pucHmxAddedEvent : SNMP event ID
* return : none
******************************************************************************/
void humax_ProcessEcmNotifications(char * pucHmxAddedEvent)
{
#if 1
	CM_UNUSED(pucHmxAddedEvent);
#else
	switch (pucHmxAddedEvent[0])
	{
#if 0
		case Event_CDFilename:
			{
				int data_len = 0;
				unsigned char *data_ptr = NULL;
				unsigned char *ptr = (unsigned char *)pucHmxAddedEvent;

				//init filename
				VK_memset(g_CDFileName, 0, 128);

				//skip the 1st byte
				ptr++;

				//get the packet length
				data_len  = ((int)*ptr++) << 24;
				data_len |= ((int)*ptr++) << 16;
				data_len |= ((int)*ptr++) << 8;
				data_len |= ((int)*ptr++) << 0;

				//get the data payload pointer
				data_ptr = ptr;

				if (data_len > 0 && data_ptr != NULL)
				{
					VK_memcpy(g_CDFileName, data_ptr, data_len);
					CM_DI_Print(0,("[%s][%d] Docsis Common Download FileName(%s)\n", __FUNCTION__, __LINE__, g_CDFileName));
				}
			}
			break;

		case Event_CDIpAddress:
			{
				int data_len = 0, ip_address = 0;
				unsigned char *ptr = (unsigned char *)pucHmxAddedEvent;

				//init IP address
				g_CDIpAddress = 0;

				//skip the 1st byte
				ptr++;

				// get the packet length
				data_len  = ((int)*ptr++) << 24;
				data_len |= ((int)*ptr++) << 16;
				data_len |= ((int)*ptr++) << 8;
				data_len |= ((int)*ptr++) << 0;

				//get the data payload pointer
				ip_address  = ((int)*ptr++) << 24;
				ip_address |= ((int)*ptr++) << 16;
				ip_address |= ((int)*ptr++) << 8;
				ip_address |= ((int)*ptr++) << 0;

				if (data_len > 0 && ip_address != 0)
				{
					g_CDIpAddress = htonl(ip_address);
					CM_DI_Print(0,("[%s][%d] Docsis Common Download IpAddress(%lx)\n", __FUNCTION__, __LINE__, g_CDIpAddress);
				}
			}
			break;

		case Event_CmSerialNum:
			{
				int data_len = 0;
				unsigned char *data_ptr = NULL;
				unsigned char *ptr = (unsigned char *)pucHmxAddedEvent;

				//init filename
				VK_memset(g_ReceivedCmSerialNum, 0, sizeof(g_ReceivedCmSerialNum));

				//skip the 1st byte
				ptr++;

				//get the packet length
				data_len  = ((int)*ptr++) << 24;
				data_len |= ((int)*ptr++) << 16;
				data_len |= ((int)*ptr++) << 8;
				data_len |= ((int)*ptr++) << 0;

				//get the data payload pointer
				data_ptr = ptr;

				if (data_len > 0 && data_ptr != NULL)
				{
					VK_memcpy(g_ReceivedCmSerialNum, data_ptr, data_len);
					g_ReceivedCmSerialNumLen = data_len;
					CM_DI_Print(0,("[%s][%d] CmSerialNumber(%s) len(%d)\n", __FUNCTION__, __LINE__, g_ReceivedCmSerialNum, g_ReceivedCmSerialNumLen);

				}
			}
			break;

		case Event_MpegMacAddr:
			{
				int data_len = 0;
				unsigned char *data_ptr = NULL;
				unsigned char *ptr = (unsigned char *)pucHmxAddedEvent;

				//init filename
				VK_memset(g_ReceivedMpegMacAddr, 0, sizeof(g_ReceivedMpegMacAddr));

				//skip the 1st byte
				ptr++;

				//get the packet length
				data_len  = ((int)*ptr++) << 24;
				data_len |= ((int)*ptr++) << 16;
				data_len |= ((int)*ptr++) << 8;
				data_len |= ((int)*ptr++) << 0;

				//get the data payload pointer
				data_ptr = ptr;

				if (data_len > 0 && data_ptr != NULL)
				{
					VK_memcpy(g_ReceivedMpegMacAddr, data_ptr, data_len);
					g_ReceivedMpegMacAddrLen = data_len;
#if 0
					VK_printf("\nNHLEE : MpegMacAddr[%x][%x][%x][%x][%x][%x] len(%d)\n"
					, g_ReceivedMpegMacAddr[0], g_ReceivedMpegMacAddr[1],g_ReceivedMpegMacAddr[2],
					g_ReceivedMpegMacAddr[3], g_ReceivedMpegMacAddr[4],g_ReceivedMpegMacAddr[5], g_ReceivedMpegMacAddrLen);
#endif
				}
			}
			break;
#endif
		case Event_eCMSNMPGetRequest: /* eCM -> eSTB */
			{
				unsigned int data_len = 0;
				unsigned short ucSnmpEventId;
				unsigned char *ptr = (unsigned char *)pucHmxAddedEvent;
#if 0
				VK_printf("\n NHLEE Receiving SNMP pkt\n");
				{
					unsigned int i = 0;
					for( i=0; i<16 ; i++)
					{
						VK_printf(" %02x ", ptr[i]);
					}
					VK_printf("\n");
				}
#endif
				//skip the 1st byte(Notification ID)
				ptr++;

				// get the packet length
				data_len  = ((int)*ptr++) << 24;
				data_len |= ((int)*ptr++) << 16;
				data_len |= ((int)*ptr++) << 8;
				data_len |= ((int)*ptr++) << 0;

				//get the data payload pointer
				ucSnmpEventId = ( (unsigned char) pucHmxAddedEvent[5] ) << 8;
				ucSnmpEventId |= ( (unsigned char) pucHmxAddedEvent[6] );

				if (data_len > 0 && ucSnmpEventId != 0)
				{
					CM_DI_Print(1,("[%s][%d] eCM SNMP GetRequest(%d)(%d)\n", __FUNCTION__, __LINE__, ucSnmpEventId, data_len));
					P_CM_RecvSNMPGetRequest(ucSnmpEventId, data_len);
				}
			}
			break;

		case Event_eCMSNMPSetRequest: /* eCM -> eSTB */
			{
				unsigned int data_len = 0;
				unsigned short ucSnmpEventId;
				unsigned char *ptr = (unsigned char *)pucHmxAddedEvent;
				unsigned char ucPduData = 0;
#if 0
				VK_printf("\n NHLEE Receiving SetRequest SNMP pkt\n");
				{
					unsigned int i = 0;
					for( i=0; i<16 ; i++)
					{
						VK_printf(" %02x ", ptr[i]);
					}
					VK_printf("\n");
				}
#endif
				//skip the 1st byte(Notification ID)
				ptr++;

				// get the packet length
				data_len  = ((int)*ptr++) << 24;
				data_len |= ((int)*ptr++) << 16;
				data_len |= ((int)*ptr++) << 8;
				data_len |= ((int)*ptr++) << 0;

				//get the data payload pointer
				ucSnmpEventId = ( (unsigned char) pucHmxAddedEvent[5] ) << 8;
				ucSnmpEventId |= ( (unsigned char) pucHmxAddedEvent[6] );

				ucPduData = (unsigned char) pucHmxAddedEvent[7];
				if (data_len > 0 && ucSnmpEventId != 0)
				{
					CM_DI_Print(1,("[%s][%d] eCM SNMP SetRequest(%d)(%d)\n", __FUNCTION__, __LINE__, ucSnmpEventId, data_len));
					P_CM_RecvSNMPSetRequest(ucSnmpEventId, data_len, ucPduData);
				}
			}
			break;

		default:
			CM_DI_Print(0,("[%s][%d] humax_ProcessEcmNotifications :: Invalid Notification from eCM %d\n", __FUNCTION__, __LINE__, pucHmxAddedEvent[0]));
			break;
	}
#endif
}


/*****************************************************************************
* function: DRV_CM_StartRebootMonitoringTask
* description : Start task for monitoring eCM rebooting
* input : none
* return:
*   DRV_Error : DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_StartRebootMonitoringTask(void)
{
	DRV_Error	nRet = DRV_ERR;
	HULONG		ulRebootMonitorTask = 0;

	nRet = VK_TASK_Create(  P_CM_RebootMonitorTask,
							DRV_CM_TASK_PRIORITY,
							BASIC_TASK_STACK_SIZE,
							"CM_Reboot_Monitor",
							NULL,
							&ulRebootMonitorTask,
							0 );

	if ((ulRebootMonitorTask == 0) || (nRet != VK_OK))
	{
		CM_DI_Print(0, ("[%s][%d] Error nRet(0x%x)", __FUNCTION__, __LINE__, nRet));
		return DRV_ERR;
	}
	else
	{
		VK_TASK_Start(ulRebootMonitorTask);
	}

	return DRV_OK;
}


/*****************************************************************************
* function: DRV_CM_RegisterEventNotify
* description : Register callback function for each event
* input :
*   eCmEvent : event for eCM
*   pfCmEventCallback : call-back function
* return: none
******************************************************************************/
void DRV_CM_RegisterEventNotify(DRV_CM_EVENT eCmEvent, DRV_CM_CALLBACK pfCmEventCallback)
{
	if (s_pfCmCallback[eCmEvent] != NULL)
	{
		CM_DI_Print(0, ("[%s][%d] CM callback function for event(%d) is already registered \n", __FUNCTION__, __LINE__, eCmEvent));
	}
	else
	{
		s_pfCmCallback[eCmEvent]= pfCmEventCallback;
		CM_DI_Print(0, ("[%s][%d] CM callback function for event(%d) is successfully registered \n", __FUNCTION__, __LINE__, eCmEvent));
	}

	return;
}


/*****************************************************************************
* function: DRV_CM_API_Install
* description : Install DRV_CM_xxx APIs
* input : none
* return: none
******************************************************************************/
void DRV_CM_API_Install(void)
{
	DRV_CM_GetEcmIp = &DRV_CM_Brpc_GetEcmIp;
	DRV_CM_GetEcmMac = &DRV_CM_Brpc_GetEcmMac;
	DRV_CM_SetEcmMac = &DRV_CM_Brpc_SetEcmMac;
	DRV_CM_GetNetmask = &DRV_CM_Brpc_GetNetmask;
	DRV_CM_GetGateway = &DRV_CM_Brpc_GetGateway;
	DRV_CM_GetSwVer = &DRV_CM_Brpc_GetSwVer;
	DRV_CM_GetCmRegStatus = &DRV_CM_Brpc_GetCmRegStatus;
	DRV_CM_GetDsFreq = &DRV_CM_Brpc_GetDsFreq;
	DRV_CM_GetDsPower = &DRV_CM_Brpc_GetDsPower;
	DRV_CM_GetDsSnr = &DRV_CM_Brpc_GetDsSnr;
	DRV_CM_GetDsBer = &DRV_CM_Brpc_GetDsBer;
	DRV_CM_GetDsChannelId = &DRV_CM_Brpc_GetDsChannelId;
	DRV_CM_GetDsModulation = &DRV_CM_Brpc_GetDsModulation;
	DRV_CM_GetDsCorrected = &DRV_CM_Brpc_GetDsCorrected;
	DRV_CM_GetDsUncorrected = &DRV_CM_Brpc_GetDsUncorrected;
	DRV_CM_GetDsSignalInfo = &DRV_CM_Brpc_GetDsSignalInfo;
	DRV_CM_GetUsFreq = &DRV_CM_Brpc_GetUsFreq;
	DRV_CM_GetUsPower = &DRV_CM_Brpc_GetUsPower;
	DRV_CM_GetUsRangingStatus = &DRV_CM_Brpc_GetUsRangingStatus;
	DRV_CM_GetUsChannelId = &DRV_CM_Brpc_GetUsChannelId;
	DRV_CM_GetUsModulation = &DRV_CM_Brpc_GetUsModulation;
	DRV_CM_GetUsBandwidth = &DRV_CM_Brpc_GetUsBandwidth;
	DRV_CM_GetUsSymbolrate = &DRV_CM_Brpc_GetUsSymbolrate;
	DRV_CM_GetUsSignalInfo = &DRV_CM_Brpc_GetUsSignalInfo;
	DRV_CM_GetScanDsFreq = &DRV_CM_Brpc_GetScanDsFreq;
	DRV_CM_SetScanDsFreq = &DRV_CM_Brpc_SetScanDsFreq;
	DRV_CM_GetTemperature = &DRV_CM_Brpc_GetTemperature;
	DRV_CM_SetTemperature = &DRV_CM_Brpc_SetTemperature;
	DRV_CM_SetStandbyMode = &DRV_CM_Brpc_SetStandbyMode;
	DRV_CM_SetFanMode = &DRV_CM_Brpc_SetFanMode;
	DRV_CM_SetDefault = &DRV_CM_Brpc_SetDefault;
	DRV_CM_SetSerialNumber = &DRV_CM_Brpc_SetSerialNumber;
	DRV_CM_SetDsgMode = &DRV_CM_Brpc_SetDsgMode;
	DRV_CM_SetDsgScan = &DRV_CM_Brpc_SetDsgScan;
	DRV_CM_SetAdminStatus = &DRV_CM_Brpc_SetAdminStatus;
	DRV_CM_GetSwOperStatus = &DRV_CM_Brpc_GetSwOperStatus;
	DRV_CM_SwUpgradeTrigger = &DRV_CM_Brpc_SwUpgradeTrigger;
	DRV_CM_SwUpgrade = &DRV_CM_Brpc_SwUpgrade;
	DRV_CM_GetEstbIp = &DRV_CM_Brpc_GetEstbIp;
	DRV_CM_GetEstbMac = &DRV_CM_Brpc_GetEstbMac;
}


/* end of file */
