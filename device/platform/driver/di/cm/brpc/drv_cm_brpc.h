/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: drv_ts_path.h
// Original Author:
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.								*/
/* All rights reserved. 												*/
/*******************************************************************/

#ifndef	_DRV_CM_BRCM_RPC_H_
#define	_DRV_CM_BRCM_RPC_H_

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"
#include "di_cm.h"

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef	enum
{
	DRV_CM_EVENT_REBOOT,
	DRV_CM_EVENT_END
}DRV_CM_EVENT;

typedef void (*DRV_CM_CALLBACK)(void*);


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

/*****************************************************************************
* function : DRV_CM_Brpc_GetEcmIp
* description : Get eCM IP address
* input :
*   pulEcmIp : pointer for storing eCM IP address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetEcmIp(HUINT32 *pulEcmIp);

/*****************************************************************************
* function : DRV_CM_Brpc_GetEcmMac
* description : Get eCM MAC address
* input :
*   pucEcmMac : pointer for storing eCM MAC address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetEcmMac(HUINT8 *pucEcmMac);

/*****************************************************************************
* function : DRV_CM_Brpc_SetEcmMac
* description : Set eCM MAC address
* input :
*   pucEcmMac : pointer for storing eCM MAC address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetEcmMac(HUINT8 *pucEcmMac);

/*****************************************************************************
* function : DRV_CM_Brpc_GetNetmask
* description : Get eCM subnet mask
* input :
*   pulCmNetmask : pointer for storing eCM subnet mask
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetNetmask(HUINT32 *pulCmNetmask);

/*****************************************************************************
* function : DRV_CM_Brpc_GetGateway
* description : Get eCM gateway IP address
* input :
*   pulCmGateway : pointer for storing eCM gateway IP address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetGateway(HUINT32 *pulCmGateway);

/*****************************************************************************
* function : DRV_CM_Brpc_GetSwVer
* description : Get eCM SW version
* input :
*   pucCmSwVer : pointer for storing eCM SW version
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetSwVer(HUINT8 *pucCmSwVer);

/*****************************************************************************
* function : DRV_CM_Brpc_GetCmRegStatus
* description : Get eCM registration status
* input :
*   pnRegStatus : pointer for storing eCM registration status
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetCmRegStatus(HINT32 *pnRegStatus);

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsFreq
* description : Get eCM downstream frequency
* input :
*   pnDsFreq : pointer for storing eCM downstream frequency
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsFreq(HINT32 nDsId, HINT32 *pnDsFreq);

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsPower
* description : Get eCM downstream RF power level
* input :
*   nDsId : downstream channel index
*   pnDsPower : pointer for storing eCM downstream RF power level
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsPower(HINT32 nDsId, HINT32 *pnDsPower);

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsSnr
* description : Get eCM downstream SNR
* input :
*   nDsId : downstream channel index
*   pnDsSnr : pointer for storing eCM downstream SNR
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsSnr(HINT32 nDsId, HINT32 *pnDsSnr);

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsBer
* description : Get eCM downstream BER
* input :
*   nDsId : downstream channel index
*   pnDsBer : pointer for storing eCM downstream BER
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsBer(HINT32 nDsId, HINT32 *pnDsBer);

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsChannelId
* description : Get eCM downstream channel ID
* input :
*   nDsId : downstream channel index
*   pnDsChId : pointer for storing eCM downstream channel ID
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsChannelId(HINT32 nDsId, HINT32 *pnDsChId);

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsModulation
* description : Get eCM downstream modulation
* input :
*   nDsId : downstream channel index
*   pnDsMod : pointer for storing eCM downstream modulation
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsModulation(HINT32 nDsId, HINT32 *pnDsMod);

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsCorrected
* description : Get eCM downstream corrected error count
* input :
*   nDsId : downstream channel index
*   pnDsCorrected : pointer for storing eCM downstream corrected error count
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsCorrected(HINT32 nDsId, HINT32 *pnDsCorrected);

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsUncorrected
* description : Get eCM downstream uncorrected error count
* input :
*   nDsId : downstream channel index
*   pnDsUncorrected : pointer for storing eCM downstream uncorrected error count
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsUncorrected(HINT32 nDsId, HINT32 *pnDsUncorrected);

/*****************************************************************************
* function : DRV_CM_Brpc_GetDsSignalInfo
* description : Get eCM downstream signal information
* input :
*   nDsId : downstream channel index
*   pstDsInfo : pointer for storing eCM downstream signal information
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetDsSignalInfo(HINT32 nDsId, DI_CM_DsSignalQuality_t *pstDsInfo);

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsFreq
* description : Get eCM upstream frequency
* input :
*   nUsId : upstream channel index
*   pnUsFreq : pointer for storing eCM upstream frequency
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsFreq(HINT32 nUsId, HINT32 *pnUsFreq);

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsPower
* description : Get eCM upstream RF power level
* input :
*   nUsId : upstream channel index
*   pnUsPower : pointer for storing eCM upstream RF power level
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsPower(HINT32 nUsId, HINT32 *pnUsPower);

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsRangingStatus
* description : Get eCM upstream ranging status
* input :
*   nUsId : upstream channel index
*   pnUsRanging : pointer for storing eCM upstream ranging status
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsRangingStatus(HINT32 nUsId, HINT32 *pnUsRanging);

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsChannelId
* description : Get eCM upstream channel ID
* input :
*   nUsId : upstream channel index
*   pnUsChId : pointer for storing eCM upstream channel ID
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsChannelId(HINT32 nUsId, HINT32 *pnUsChId);

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsModulation
* description : Get eCM upstream modulation
* input :
*   nUsId : upstream channel index
*   pnUsMod : pointer for storing eCM upstream modulation
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsModulation(HINT32 nUsId, HINT32 *pnUsMod);

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsBandwidth
* description : Get eCM upstream bandwidth
* input :
*   nUsId : upstream channel index
*   pnUsBandwith : pointer for storing eCM upstream bandwidth
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsBandwidth(HINT32 nUsId, HINT32 *pnUsBandwith);

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsSymbolrate
* description : Get eCM upstream symbol rate
* input :
*   nUsId : upstream channel index
*   pnUsSymbolrate : pointer for storing eCM upstream symbol rate
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsSymbolrate(HINT32 nUsId, HINT32 *pnUsSymbolrate);

/*****************************************************************************
* function : DRV_CM_Brpc_GetUsSignalInfo
* description : Get eCM upstream signal information
* input :
*   nUsId : upstream channel index
*   pstUsInfo : pointer for storing eCM upstream signal information
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetUsSignalInfo (HINT32 nUsId, DI_CM_UsSignalQuality_t *pstUsInfo);

/*****************************************************************************
* function : DRV_CM_Brpc_GetScanDsFreq
* description : Get downstream frequency from list
* input :
*   pszDsFreq : pointer for storing downstream frequency
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetScanDsFreq(HCHAR *pszDsFreq);

/*****************************************************************************
* function : DRV_CM_Brpc_SetScanDsFreq
* description : Set downstream frequency to list
* input :
*   pszDsFreq : pointer for storing downstream frequency
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetScanDsFreq(HUINT32 *pulDsFreq);

/*****************************************************************************
* function : DRV_CM_Brpc_GetTemperature
* description : Get saved and current temperature of eCM
* input :
*   pnCmSavedTemp : pointer for storing saved temperature
*   pnCmCurrentTemp : pointer for storing current temperature
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetTemperature(HINT32 *pnCmSavedTemp, HINT32 *pnCmCurrentTemp);

/*****************************************************************************
* function : DRV_CM_Brpc_SetTemperature
* description : Set saved temperature of eCM
* input :
*   nTempMode : temperature for saving eCM flash
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetTemperature(HINT32 nTempMode);

/*****************************************************************************
* function : DRV_CM_Brpc_SetStandbyMode
* description : Set eCM LED
* input :
*   nStandbyCmLed : 0 - LED off (standby) / 1 - LED on (operation)
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetStandbyMode(HINT32 nStandbyCmLed);

/*****************************************************************************
* function : DRV_CM_Brpc_SetFanMode
* description : Set fan mode controlled by eCM
* input :
*   nFanMode : 0 - fan off / 1 - fan low speed / 2 - fan high speed
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetFanMode(HINT32 nFanMode);

/*****************************************************************************
* function : DRV_CM_Brpc_SetDefault
* description : Set eCM default settings
* input : none
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetDefault(void);

/*****************************************************************************
* function : DRV_CM_Brpc_SetSerialNumber
* description : Set eCM serial number
* input :
*   pszCmSerialNo : pointer for storing seriel number
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetSerialNumber(HCHAR *pszCmSerialNo);

/*****************************************************************************
* function : DRV_CM_Brpc_SetDsgMode
* description : Set eCM DSG mode
* input :
*   mode : 0 - disable / 1 - basic / 2 - advanced
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetDsgMode(HINT32 mode);

/*****************************************************************************
* function : DRV_CM_Brpc_SetDsgScan
* description : Set eCM scan
* input :
*   ucIsStart : 0 - scan stop / 1 - scan start
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetDsgScan(HUINT8 ucIsStart);

/*****************************************************************************
* function : DRV_CM_Brpc_SetAdminStatus
* description : Set eCM docsDevSwAdminStatus
* input :
*   nMode : 1 - allowProvisioningUpgrade / 2 - ignoreProvisioningUpgrade
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SetAdminStatus(HINT32 nMode);

/*****************************************************************************
* function : DRV_CM_Brpc_GetSwOperStatus
* description : Get eCM docsDevSwOperStatus
* input : none
* return :
*    docsDevSwOperStatus
******************************************************************************/
HINT32 DRV_CM_Brpc_GetSwOperStatus(void);

