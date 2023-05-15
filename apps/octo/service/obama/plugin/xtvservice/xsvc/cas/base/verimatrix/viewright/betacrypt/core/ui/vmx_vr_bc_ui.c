/* (c) 2011-2015 Humax Co., Ltd. 
 * This program is produced by Humax Co., Ltd. ("Humax") and 
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
 * non-assignable, non-transferable and non-exclusive license to use this Software. 
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
 * you agree to the responsibility to take all reasonable efforts to protect the any information 
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, 
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies 
 * of the Software including all documentation. This License will terminate immediately without notice from Humax 
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies 
 * of the Software and all documentation.  

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice. 
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court, 
 * in the Republic of Korea.
*/
/**
	@file     vmx_vr_bc_ui.c
	@brief    vmx_vr_bc_ui.c (Verimatrix BC UI)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#include <db_param.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stdarg.h>
#include <math.h>

#include <isosvc.h>
#include <casmgr_mgr.h>
#include "vmx_mgr.h"
#include "vmx_ui.h"
#include "_xsvc_cas_vmx_os.h"
#include "_xsvc_cas_vmx_bc_main.h"
#include "vmx_vr_bc_api.h"
#include "vmx_vr_bc_ui_log.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
typedef enum
{
	eSvcXxx_Type,

} xsvcXxx_Type_e;

typedef  struct
{
	...
} xsvcXxx_Type_t;
*/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
VmxBc_Requst_t 	stBcRequest;

VmxBc_Osm_t * vmx_ui_GetBcOsmRequest(void);
VmxBc_ISC_Pin_t * bc_ui_GetBcPinRequest(void);



//VmxBc_Osm_t stOsm;

// TODO_VMX_UI : 아래 두개의 구조체를 service index만큼 배열로 만들어야한다.
VmxBc_ISCOrderPin_t	stOrderPin;//[3];	// 우선 max service index를 3개로 하자 
VmxBc_ISCCheckPin_t	stCheckPin;//[3];   // 우선 max service index를 3개로 하자

void bc_ui_SetOsmInit(void);
void vmx_bc_ui_SetPinInit(void);


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*	
// Local function
STATIC HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