/*****************************************************************************
* function : DRV_CM_Brpc_SwUpgradeTrigger
* description : Trigger eCM SW upgrade with canned-SNMP
* input : none
* return : none
******************************************************************************/
void DRV_CM_Brpc_SwUpgradeTrigger(void);

/*****************************************************************************
* function : DRV_CM_Brpc_SwUpgrade
* description : Start eCM SW upgrade
* input : none
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_SwUpgrade(void);

/*****************************************************************************
* function : DRV_CM_Brpc_GetEstbIp
* description : Get eSTB IP address
* input :
*   pcEstbIp : pointer for storing eSTB IP address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetEstbIp(HCHAR *pcEstbIp);

/*****************************************************************************
* function : DRV_CM_Brpc_GetEstbMac
* description : Get eSTB MAC address
* input :
*   pucEstbMac : pointer for storing eSTB MAC address
* return :
*   DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_Brpc_GetEstbMac(HUINT8 *pucEstbMac);

/*****************************************************************************
* function: DRV_CM_StartRebootMonitoringTask
* description : Start task for monitoring eCM rebooting
* input : none
* return:
*   DRV_Error : DRV_OK / DRV_ERR
******************************************************************************/
DRV_Error DRV_CM_StartRebootMonitoringTask(void);

/*****************************************************************************
* function: DRV_CM_RegisterEventNotify
* description : Register callback function for each event
* input : none
*   eCmEvent : event for eCM
*   pfCmEventCallback : call-back function
* return: none
******************************************************************************/
void DRV_CM_RegisterEventNotify(DRV_CM_EVENT eCmEvent, DRV_CM_CALLBACK pfCmEventCallback);

/*****************************************************************************
* function: DRV_CM_API_Install
* description : Install DRV_CM_xxx APIs
* input : none
* return: none
******************************************************************************/
void DRV_CM_API_Install(void);
DRV_Error (*DRV_CM_GetEcmIp)(HUINT32 *pulEcmIp);
DRV_Error (*DRV_CM_GetEcmMac)(HUINT8 *pucEcmMac);
DRV_Error (*DRV_CM_SetEcmMac)(HUINT8 *pucEcmMac);
DRV_Error (*DRV_CM_GetNetmask)(HUINT32 *pulCmNetmask);
DRV_Error (*DRV_CM_GetGateway)(HUINT32 *pulCmGateway);
DRV_Error (*DRV_CM_GetSwVer)(HUINT8 *pucCmSwVer);
DRV_Error (*DRV_CM_GetCmRegStatus)(HINT32 *pnRegStatus);
DRV_Error (*DRV_CM_GetDsFreq)(HINT32 nDsId, HINT32 *pnDsFreq);
DRV_Error (*DRV_CM_GetDsPower)(HINT32 nDsId, HINT32 *pnDsPower);
DRV_Error (*DRV_CM_GetDsSnr)(HINT32 nDsId, HINT32 *pnDsSnr);
DRV_Error (*DRV_CM_GetDsBer)(HINT32 nDsId, HINT32 *pnDsBer);
DRV_Error (*DRV_CM_GetDsChannelId)(HINT32 nDsId, HINT32 *pnDsChId);
DRV_Error (*DRV_CM_GetDsModulation)(HINT32 nDsId, HINT32 *pnDsMod);
DRV_Error (*DRV_CM_GetDsCorrected)(HINT32 nDsId, HINT32 *pnDsCorrected);
DRV_Error (*DRV_CM_GetDsUncorrected)(HINT32 nDsId, HINT32 *pnDsUncorrected);
DRV_Error (*DRV_CM_GetDsSignalInfo)(HINT32 nDsId, DI_CM_DsSignalQuality_t *pstDsInfo);
DRV_Error (*DRV_CM_GetUsFreq)(HINT32 nUsId, HINT32 *pnUsFreq);
DRV_Error (*DRV_CM_GetUsPower)(HINT32 nUsId, HINT32 *pnUsPower);
DRV_Error (*DRV_CM_GetUsRangingStatus)(HINT32 nUsId, HINT32 *pnUsRanging);
DRV_Error (*DRV_CM_GetUsChannelId)(HINT32 nUsId, HINT32 *pnUsChId);
DRV_Error (*DRV_CM_GetUsModulation)(HINT32 nUsId, HINT32 *pnUsMod);
DRV_Error (*DRV_CM_GetUsBandwidth)(HINT32 nUsId, HINT32 *pnUsBandwith);
DRV_Error (*DRV_CM_GetUsSymbolrate)(HINT32 nUsId, HINT32 *pnUsSymbolrate);
DRV_Error (*DRV_CM_GetUsSignalInfo)(HINT32 nUsId, DI_CM_UsSignalQuality_t *pstUsInfo);
DRV_Error (*DRV_CM_GetScanDsFreq)(HCHAR *pszDsFreq);
DRV_Error (*DRV_CM_SetScanDsFreq)(HUINT32 *pulDsFreq);
DRV_Error (*DRV_CM_GetTemperature)(HINT32 *pnCmSavedTemp, HINT32 *pnCmCurrentTemp);
DRV_Error (*DRV_CM_SetTemperature)(HINT32 nTempMode);
DRV_Error (*DRV_CM_SetStandbyMode)(HINT32 nStandbyCmLed);
DRV_Error (*DRV_CM_SetFanMode)(HINT32 nFanMode);
DRV_Error (*DRV_CM_SetDefault)(void);
DRV_Error (*DRV_CM_SetSerialNumber)(HCHAR *pszCmSerialNo);
DRV_Error (*DRV_CM_SetDsgMode)(HINT32 mode);
DRV_Error (*DRV_CM_SetDsgScan)(HUINT8 ucIsStart);
DRV_Error (*DRV_CM_SetAdminStatus)(HINT32 nMode);
HINT32 (*DRV_CM_GetSwOperStatus)(void);
void (*DRV_CM_SwUpgradeTrigger)(void);
DRV_Error (*DRV_CM_SwUpgrade)(void);
DRV_Error (*DRV_CM_GetEstbIp)(HCHAR *pcEstbIp);
DRV_Error (*DRV_CM_GetEstbMac)(HUINT8 *pucEstbMac);

#endif /* !_DRV_CM_BRCM_RPC_H_ */