/*
// Global function
HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/
#if CONFIG_DEBUG
void bc_ui_HexDump(HUINT8 *buf)
{
	HUINT16 i;
	HBOOL bcnt = FALSE;

	HxLOG_Print ("\tBuf = \n\t{\n");
	for(i=0;i<512;i++)
	{
		if(i != 0)
			HxLOG_Print (", ");
		if(i%16==0 && bcnt != FALSE)
			HxLOG_Print ("\t  \n");
		bcnt = TRUE;
		HxLOG_Print ("0x%02x", buf[i]);
		if(buf[i] == '\0')
			break;		
	}
	HxLOG_Print ("\n\t}\n\n");
}

void vmx_ui_DebugPrintforRequest(VmxBc_Requst_t *pstRequest, HUINT8 *pMsg)
{
	int i;
	if(pstRequest == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return;
	}

	HxLOG_Print ("*****************************************************\n");
	HxLOG_Print ("*\n");
	HxLOG_Print ("*\t< Request Parse : %s >\n", pMsg);
	HxLOG_Print ("* \tRequest Type(%d) = %s\n", pstRequest->eType , pstRequest->eType == eVmxBc_Bc_Osm?"eVmxBc_Bc_Osm":"eVmxBc_Bc_Pin");	
	HxLOG_Print ("*\n");
	HxLOG_Print ("*\n");

	if(pstRequest->eType == eVmxBc_Bc_Osm)
	{
		VmxBc_Osm_t *pstOsm = NULL;
		VmxBc_Trigger_t *pstTrigger = NULL;
		VmxBc_OSD_t	*pstOSD = NULL;

		pstOsm = &pstRequest->stOsm;
		if(pstOsm == NULL)
		{
			HxLOG_Error(DBG_ASSERT, ("\n");
			return;
		}

		HxLOG_Print ("********** OSM Print ***********\n");	
		HxLOG_Print ("\t* OsmType : %s\n", VmxBc_OsmType_Str[pstOsm->eOsmType]);

		if(pstOsm->eOsmType == eVmxBc_OsmType_Trigger)
		{
			pstTrigger = &pstOsm->stTrigger[pstOsm->eTriggerType];
			if(pstTrigger == NULL)
			{
				HxLOG_Error(DBG_ASSERT, ("\n");
				return;				
			}

			if(pstTrigger->eTriggerType == eVmxBc_TriggerType_Upgrade)
			{
				HxLOG_Print ("\t - UpgradeType(%d) = %s\n", pstTrigger->item.stUpgrade.eType, VmxBc_UpgradeType_Str[pstTrigger->item.stUpgrade.eType]);

				if(pstTrigger->item.stUpgrade.stDesc.pDescriptor)
				{
					HxLOG_Print ("\t - With descriptor : length(%d)\n", pstTrigger->item.stUpgrade.stDesc.usLength);
					for(i=0;i<pstTrigger->item.stUpgrade.stDesc.usLength;i++)
					{
						if(i%16==0)
							HxLOG_Print ("\n\t");
						HxLOG_Print ("%02X ", pstTrigger->item.stUpgrade.stDesc.pDescriptor[i]);
					}
					HxLOG_Print ("\n");
				}
				else
				{
					HxLOG_Print ("\t - Without descriptor\n");
				}
			}
			else if(pstTrigger->eTriggerType == eVmxBc_TriggerType_Rescan)
			{
				HxLOG_Print ("\t - RescanType(%d) = %s\n", pstTrigger->item.stRescan.eType, VmxBc_RescanType_Str[pstTrigger->item.stRescan.eType]);			
			}
			else if(pstTrigger->eTriggerType == eVmxBc_TriggerType_Switch)
			{
				HxLOG_Print ("\t - SwitchType(%d) = %s\n", pstTrigger->item.stSwitch.eType, VmxBc_SwitchType_Str[pstTrigger->item.stSwitch.eType]);			
			}
			else if(pstTrigger->eTriggerType == eVmxBc_TriggerType_PinNumberReset)
			{
				HxLOG_Print ("\t - PinNumberResetType : New Pin Number = %d%d%d%d\n", 
					pstTrigger->item.stPinNumberReset.ucPinNumberBuf[0],
					pstTrigger->item.stPinNumberReset.ucPinNumberBuf[1],
					pstTrigger->item.stPinNumberReset.ucPinNumberBuf[2],
					pstTrigger->item.stPinNumberReset.ucPinNumberBuf[3]);
			}
			else if(pstTrigger->eTriggerType == eVmxBc_TriggerType_Watermarking)
			{
				HxLOG_Print ("\t - WatermarkingType = %s\n", pstTrigger->item.stWatermarking.ucSCSerialNumStr);
				HHxLOG_PrintLog ("\t - duration = %d\n", pstTrigger->item.stWatermarking.usDuration);
				HxLOG_Print ("\t - x frame = %d\n", pstTrigger->item.stWatermarking.x_frame);
				HxLOG_Print ("\t - y frame = %d\n", pstTrigger->item.stWatermarking.y_frame);
			}
			else if(pstTrigger->eTriggerType == eVmxBc_TriggerType_Eas)
			{
				HxLOG_Print ("\t - EasType\n");
			}
			//bc_ui_HexDump(&pstTrigger->buf[0]);
		}
		else
		{
			pstOSD = &pstOsm->stOSD;
			if(pstOSD == NULL)
			{
				HxLOG_Error(DBG_ASSERT, ("\n");
				return;
			}

			HxLOG_Print ("\t* OsmStatus(%d) : %s\n", pstOSD->eOsmStatus, VmxBc_OsmStauts_Str[pstOSD->eOsmStatus]);	
			HxLOG_Print ("\t* X_Pos = %s\n", VmxBc_Osm_X_Pos_Str[pstOSD->stBuildWindow.stMode.x_pos]);
			HxLOG_Print ("\t* Y_Pos = %s\n", VmxBc_Osm_Y_Pos_Str[pstOSD->stBuildWindow.stMode.y_pos]);	
			HxLOG_Print ("\t* Text Alig = %s\n", VmxBc_Osm_Text_Algi_Str[pstOSD->stBuildWindow.stMode.text_alig]);		
			HxLOG_Print ("\t* x(%d), y(%d), w(%d), h(%d)\n", pstOSD->stBuildWindow.stScreen.x, pstOSD->stBuildWindow.stScreen.y, pstOSD->stBuildWindow.stScreen.w, pstOSD->stBuildWindow.stScreen.h);
			HxLOG_Print ("\t* bg(0x%x)\n", pstOSD->stBuildWindow.ucBg);
			HxLOG_Print ("\t* Alpha(0x%x)\n", pstOSD->stBuildWindow.ucAlpha);
			HxLOG_Print ("\t* fg(0x%x)\n", pstOSD->stBuildWindow.ucFg);		
			HxLOG_Print ("\t* Display_Mode(%d) = %s\n", pstOSD->stDisplayWindow.eMode, VmxBc_DisplayWindowMode_Str[pstOSD->stDisplayWindow.eMode]);
			HxLOG_Print ("\t* Display_Type(%d) = %s\n", pstOSD->stDisplayWindow.eType, VmxBc_DisplayWindowType_Str[pstOSD->stDisplayWindow.eType-1]);		
			HxLOG_Print ("\t* Display_Duration(%d)\n", pstOSD->stDisplayWindow.usDuration);

			//bc_ui_HexDump(&pstTrigger->buf[0]);
		}
	}
	else if(pstRequest->eType == eVmxBc_Bc_Pin)
	{
		VmxBc_ISC_Pin_t * pPin = NULL;
		HxLOG_Print ("********** PIN Print ***********\n");			
		pPin = &pstRequest->stPin;
		if(pPin == NULL)
		{
			HxLOG_Error(DBG_ASSERT, ("\n");
		}
		
		HxLOG_Print ("\t* pPin Type = %s\n", pPin->eType==eVmxBc_PinType_Check?"eVmxBc_PinType_Check":"eVmxBc_PinType_Order");

		if(pPin->eType==eVmxBc_PinType_Check)
		{
			HxLOG_Print ("\t* bPinIndex = %d\n", pPin->stCheckPin.bPinIndex);
			HxLOG_Print ("\t* bServiceIdx = %d\n",pPin->stCheckPin.bServiceIdx);
			HxLOG_Print ("\t* eReason[%d] = %s\n", pPin->stCheckPin.eReason, VmxBc_PinReason_Type_Str[pPin->stCheckPin.eReason]);
		}
		else if(pPin->eType==eVmxBc_PinType_Order)
		{
			HxLOG_Print ("\t* bPinIndex = %d\n", pPin->stOrderPin.bPinIndex);
			HxLOG_Print ("\t* lEventId = %d\n", pPin->stOrderPin.lEventId);
			HxLOG_Print ("\t* bServiceIdx = %d\n", pPin->stOrderPin.bServiceIdx);
			HxLOG_Print ("\t* eReason = %d\n", pPin->stOrderPin.eReason);
			HxLOG_Print ("\t* Credit_Buf = %s\n", pPin->stOrderPin.Credit_Buf);
			HxLOG_Print ("\t* Cost_Buf = %s\n", pPin->stOrderPin.Cost_Buf);
		}
		else
		{
			HxLOG_Error(DBG_ASSERT, ("\n");
		}
	}
	HxLOG_Print ("*\n");
	HxLOG_Print ("*****************************************************\n");
}

void bc_ui_OrderPinDebugPrint(VmxBc_ISCOrderPin_t *pstOrderPin, HUINT8 bPinIndex, HUINT32 lPurse, HUINT32 lCost, HUINT16 wMult, HUINT16 wDiv, HUINT8 bLocation, HUINT8 bSign0, HUINT8 bSign1000, HUINT8 bCount, HUINT8 bLen, HUINT8 *abText, HUINT32 lEventId, HUINT8 bServiceIdx)
{
	if(pstOrderPin == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return;
	}

	HxLOG_Print ("* [%s:%d]\n", __FUNCTION__, __LINE__);
	HxLOG_Print ("* bPinIndex(%d)\n", bPinIndex);
	HxLOG_Print ("* lPurse(%d)\n", lPurse);
	HxLOG_Print ("* lCost(%d)\n", lCost);
	HxLOG_Print ("* wMult(%d)\n", wMult);
	HxLOG_Print ("* wDiv(%d)\n", wDiv);
	HxLOG_Print ("* bLocation(%d)\n", bLocation);
	HxLOG_Print ("* bSign0(%d)\n", bSign0);
	HxLOG_Print ("* bSign1000(%d)\n", bSign1000);
	HxLOG_Print ("* bCount(%d)\n", bCount);
	HxLOG_Print ("* bLen(%d)\n", bLen);
	HxLOG_Print ("* abText(%s)\n", abText);
	HxLOG_Print ("* lEventId(%d)\n", lEventId);
	HxLOG_Print ("* bServiceIdx(%d)\n", bServiceIdx);

	HxLOG_Print ("* Result : bPinIndex(%d), lEventId(%d), bServiceIdx(%d)\n", pstOrderPin->bPinIndex, pstOrderPin->lEventId, pstOrderPin->bServiceIdx);
	HxLOG_Print ("* Result : eReason(%s)\n", VmxBc_PinReason_Type_Str[pstOrderPin->eReason]);	
	HxLOG_Print ("* Result : Credit_Buf = %s\n", pstOrderPin->Credit_Buf);
	HxLOG_Print ("* Result : Cost_Buf = %s\n", pstOrderPin->Cost_Buf);
}

void bc_ui_CheckPinDebugPrint(VmxBc_ISCCheckPin_t *pstCheckPin, HUINT8 *pMsg)
{
	if(pstCheckPin == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return;
	}

	HxLOG_Print ("*****************************************************\n");
	HxLOG_Print ("*\n");
	HxLOG_Print ("*\t < Request Parse : %s >\n", pMsg);
	HxLOG_Print ("* [%s:%d]\n", __FUNCTION__, __LINE__);
	HxLOG_Print ("* bPinIndex(%d)\n", pstCheckPin->bPinIndex);
	HxLOG_Print ("* bServiceIdx(%d)\n", pstCheckPin->bServiceIdx);
	HxLOG_Print ("* eReason(%s)\n", VmxBc_PinReason_Type_Str[pstCheckPin->eReason]);
	HxLOG_Print ("*\n");
	HxLOG_Print ("*****************************************************\n");
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// UTIL ////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// UTIL ////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// UTIL ////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
STATIC HUINT8 bc_ui_Str2Hex( HUINT8 *pStr )
{
	int i;
	HUINT8 c;
	HUINT8 hex = 0;
	HUINT8 medium[2];

	for ( i=0; i<2; i++ ) 
	{
		c = pStr[i];
		if ( (c>='0' && c<='9') ) 
		{
			medium[i] = c-'0';
		}
		else if ( (c>='a' && c<='f') ) 
		{
			medium[i] = c-'a'+0xa;
		}
		else if ( (c>='A' && c<='F') ) 
		{
			medium[i] = c-'A'+0xa;
		}
	}

	hex = medium[0]<<4 | medium[1];

	return hex;
}

STATIC HERROR bc_ui_ConvertFromStringToHex(HUINT8 *pucSource, HUINT16 ulSourceSize, HUINT8 *pucDes, HUINT16 *ulDesSize)
{
	HUINT32 ulIdx = 0;

	if(!pucSource || !pucDes)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	if(ulSourceSize <= 0 )
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	while(1)
	{
		pucDes[ulIdx] = bc_ui_Str2Hex(pucSource);
		pucSource = pucSource + 2;
		ulIdx++;
		if(ulIdx >= (ulSourceSize/2))
			break;
	}

	*ulDesSize = ulSourceSize/2;

	return ERR_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// OSM ////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// OSM //////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// OSM //////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VmxBc_Requst_t * bc_ui_GetBcRequest(void)
{
	return &stBcRequest;
}

VmxBc_Osm_t * bc_ui_GetBcOsmRequest(void)
{
	VmxBc_Requst_t *pstBcRequest =  bc_ui_GetBcRequest();

	// serviceIdx에 의해 hadle or others를 얻어낸다.

	if(pstBcRequest == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return NULL;
	}

	return &pstBcRequest->stOsm;
}

VmxBc_ISC_Pin_t * bc_ui_GetBcPinRequest(void)
{
	return &stBcRequest.stPin;
}

VmxBc_Osm_t * bc_ui_GetOsm(HUINT8 ucServiceIdx)
{
	VmxBc_Requst_t *pstBcRequest =  bc_ui_GetBcRequest();

	// serviceIdx에 의해 hadle or others를 얻어낸다.

	if(pstBcRequest == NULL || ucServiceIdx == 0xff)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return NULL;
	}

	return &pstBcRequest->stOsm;
}

VmxBc_ISCOrderPin_t * bc_ui_GetOrderPin(HUINT8 ucServiceIdx)
{
	VmxBc_Requst_t *pstBcRequest =  bc_ui_GetBcRequest();
	VmxBc_ISC_Pin_t *pstPin = NULL;

	// serviceIdx에 의해 hadle or others를 얻어낸다.

	if(pstBcRequest == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return NULL;
	}

	pstPin = &pstBcRequest->stPin;
	if(pstPin == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return NULL;
	}

	return &pstPin->stOrderPin;
}

VmxBc_ISCCheckPin_t * ui_GetCheckPin(HUINT8 ucServiceIdx)
{
	VmxBc_Requst_t *pstBcRequest =  bc_ui_GetBcRequest();
	VmxBc_ISC_Pin_t *pstPin = NULL;

	// serviceIdx에 의해 hadle or others를 얻어낸다.

	if(pstBcRequest == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return NULL;
	}

	pstPin = &pstBcRequest->stPin;
	if(pstPin == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return NULL;
	}

	return &pstPin->stCheckPin;
}



void bc_ui_SetOsmInit(void)
{
	VmxBc_Osm_t *pstOsm =  bc_ui_GetBcOsmRequest();
	VMX_memset(pstOsm, 0, sizeof(VmxBc_Osm_t));
}

HERROR bc_ui_SetTriggerInit(VmxBc_TriggerType_e eType)
{
	VmxBc_Osm_t *pstOsm =  bc_ui_GetBcOsmRequest();
	if(pstOsm == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	VMX_memset(&pstOsm->stTrigger[eType], 0, sizeof(VmxBc_Trigger_t));

	return ERR_OK;
}

void bc_ui_SetOrderPinInit(void)
{
	VmxBc_ISC_Pin_t *pstPin =  bc_ui_GetBcPinRequest();
	VMX_memset(&pstPin->stOrderPin, 0, sizeof(VmxBc_ISCOrderPin_t));
}

void bc_ui_SetCheckPinInit(void)
{
	VmxBc_ISC_Pin_t *pstPin =  bc_ui_GetBcPinRequest();
	VMX_memset(&pstPin->stCheckPin, 0, sizeof(VmxBc_ISCCheckPin_t));
}

void bc_ui_SetPinInit(void)
{
	VmxBc_ISC_Pin_t *pstPin =  bc_ui_GetBcPinRequest();
	VMX_memset(pstPin, 0, sizeof(VmxBc_ISC_Pin_t));
}


HERROR bc_ui_OsmTrigger_UpgradeParse(HUINT8*pabMsg, VmxBc_Trigger_t *pstTrigger, HBOOL *bSendFlag)
{
	HUINT16 usMsgLen = 0;
	HUINT16 usDescLen = 0;
	if(pabMsg == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	if(pstTrigger == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	pstTrigger->eTriggerType = eVmxBc_TriggerType_Upgrade;
	 *bSendFlag = FALSE;

	usMsgLen = strlen(pabMsg);
	usDescLen = usMsgLen - 19;

	if(pstTrigger->item.stUpgrade.stDesc.pDescriptor)
	{
		VMX_MEM_Free(pstTrigger->item.stUpgrade.stDesc.pDescriptor);
		pstTrigger->item.stUpgrade.stDesc.pDescriptor = NULL;
	}

	if(usDescLen > 0)
	{
		pstTrigger->item.stUpgrade.stDesc.pDescriptor = (HUINT8 *)VMX_MEM_Alloc(usDescLen/2+1);
		if(pstTrigger->item.stUpgrade.stDesc.pDescriptor)
		{
			bc_ui_ConvertFromStringToHex(&pabMsg[19], usDescLen, pstTrigger->item.stUpgrade.stDesc.pDescriptor, &pstTrigger->item.stUpgrade.stDesc.usLength);
		}
	}

	switch(pabMsg[2])
	{
		case OSM_U_FORCED_UPGRADE:
			HxLOG_Print(DBG_PRINT, ("OSM_U_FORCED_UPGRADE...........\n");
			pstTrigger->item.stUpgrade.eType = eVmxBc_UpgradeType_Forced;
			 *bSendFlag = TRUE;
			break;
		case OSM_U_UPGRADE_NOW	:
			HxLOG_Print(DBG_PRINT, ("OSM_U_UPGRADE_NOW...........\n");
			pstTrigger->item.stUpgrade.eType = eVmxBc_UpgradeType_Now;
			 *bSendFlag = TRUE;
			break;			
		case OSM_U_UPGRADE_NOW_AFTER_USER_CONFIRMATION:
			HxLOG_Print(DBG_PRINT, ("OSM_U_UPGRADE_NOW_AFTER_USER_CONFIRMATION...........\n");
			pstTrigger->item.stUpgrade.eType = eVmxBc_UpgradeType_NowAfterUserConfirmation;
			break;			
		case OSM_U_UPGRADE_POWER_DOWN:
			HxLOG_Print(DBG_PRINT, ("OSM_U_UPGRADE_POWER_DOWN...........\n");
			pstTrigger->item.stUpgrade.eType = eVmxBc_UpgradeType_PowerDown;
			break;			
		case OSM_U_UPGRADE_ON_POWER_DOWN_AFTER_USER_CONFIRMATION:
			HxLOG_Print(DBG_PRINT, ("OSM_U_UPGRADE_ON_POWER_DOWN_AFTER_USER_CONFIRMATION...........\n");
			pstTrigger->item.stUpgrade.eType = eVmxBc_UpgradeType_PowerDownAfterUserConfirmation;
			break;			
		case OSM_U_UPGRADE_POWER_UP:
			HxLOG_Print(DBG_PRINT, ("OSM_U_UPGRADE_POWER_UP...........\n");
			pstTrigger->item.stUpgrade.eType = eVmxBc_UpgradeType_PowerUp;
			break;			
		case OSM_U_UPGRADE_ON_POWER_UP_AFTER_USERCONFIRMATION:
			HxLOG_Print(DBG_PRINT, ("OSM_U_UPGRADE_ON_POWER_UP_AFTER_USERCONFIRMATION...........\n");
			pstTrigger->item.stUpgrade.eType = eVmxBc_UpgradeType_PowerUpAfterUserConfirmation;
			break;			
		case OSM_U_UPGRADE_ON_NEXT_SERVICE_CHANGE:
			HxLOG_Print(DBG_PRINT, ("OSM_U_UPGRADE_ON_NEXT_SERVICE_CHANGE...........\n");
			pstTrigger->item.stUpgrade.eType = eVmxBc_UpgradeType_OnNextServiceChange;
			break;			
		case OSM_U_UPGRADE_ON_NEXT_NON_DESCRAMBLING_PERIOD:
			HxLOG_Print(DBG_PRINT, ("OSM_U_UPGRADE_ON_NEXT_NON_DESCRAMBLING_PERIOD...........\n");
			pstTrigger->item.stUpgrade.eType = eVmxBc_UpgradeType_OnNextNonDescramblingPeriod;			
			break;
		default:
			HxLOG_Error(DBG_ASSERT, ("\n");
			return ERR_FAIL;
			break;
	}

	return ERR_OK;
}

HERROR bc_ui_OsmTrigger_RescanParse(HUINT8*pabMsg, VmxBc_Trigger_t *pstTrigger, HBOOL *bSendFlag)
{
	if(pabMsg == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	if(pstTrigger == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	pstTrigger->eTriggerType = eVmxBc_TriggerType_Rescan;

	*bSendFlag = FALSE;

	switch(pabMsg[2])
	{
		case OSM_R_RESCAN_NOW:
			pstTrigger->item.stRescan.eType = eVmxBc_RescanType_Now;
			break;
		case OSM_R_RESCAN_NOW_AFTER_USER_CONFIRMATION:
			pstTrigger->item.stRescan.eType = eVmxBc_RescanType_NowAfterUserConfirmation;
			break;
		case OSM_R_RESCAN_ON_POWER_DOWN:
			pstTrigger->item.stRescan.eType = eVmxBc_RescanType_PowerDown;			
			break;
		case OSM_R_RESCAN_ON_POWER_DOWN_AFTER_USER_CONFIRMATION:
			pstTrigger->item.stRescan.eType = eVmxBc_RescanType_PowerDownAfterUserConfirmation;			
			break;
		case OSM_R_RESCAN_ON_POWER_UP:
			pstTrigger->item.stRescan.eType = eVmxBc_RescanType_PowerUp;			
			break;
		case OSM_R_RESCAN_ON_POWER_UP_AFTER_USER_CONFIRMATION:
			pstTrigger->item.stRescan.eType = eVmxBc_RescanType_PowerUpAfterUserConfirmation;			
			break;
		case OSM_R_RESCAN_ON_NEXT_SERVICE_CHANGE:
			pstTrigger->item.stRescan.eType = eVmxBc_RescanType_OnNextServiceChange;			
			break;
		case OSM_R_RESCAN_ON_NEXT_NON_DESCRAMBLING_PERIOD:
			pstTrigger->item.stRescan.eType = eVmxBc_RescanType_OnNextNonDescramblingPeriod;			
			break;
		default:
			HxLOG_Error(DBG_ASSERT, ("\n");
			return ERR_FAIL;
			break;
	}

	return ERR_OK;
}

HERROR bc_ui_OsmTrigger_SwitchParse(HUINT8*pabMsg, VmxBc_Trigger_t *pstTrigger, HBOOL *bSendFlag)
{
	if(pabMsg == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	if(pstTrigger == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	pstTrigger->eTriggerType = eVmxBc_TriggerType_Switch;
	*bSendFlag = FALSE;

	switch(pabMsg[2])
	{
		case OSM_S_SWITCH_SERVICE_NOW:
			pstTrigger->item.stSwitch.eType = eVmxBc_SwitchType_ServiceNow;
			break;
		case OSM_S_SWITCH_AT_TIME:
			pstTrigger->item.stSwitch.eType = eVmxBc_SwitchType_AtTime;
			break;
		case OSM_S_SWITCH_FOR_TIMED_PERIOD	:
			pstTrigger->item.stSwitch.eType = eVmxBc_SwitchType_ForTimePeriod;			
			break;
		case OSM_S_SWITCH_AT_TIME_FOR_TIMED_PERIOD:
			pstTrigger->item.stSwitch.eType = eVmxBc_SwitchType_AtTimeForTimePeriod;			
			break;
		case OSM_S_SWITCH_IF_SERVICE_CANNOT_BE_DESCRAMBLED	:
			pstTrigger->item.stSwitch.eType = eVmxBc_SwitchType_IfServiceCannotBeDescrambled;			
			break;
		case OSM_S_SWITCH_UNTIL_TIME_REACHED:
			pstTrigger->item.stSwitch.eType = eVmxBc_SwitchType_UntilTimeReached;			
			break;
		case OSM_S_SWITCH_BACK_NOW:
			pstTrigger->item.stSwitch.eType = eVmxBc_SwitchType_BackNow;			
			break;
		case OSM_S_SWITCH_BACK_IN_X_SECONDS:
			pstTrigger->item.stSwitch.eType = eVmxBc_SwitchType_BackInXSeconds;			
			break;
		case OSM_S_SWITCH_BACK_AT_TIME:
			pstTrigger->item.stSwitch.eType = eVmxBc_SwitchType_BackAtTime;			
			break;
		default:
			HxLOG_Error(DBG_ASSERT, ("\n");
			return ERR_FAIL;
			break;
	}

	return ERR_OK;
}

HERROR bc_ui_OsmTrigger_PinNumberReset(HUINT8*pabMsg, VmxBc_Trigger_t *pstTrigger, HBOOL *bSendFlag)
{	
	HUINT8 Pin1[2] = {0,0};
	HUINT8 Pin2[2]= {0,0};
	HUINT8 Pin3[2]= {0,0};
	HUINT8 Pin4[2]= {0,0};

	* bSendFlag = FALSE;
	if(pabMsg == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	if(pstTrigger == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	pstTrigger->eTriggerType = eVmxBc_TriggerType_PinNumberReset;

	VMX_memcpy(Pin1, &pabMsg[2], 1);
	VMX_memcpy(Pin2, &pabMsg[3], 1);
	VMX_memcpy(Pin3, &pabMsg[4], 1);
	VMX_memcpy(Pin4, &pabMsg[5], 1);	

	pstTrigger->item.stPinNumberReset.ucPinNumberBuf[0] = atoi(Pin1);
	pstTrigger->item.stPinNumberReset.ucPinNumberBuf[1] = atoi(Pin2);
	pstTrigger->item.stPinNumberReset.ucPinNumberBuf[2] = atoi(Pin3);
	pstTrigger->item.stPinNumberReset.ucPinNumberBuf[3] = atoi(Pin4);

	HxLOG_Print(DBG_PRINT, ("length(%d) : STB Pin = %d%d%d%d\n", strlen(pabMsg), 
						pstTrigger->item.stPinNumberReset.ucPinNumberBuf[0],
						pstTrigger->item.stPinNumberReset.ucPinNumberBuf[1],
						pstTrigger->item.stPinNumberReset.ucPinNumberBuf[2],
						pstTrigger->item.stPinNumberReset.ucPinNumberBuf[3]);

	// 만일 필요하다면 pinNumberReset String을 display하자.
	// STB PIN
	* bSendFlag = TRUE;

	return ERR_OK;
}

HERROR bc_ui_OsmTrigger_Watermarking(HUINT8*pabMsg, VmxBc_Trigger_t *pstTrigger, HBOOL *bSendFlag)
{	
	
	HUINT8 	SCSerialNumStr[40];
	HUINT8	secondStr[10];
	HUINT8	x_frameStr[10];
	HUINT8  y_frameStr[10];
	HUINT8  length = 0;//strlen(pBuf);
	HUINT16 SNLength = 0;

	*bSendFlag = FALSE;

	if(pabMsg == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	if(pstTrigger == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	pstTrigger->eTriggerType = eVmxBc_TriggerType_Watermarking;
	length = strlen(pabMsg);
	

	VMX_memset(&pstTrigger->item.stWatermarking, 0, sizeof(VmxBc_Watermarking_t));
	VMX_memset(SCSerialNumStr, 0, sizeof(SCSerialNumStr));
	VMX_memset(secondStr, 0, sizeof(secondStr));
	VMX_memset(x_frameStr, 0, sizeof(x_frameStr));
	VMX_memset(y_frameStr, 0, sizeof(y_frameStr));	

	HxLOG_Print(DBG_PRINT, ("Watermarking...........\n");

	if(length <= 2)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	if(length > 2 && length <= 10)	// Second only
	{
		VMX_memcpy(secondStr, &pabMsg[2], 8);
		pstTrigger->item.stWatermarking.usDuration = strtoul(secondStr, NULL, 16);

	}
	else if(length > 10)	// with x frame and y frame : XXXXYY
	{
		VMX_memcpy(secondStr, &pabMsg[2], 8);
		VMX_memcpy(x_frameStr, &pabMsg[10], 4);
		VMX_memcpy(y_frameStr, &pabMsg[14], 2);

		pstTrigger->item.stWatermarking.usDuration = strtoul(secondStr, NULL, 16);
		pstTrigger->item.stWatermarking.x_frame = strtoul(x_frameStr, NULL, 16);
		pstTrigger->item.stWatermarking.y_frame = strtoul(y_frameStr, NULL, 16);
	}

	HxLOG_Print(DBG_PRINT, ("length(%d), dur(%s)(%d), xframe(%s)(%d), yframe(%s)(%d)\n", length, 
		secondStr, pstTrigger->item.stWatermarking.usDuration, x_frameStr, pstTrigger->item.stWatermarking.x_frame, y_frameStr, pstTrigger->item.stWatermarking.y_frame);

	if(vmx_vr_bc_api_GetSCNo(SCSerialNumStr, MAX_SC_NUMBER) == ERR_OK)
	{
		#if 0//CONFIG_DEBUG
		sprintf(SCSerialNumStr, "%s", "SerialNUmber-Test ABCDE"); // tes only
		#endif
		
		SNLength = strlen(SCSerialNumStr);
		HxLOG_Print(DBG_PRINT, ("SNLength(%d)\n", SNLength);
		if(SNLength > 0 )
		{
			//if(vmx_bc_ui_ConvertDecimalToHex(SCSerialNumStr, SNLength, pstTrigger->item.stWatermarking.ucSCSerialNum, &size) == ERR_OK)
			VMX_memcpy(pstTrigger->item.stWatermarking.ucSCSerialNumStr, SCSerialNumStr, SNLength);
			{
				*bSendFlag = TRUE;
				HxLOG_Print(DBG_PRINT, ("Serial Number[%d] = (%s)\n", SNLength, pstTrigger->item.stWatermarking.ucSCSerialNumStr);
				return ERR_OK;
			}
		}
	}	

// just for testing
#if 0
    *bSendFlag = TRUE;
	snprintf(pstTrigger->item.stWatermarking.ucSCSerialNumStr, 40, "C0DX-O387-DGADFV");
	HxLOG_Print(DBG_PRINT, ("Serial Number[%d] = (%s)\n", SNLength, pstTrigger->item.stWatermarking.ucSCSerialNumStr);
	return ERR_OK;
#endif
	HxLOG_Error(DBG_ASSERT, ("\n");
	return ERR_FAIL;
}

HERROR bc_ui_OsmOsdParse(VmxBc_OSD_t * pstOsd, HUINT8*pabMsg, HINT16 wMode, HINT16 wX, HINT16 wY, HINT16 wW, HINT16 wH, HINT8 bBackground, HINT8 bAlpha, HINT8 bForeGround)
{
	VmxBc_BuildWindow_t *pBWin = NULL;
	HUINT16 x_pos;
	HUINT16 y_pos;
	HUINT16 text_alignment;
	HUINT32 ulMsgLen = 0;
	
	if(pstOsd == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	ulMsgLen = MWC_UTIL_DvbStrlen(pabMsg);
	if(ulMsgLen >= sizeof(pstOsd->buf))
	{
		ulMsgLen = sizeof(pstOsd->buf) - 1;
	}
	
	HxLOG_Print(DBG_PRINT, ("\rOSD : Length(%d), wMode(%d), wX(%d), wY(%d), wW(%d), wH(%d), bBackground(%d), bAlpha(%d), bForeGround(%d)\n", ulMsgLen, wMode, wX, wY, wW, wH, bBackground, bAlpha, bForeGround);

	VMX_memcpy(pstOsd->buf, pabMsg, ulMsgLen);
    pstOsd->buf[ulMsgLen] = '\0';
	
	pBWin = &pstOsd->stBuildWindow;

	if(pBWin == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;	
	}

	x_pos = (wMode/4) & 3;
	pBWin->stMode.x_pos = eVmxBc_Osm_X_Pos_Left;
	if((x_pos&1) == 1) // centered point
		pBWin->stMode.x_pos = eVmxBc_Osm_X_Pos_Center;
	if((x_pos&2) == 2) // right
		pBWin->stMode.x_pos = eVmxBc_Osm_X_Pos_Right;		

	y_pos = wMode & 3;
	pBWin->stMode.y_pos = eVmxBc_Osm_Y_Pos_Bottom;
	if((y_pos&1) == 1) // centered point
		pBWin->stMode.y_pos = eVmxBc_Osm_Y_Pos_Center;
	if((y_pos&2) == 2) // upper
		pBWin->stMode.y_pos = eVmxBc_Osm_Y_Pos_Upper;

	text_alignment = (wMode/16) & 3;
	if(text_alignment==0) // left aligned
		pBWin->stMode.text_alig = eVmxBc_Osm_Text_Alignment_LeftAligned;
	else if(text_alignment==1) // right aligned
		pBWin->stMode.text_alig = eVmxBc_Osm_Text_Alignment_RightAligned;
	else if(text_alignment==2) // centered
		pBWin->stMode.text_alig = eVmxBc_Osm_Text_Alignment_Centered;
	else if(text_alignment==3) // justified
		pBWin->stMode.text_alig = eVmxBc_Osm_Text_Alignment_Justified;

	pBWin->stScreen.x = wX;
	pBWin->stScreen.y = wY;
	pBWin->stScreen.w = wW;
	pBWin->stScreen.h =  wH;
	if((wMode&0x100) != 0x100) // width, height를 사용하지 않는경우.
	{
		pBWin->stScreen.w = 0;
		pBWin->stScreen.h =  0;
	}

	pBWin->ucBg = bBackground;
	pBWin->ucAlpha = bAlpha;
	pBWin->ucFg = bForeGround;

	pstOsd->eOsmStatus = eVmxBc_OsmStatus_BuildWindow;		

	return ERR_OK;
}

HERROR vmx_vr_bc_ui_SetOsdBuildWindow(HUINT8*pabMsg, HINT16 wMode, HINT16 wX, HINT16 wY, HINT16 wW, HINT16 wH, HINT8 bBackground, HINT8 bAlpha, HINT8 bForeGround)
{
	VmxBc_Requst_t *pstRequest = bc_ui_GetBcRequest();
	VmxBc_Osm_t *pstOsm = NULL;//bc_ui_GetOsm(0);
	VmxBc_TriggerType_e eTriggerType;
	HBOOL bSendFlag = 0;
 
	if(pstRequest == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	if(pabMsg == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	// 해당 함수가 call되면 무조건 init를 해주자.
	//bc_ui_SetOsmInit();

	pstRequest->eType = eVmxBc_Bc_Osm;
	pstOsm = &pstRequest->stOsm;
	if(pstOsm == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	if(pabMsg[0] == BC_OSM_TRIGGER)
	{
		HxLOG_Print(DBG_PRINT, ("TRIGGER\n");

		pstOsm->eOsmType = eVmxBc_OsmType_Trigger;
		//pstOsm->eTriggerType = eVmxBc_TriggerType_Max; // default로 max로 .
		switch(pabMsg[1])
		{
			case BC_OSM_TRIGGER_UPGRADE:
				eTriggerType = eVmxBc_TriggerType_Upgrade;
				if(bc_ui_OsmTrigger_UpgradeParse(pabMsg, &pstOsm->stTrigger[eTriggerType], &bSendFlag) == ERR_FAIL)
				{
					HxLOG_Error(DBG_ASSERT, ("\n");
					return ERR_FAIL;
				}
				break;
			case BC_OSM_TRIGGER_RESCAN:
				#if 1 // TODO : Future
				eTriggerType = eVmxBc_TriggerType_Rescan;
				if(bc_ui_OsmTrigger_RescanParse(pabMsg, &pstOsm->stTrigger[eTriggerType], &bSendFlag) == ERR_FAIL)
				{
					HxLOG_Error(DBG_ASSERT, ("\n");
					return ERR_FAIL;
				}
				#else
				return ERR_FAIL;
				#endif
				break;
			case BC_OSM_TRIGGER_SWITCH:
				#if 1 // TODO : Future
				eTriggerType = eVmxBc_TriggerType_Switch;
				if(bc_ui_OsmTrigger_SwitchParse(pabMsg, &pstOsm->stTrigger[eTriggerType], &bSendFlag) == ERR_FAIL)
				{
					HxLOG_Error(DBG_ASSERT, ("\n");
					return ERR_FAIL;
				}
				#else
				return ERR_FAIL;
				#endif
				break;
			case BC_OSM_TRIGGER_WATERMARKING:
				eTriggerType = eVmxBc_TriggerType_Watermarking;
				if(bc_ui_OsmTrigger_Watermarking(pabMsg,  &pstOsm->stTrigger[eTriggerType], &bSendFlag) == ERR_FAIL)
				{
					HxLOG_Error(DBG_ASSERT, ("\n");
					return ERR_FAIL;
				}
				break;
			case BC_OSM_TRIGGER_PINNUMBERRESET:
				eTriggerType = eVmxBc_TriggerType_PinNumberReset;
				if(bc_ui_OsmTrigger_PinNumberReset(pabMsg, &pstOsm->stTrigger[eTriggerType], &bSendFlag) == ERR_FAIL)
				{
					HxLOG_Error(DBG_ASSERT, ("\n");
					return ERR_FAIL;
				}
				break;
			case BC_OSM_TRIGGER_EAS:
				eTriggerType = eVmxBc_TriggerType_Eas;
				HxLOG_Error(DBG_ASSERT, ("BC_OSM_TRIGGER_EAS : Todo\n");
				return ERR_FAIL;
				break;
			default:
				HxLOG_Error(DBG_ASSERT, ("BC_OSM_TRIGGER : Error(triggere is not parse)\n");
				return ERR_FAIL;
				break;
		}

		pstOsm->eTriggerType = eTriggerType;

		if(bSendFlag == TRUE)
		{
			// osm은 일단 live 로 생각해서 구현하자, 
			if(xsvc_cas_vmx_UpdateCasMessage(xsvc_vmx_main_GetLiveActionHandle(), eCasMsgType_Command, eGroupID_VMXBC, (void*)pstRequest, eCasShowType_Both) != ERR_OK)
			{
				HxLOG_Error(DBG_ASSERT, ("\n");
				return ERR_FAIL;		
			}
		}
	}
	else
	{
		pstOsm->eOsmType = eVmxBc_OsmType_Osd;
		bc_ui_OsmOsdParse(&pstOsm->stOSD, pabMsg, wMode, wX, wY, wW, wH, bBackground, bAlpha, bForeGround);
	}

#ifdef CONFIG_DEBUG
	vmx_ui_DebugPrintforRequest(pstRequest, pstOsm->eOsmType == eVmxBc_OsmType_Trigger ? "ViewRight Bc Trigger" : "ViewRight Bc OSD");
#endif

	return ERR_OK;
}

HERROR vmx_vr_bc_ui_SetOsdDisplayWindow(HUINT8 bDisplayMode, HUINT16 wDuration)
{	
	VmxBc_Requst_t *pstRequest = bc_ui_GetBcRequest();
	VmxBc_OSD_t * pstOsd = NULL;
	VmxBc_DisplayWindow_t *pstDWin = NULL;
	CAS_MsgType_t		eMsgType = eCasMsgType_Ok;
	CAS_ShowType_t	eShowType = eCasShowType_Both;

	if(pstRequest == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	pstRequest->eType = eVmxBc_Bc_Osm;

	pstOsd = &pstRequest->stOsm.stOSD;

	if(pstOsd == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}


	if(pstOsd->eOsmStatus != eVmxBc_OsmStatus_BuildWindow) // OSD_BuildWindow()가 호출이 된다음에 진행되어야함.
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	pstDWin = &pstOsd->stDisplayWindow;

	if(pstDWin == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}

	pstDWin->eType = (VmxBc_DisplayWindowType_e)bDisplayMode;
	if((bDisplayMode & 1) == 0) /* the window can be removed by the user */
	{
		pstDWin->eMode = eVmxBc_DisplayWindow_RemoveByUser;
	}
	else
	{
		pstDWin->eMode = eVmxBc_DisplayWindow_NoRemoveByUser;
	}
	/* If 0 is given, the window will displayed until a new On Screen Message is to be displayed or the user has removed the window */
	pstDWin->usDuration = wDuration;
	pstOsd->eOsmStatus = eVmxBc_OsmStatus_DisplayWindow;

#ifdef CONFIG_DEBUG
	vmx_ui_DebugPrintforRequest(pstRequest, "ViewRight Bc OSD");
#endif
	eMsgType = eCasMsgType_Command;
	eShowType = eCasShowType_Both;

	// osm은 일단 live 로 생각해서 구현하자, 
	if(xsvc_cas_vmx_UpdateCasMessage(xsvc_vmx_main_GetLiveActionHandle(), eMsgType, eGroupID_VMXBC, (void*)pstRequest, eShowType) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// PIN //////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// PIN //////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// PIN //////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void bc_ui_strreverse(char* begin, char* end) {
	char aux;
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}
static void bc_ui_itoa(int value, char* str, int base) {
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
	int sign;
	div_t res;

	// Validate base
	if (base<2 || base>35){ *wstr='\0'; return; }

	// Take care of sign
	if ((sign=value) < 0) value = -value;
	// Conversion. Number is reversed.
	do {
		res = div(value,base);
		*wstr++ = num[res.rem];
	}while((value=res.quot));
	if(sign<0) *wstr++='-';	*wstr='\0';

	// Reverse string
	bc_ui_strreverse(str,wstr-1);
}

static HUINT16 bc_ui_pow(HUINT16 x, HUINT16 y)
{
	int i;
	HUINT16 rst = 1;

	for(i=0; i<y; i++)
	{
		rst *= x;
	}

	return rst;
}

void bc_ui_SetClarifyNumber(HUINT8 *str, HUINT8 *buf, HUINT8 bSign1000, HUINT8 bCount) 
{ 
        int len;

	if(str == NULL || buf == NULL)
	{
		return;
	}

	if(bCount == 0)
	{
		sprintf(buf, "%s", str);
		return;
	}

        len = strlen(str); 

        while ((*buf++ = *str++)) { 
                if (--len && (len % 3) == 0) 
                        *buf++ = bSign1000; 
        }        
}

void vmx_vr_bc_ui_SetCheckPin(HUINT8 bPinIndex, HUINT8 bTextSelector, HUINT8 bServiceIdx)
{
	VmxBc_Requst_t *pstRequest = bc_ui_GetBcRequest();	
	VmxBc_ISCCheckPin_t *pstCheckPin = NULL;//ui_GetCheckPin(bServiceIdx);		
	CAS_MsgType_t		eMsgType = eCasMsgType_Ok;
	CAS_ShowType_t	eShowType = eCasShowType_Both;

	if(pstRequest == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return;
	}

	pstRequest->eType = eVmxBc_Bc_Pin;
	pstRequest->stPin.eType = eVmxBc_PinType_Check;
	pstCheckPin = &pstRequest->stPin.stCheckPin;

	pstCheckPin->bPinIndex = bPinIndex;
	pstCheckPin->bServiceIdx = bServiceIdx;

	if(bTextSelector == 0)
		pstCheckPin->eReason = eVmxBc_PinReason_PC;
	else if(bTextSelector == 1)
		pstCheckPin->eReason = eVmxBc_PinReason_IPPV;
	else if(bTextSelector == 3)
		pstCheckPin->eReason = eVmxBc_PinReason_PC_UsingNonSmartcardPin;
	else if(bTextSelector == 4)
		pstCheckPin->eReason = eVmxBc_PinReason_ResumingOfEvent;
	else if(bTextSelector == 5)
		pstCheckPin->eReason = eVmxBc_PinReason_SelectiingOfEvent;
	else
	{
		HxLOG_Error(DBG_ASSERT, ("bTextSelector is not exist\n");
	}

	// TODO_VMX_UI
#ifdef CONFIG_DEBUG
	bc_ui_CheckPinDebugPrint(pstCheckPin, "ViewRight Bc Chieck Pin");
#endif
	// TODO_VMX_UI 여기서 bc_ui_SetPinInit or SC에서 모든 notifiy를 확인한후.

	eMsgType = eCasMsgType_Command;
	eShowType = eCasShowType_Both;

	if(xsvc_cas_vmx_UpdateCasMessage(/*xsvc_vmx_main_GetLiveActionHandle()*/xsvc_vmx_main_GetActionHandleFromServiceIndex(bServiceIdx), eMsgType, eGroupID_VMXBC, (void*)pstRequest, eShowType) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
	}
}

HERROR vmx_vr_bc_ui_SetPinVerificationToBC(VmxBc_VerificationPin_t *pVPin)
{
	
	if(pVPin == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("pVPin is NULL\n");		
		return ERR_FAIL;
	}
	// TODO_VMX_UI : 0xFF, 0xFE 이게 뭐지? internal STB for Smart Card와 internal STB for Non-Smart Card이 언제 호출되는지 확인 필요해 보임.
	/*
		Reason, why pin check is performed:
		0xFF: internal STB for Smart Card
		0xFE: internal STB for Non-Smart Card
		others: requested by CAM-library
		using ISC_CheckPin. Parameter bTextSelector is given here

		아래 notifiy가 언제, 어디서 이루어지는지 확인해 보자.
		Notify values of BC_CheckPin():
		k_BcPinVerified = 0x00
		k_BcPinFailure = 0x21
		k_BcPinBlocked = 0x22
		k_BcPinBadIndex = 0x25
		k_BcPinBadLength = 0x26
		k_BcPinGeneralError = 0xFF		
	*/
	HxLOG_Print(DBG_PRINT, ("* Verification Pin : ucPinLength(%d)\n",  pVPin->stBcdPin.ucPinLength);
	HxLOG_Print(DBG_PRINT, ("* Verification Pin : ucPin = %d%d%d%d\n",  pVPin->stBcdPin.ucPin[0], pVPin->stBcdPin.ucPin[1], pVPin->stBcdPin.ucPin[2], pVPin->stBcdPin.ucPin[3]));
	HxLOG_Print(DBG_PRINT, ("* Verification Pin : ucPinIndex(%d)\n",  pVPin->ucPinIndex);
	HxLOG_Print(DBG_PRINT, ("* Verification Pin : ucServiceIdx(%d)\n",  pVPin->ucServiceIdx);	
	HxLOG_Print(DBG_PRINT, ("* Reason : %s\n", VmxBc_PinReason_Type_Str[pVPin->ucReason]));

	if(pVPin->ucReason == eVmxBc_PinReason_InternalSTBForSC || pVPin->ucReason == eVmxBc_PinReason_InternalSTBForNSC)
	{
		pVPin->ucServiceIdx = 0;
	}

	if(vmx_vr_bc_api_CheckPin(pVPin->stBcdPin.ucPinLength, pVPin->stBcdPin.ucPin, pVPin->ucPinIndex, pVPin->ucReason, pVPin->ucServiceIdx) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("BC_CheckPin is Error\n");
	}

	return ERR_OK;
}


void vmx_vr_bc_ui_SetOrderPin(HUINT8 bPinIndex, HUINT32 lPurse, HUINT32 lCost, HUINT16 wMult, HUINT16 wDiv, HUINT8 bLocation, HUINT8 bSign0, HUINT8 bSign1000, HUINT8 bCount, HUINT8 bLen, HUINT8 *abText, HUINT32 lEventId, HUINT8 bServiceIdx)
{
	int i;
	double Credit_Value = 0; // 몫 + 나머지
	double Credit_Quotient_Val = 0; // 몫
	double Credit_Remainder_TempVal = 0; // 나머지
	HUINT32 Credit_Remainder_Val = 0; // 나머지	
	HUINT8 Credit_QuotientBuf[100];	// 우선 임시로 100으로 설정.. -	TODO_VMX_UI  현재는 임의의 100인데 추가작업 필요.
	HUINT8 Credit_bSign1000QuotientBuf[100];
	HUINT8 Credit_RemainderBuf[100];	

	double Cost_Value = 0;
	double Cost_Quotient_Val = 0;
	double Cost_Remainder_TempVal = 0;
	HUINT32 Cost_Remainder_Val = 0;	
	HUINT8 Cost_QuotientBuf[100];
	HUINT8 Cost_bSign1000QuotientBuf[100];	
	HUINT8 Cost_RemainderBuf[100];
	HUINT8 TEXT[100];


	VmxBc_Requst_t *pstRequest = bc_ui_GetBcRequest();	
	VmxBc_ISCOrderPin_t *pstOrderPin = NULL;//bc_ui_GetOrderPin(bServiceIdx);
	CAS_MsgType_t		eMsgType = eCasMsgType_Ok;
	CAS_ShowType_t	eShowType = eCasShowType_Both;

	if(pstRequest == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
		return;
	}

	pstRequest->eType = eVmxBc_Bc_Pin;
	pstRequest->stPin.eType = eVmxBc_PinType_Order;
	pstOrderPin =&pstRequest->stPin.stOrderPin;


	HxLOG_Print(DBG_PRINT, ("* bPinIndex(%d), lPurse(%d), lCost(%d), wMult(%d), wDiv(%d), bLocation(%d), bSign0(%c), bSign1000(%c), bCount(%d), bLen(%d), abText(%s), lEventId(%d), bServiceIdx(%d)\n",
		bPinIndex, lPurse, lCost, wMult, wDiv, bLocation, bSign0, bSign1000, bCount, bLen, abText, lEventId, bServiceIdx);

	if(pstOrderPin == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("pstOrderPin is NULL\n");
		return;
	}

	VMX_memset(TEXT, 0, sizeof(TEXT));	
	if(abText == NULL || bLen == 0)
	{
		sprintf(TEXT, "%s", "");
		HxLOG_Error(DBG_ASSERT, ("abText is NULL\n");
	}
	else
	{
		sprintf(TEXT, "%s", abText);
	}

	VMX_memset(pstOrderPin->Credit_Buf, 0, sizeof(pstOrderPin->Credit_Buf));
	VMX_memset(pstOrderPin->Cost_Buf, 0, sizeof(pstOrderPin->Cost_Buf));
	VMX_memset(Credit_QuotientBuf, 0, sizeof(Credit_QuotientBuf));	
	VMX_memset(Credit_RemainderBuf, 0, sizeof(Credit_RemainderBuf));
	VMX_memset(Credit_bSign1000QuotientBuf, 0, sizeof(Credit_bSign1000QuotientBuf));	
	VMX_memset(Cost_QuotientBuf, 0, sizeof(Cost_QuotientBuf));
	VMX_memset(Cost_RemainderBuf, 0, sizeof(Cost_RemainderBuf));
	VMX_memset(Cost_bSign1000QuotientBuf, 0, sizeof(Cost_bSign1000QuotientBuf));	

	Credit_Value =  (double)lPurse * wMult / wDiv; // credit 목, 나머지 출력
	Credit_Quotient_Val = lPurse * wMult / wDiv; // credit 몫 출력
	Credit_Remainder_TempVal = Credit_Value - Credit_Quotient_Val; // credit 나머지 소수점 이하로 출력
	Credit_Remainder_Val = (HUINT32)(Credit_Remainder_TempVal * bc_ui_pow(10, bCount)); // credit 나머지를 정수로 표현
	if(Credit_Remainder_Val == 0)
	{
		for(i=0;i<bCount;i++)
		{
			Credit_RemainderBuf[i] = '0'; // count만큼 소숫점 이하를 0으로 채운다.
		}
	}
	else
	{
	 	bc_ui_itoa( Credit_Remainder_Val, Credit_RemainderBuf, 10 ); // credit 나머지를 정수로 변환된것을 문자열로 변경.
	}

	Cost_Value =  (double)lCost * wMult / wDiv; // cost 목, 나머지 출력
	Cost_Quotient_Val = lCost * wMult / wDiv; // cost 몫 출력
	Cost_Remainder_TempVal = Cost_Value - Cost_Quotient_Val; // cost 나머지 소수점 이하로 출력
	Cost_Remainder_Val = (HUINT32)(Cost_Remainder_TempVal * bc_ui_pow(10, bCount));  // cost 나머지를 정수로 표현
	if(Cost_Remainder_Val == 0)
	{		
		for(i=0;i<bCount;i++)
		{
			Cost_RemainderBuf[i] = '0'; // count만큼 소숫점 이하를 0으로 채운다.
		}
	}
	else
	{
		bc_ui_itoa( Cost_Remainder_Val, Cost_RemainderBuf, 10 ); // cost  나머지를 정수로 변환된것을 문자열로 변경.
	}

	bc_ui_itoa( Credit_Quotient_Val, Credit_QuotientBuf, 10 );
	bc_ui_SetClarifyNumber(Credit_QuotientBuf, Credit_bSign1000QuotientBuf, bSign1000, bCount);
	bc_ui_itoa( Cost_Quotient_Val, Cost_QuotientBuf, 10 );
	bc_ui_SetClarifyNumber(Cost_QuotientBuf, Cost_bSign1000QuotientBuf, bSign1000, bCount);

	if(bLocation != 0)
	{		
		sprintf(pstOrderPin->Credit_Buf, "%s%s%c%s", abText,Credit_bSign1000QuotientBuf, bSign0,Credit_RemainderBuf);	
		sprintf(pstOrderPin->Cost_Buf, "%s%s%c%s",abText,Cost_bSign1000QuotientBuf,bSign0,Cost_RemainderBuf);
	}
	else // str을 뒤
	{
		sprintf(pstOrderPin->Credit_Buf, "%s%c%s%s", Credit_bSign1000QuotientBuf,bSign0,Credit_RemainderBuf, abText);
		sprintf(pstOrderPin->Cost_Buf, "%s%c%s%s", Cost_bSign1000QuotientBuf,bSign0,Cost_RemainderBuf, abText);				
	}

	eMsgType = eCasMsgType_Command;
	pstOrderPin->bPinIndex = bPinIndex;
	pstOrderPin->lEventId = lEventId;
	pstOrderPin->bServiceIdx = bServiceIdx;
	pstOrderPin->eReason = eVmxBc_PinReason_IPPV;	

	HxLOG_Print(DBG_PRINT, ("* TEXT = %s\n", TEXT);
	HxLOG_Print(DBG_PRINT, ("* CREDIT :  Credit_Value : %f  %f + %f(%d)\n", Credit_Value, Credit_Quotient_Val, Credit_Remainder_TempVal, Credit_Remainder_Val);
	HxLOG_Print(DBG_PRINT, ("* COST :  Credit_Value = %f  : %f + %f(%d)\n", Cost_Value, Cost_Quotient_Val, Cost_Remainder_TempVal, Cost_Remainder_Val);
	HxLOG_Print(DBG_PRINT, ("* bPinIndex(%d)\n", bPinIndex);
	HxLOG_Print(DBG_PRINT, ("* lEventId(%d)\n", lEventId);
	HxLOG_Print(DBG_PRINT, ("* bServiceIdx(%d)\n", bServiceIdx);
	HxLOG_Print(DBG_PRINT, ("* eReason(%d)\n", pstOrderPin->eReason);
	HxLOG_Print(DBG_PRINT, ("* OUTPUT : bLocation[%d] Credit_Buf(%s), Cost_Buf(%s)\n", bLocation, pstOrderPin->Credit_Buf, pstOrderPin->Cost_Buf);

	// TODO_VMX_UI
#ifdef CONFIG_DEBUG
	bc_ui_OrderPinDebugPrint(pstOrderPin, bPinIndex,  lPurse, lCost, wMult, wDiv, bLocation, bSign0, bSign1000, bCount, bLen, abText, lEventId, bServiceIdx);
#endif
	// TODO_VMX_UI 여기서 bc_ui_SetPinInit or SC에서 모든 notifiy를 확인한후.
	if(xsvc_cas_vmx_UpdateCasMessage(/*xsvc_vmx_main_GetLiveActionHandle()*/xsvc_vmx_main_GetActionHandleFromServiceIndex(bServiceIdx), eMsgType, eGroupID_VMXBC, (void*)pstRequest, eShowType) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("\n");
	}	
}

void vmx_vr_bc_ui_SetPinChangeToBC(VmxBc_ChangePin_t *pChPin)
{
	if(pChPin == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("pVPin is NULL\n");		
		return;
	}

	HxLOG_Print(DBG_PRINT, ("* Old Pin : ucPinLength(%d), Pin(%s)\n",  pChPin->stOldPin.ucPinLength, pChPin->stOldPin.ucPin);
	HxLOG_Print(DBG_PRINT, ("* New Pin : ucPinLength(%d), Pin(%s)\n",  pChPin->stNewPin.ucPinLength, pChPin->stNewPin.ucPin);
	HxLOG_Print(DBG_PRINT, ("* Pin Index = %s\n",VmxBc_PinIndex_Str[pChPin->ePinIndex]);

	if(pChPin->ePinIndex == eVmxBc_PinIndex_PC_PIN2 || pChPin->ePinIndex  == eVmxBc_PinIndex_IPPC_PC_PIN1)
	{
		HxLOG_Error(DBG_ASSERT, ("Pin Index is not exist\n");
		return;
	}

	if(vmx_vr_bc_api_ChangePin(pChPin->stOldPin.ucPinLength, pChPin->stOldPin.ucPin, pChPin->stNewPin.ucPinLength, pChPin->stNewPin.ucPin, pChPin->ePinIndex) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("BC_ChangePin is Error\n");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// Descrambling status///////////////////////////////////////////////////////////
///////////////////////////////////////////////////// Descrambling status///////////////////////////////////////////////////////////
///////////////////////////////////////////////////// Descrambling status///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HERROR vmx_vr_bc_ui_SetDescramblingState(HUINT16 wIndex, HUINT16* pawStreamPid, void* penDescState, HUINT8 bServiceIdx)
{
	int i;
//	enDescState_t *peDescState =  (enDescState_t *)penDescState;
	// TODO_VMX : 어디에 사용할지 ???	
	HxLOG_Print(DBG_PRINT, ("\rwIndex(%d), bServiceIdx(%d)\n", wIndex, bServiceIdx);
#ifdef CONFIG_DEBUG
	for(i=0;i<wIndex;i++)
	{
//		HxLOG_Print ("[%d] : Pid(0x%x) - DscState(%s)\n", wIndex, pawStreamPid[i], VmxBc_DescState_str[peDescState[i]]);
	}
#endif
	return ERR_OK;
}

HERROR vmx_vr_ui_Init(void)
{
	VMX_memset(&stBcRequest, 0, sizeof(VmxBc_Requst_t));

	return ERR_OK;
}

