/**
	@file     ap_cas_cx_pincode.c
	@brief    file_name & simple comment.

	Description: CONAX Pin code API  \n
	Module: Application(Layer), AP_CORE(module), CAS(module), CONAX(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/30		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include "local_include/_xmgr_cas_cx_util.h"
#include "local_include/_xmgr_cas_cx_adapt.h"
#include <conax.h>
#include <cx_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#if defined(WIN32)
STATIC	HUINT32		s_ulSCPinCode = 1234;
#endif

/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/

STATIC HERROR	xmgr_cas_CxCheckSmartCardPin (CxUiMsgType_t	eUiMsgType, Handle_t hSvcHandle, HUINT8* szPincode);
STATIC HUINT32	xmgr_cas_CxGetPincodeFromString (HUINT8 *szPincode);

STATIC HERROR	xmgr_cas_CxChangeSmartCardPin (HUINT8* szOldPincode, HUINT8* szNewPincode);
STATIC HERROR	xmgr_cas_CxPinCodeCheckAndAction (CxUiMsgType_t eUiMsgType, Handle_t hSvcHandle);

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

STATIC HUINT32	xmgr_cas_CxGetPincodeFromString(HUINT8 *szPincode)
{
	HUINT8		*p;
	HUINT32 	nPinCode;

	for (nPinCode = 0, p = szPincode; *p ; p++)
	{
		nPinCode *= 10;
		nPinCode += ((*p)-'0');
	}

	return nPinCode;
}


STATIC HERROR	xmgr_cas_CxCheckSmartCardPin(CxUiMsgType_t eUiMsgType, Handle_t hSvcHandle, HUINT8* szPincode)
{
	HERROR			hPinStatus = ERR_OK;

	if (szPincode == NULL)
	{
		return	ERR_FAIL;
	}

	HxLOG_Print("Pin-code check type[0x%x] \n", eUiMsgType);

	switch (eUiMsgType)
	{
		case	eCxUi_GroupMr_Ask:
			hPinStatus = CXAPI_UiAcceptMaturityRatingOverride(hSvcHandle, szPincode);
			break;
		case	eCxUi_GroupPpv_AcceptViewing:
			hPinStatus = CXAPI_UiStartViewingPPV(hSvcHandle, szPincode);
			break;
		case	eCxUi_GroupPpv_TokenPpv:
			hPinStatus = CXAPI_UiStartViewingTokenPPV(hSvcHandle, szPincode);
			break;
		default:
			hPinStatus = CXAPI_UiEnterCaPin(CX_SC_SLOT_0, szPincode);
			break;
	}

	HxLOG_Print("hPinStatus[0x%x] \n", hPinStatus);

	if (hPinStatus == ERR_OK)
	{
		return	ERR_OK;
	}

	return	ERR_FAIL;
}


STATIC HERROR	xmgr_cas_CxChangeSmartCardPin(HUINT8* szOldPincode, HUINT8* szNewPincode)
{
	HERROR		hPinStatus = ERR_OK;
	HUINT32 	ulNewPinCode, ulOldPincode;

	if ((szOldPincode == NULL) || (szNewPincode == NULL))
	{
		return	ERR_FAIL;
	}

	ulOldPincode = xmgr_cas_CxGetPincodeFromString(szOldPincode);
	ulNewPinCode = xmgr_cas_CxGetPincodeFromString(szNewPincode);

	hPinStatus = CXAPI_UiChangePin(CX_SC_SLOT_0, szOldPincode, szNewPincode, szNewPincode);
	HxLOG_Print("hPinStatus[%x] \n", hPinStatus);

	return	hPinStatus;
}


STATIC HERROR	xmgr_cas_CxPinCodeCheckAndAction(CxUiMsgType_t eUiMsgType, Handle_t hSvcHandle)
{
	HERROR			hPinStatus = ERR_OK;
	HUINT8			ucAcceptPin[5];

	HxLOG_Print("Pin-code check&Action type[0x%x] \n", eUiMsgType);

	xmgr_cas_CxGetAcceptCAPincode(ucAcceptPin);

	switch (eUiMsgType)
	{
		case	eCxUi_GroupMr_Ask:
			hPinStatus = CXAPI_UiAcceptMaturityRatingOverride(hSvcHandle, ucAcceptPin);
			break;
		case	eCxUi_GroupPpv_AcceptViewing:
			hPinStatus = CXAPI_UiStartViewingPPV(hSvcHandle, ucAcceptPin);
			break;
		case	eCxUi_GroupPpv_TokenPpv:
			hPinStatus = CXAPI_UiStartViewingTokenPPV(hSvcHandle, ucAcceptPin);
			break;
		default:
			HxLOG_Critical("\n\n");
			break;
	}

	HxLOG_Print("hPinStatus[0x%x] \n", hPinStatus);

	if (hPinStatus == ERR_OK)
	{
		return	ERR_OK;
	}

	return	ERR_FAIL;
}

#define	________________Global_Function_______________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

HERROR		xmgr_cas_CxCheckSmartCardPin(CxUiMsgType_t eUiMsgType, Handle_t hSvcHandle, HUINT8* szPincode)
{
	return	xmgr_cas_CxCheckSmartCardPin(eUiMsgType, hSvcHandle, szPincode);
}


HERROR		xmgr_cas_CxChangeSmartCardPin(HUINT8* szOldPincode, HUINT8* szNewPincode)
{
	return	xmgr_cas_CxChangeSmartCardPin(szOldPincode, szNewPincode);
}


HERROR		xmgr_cas_CxPinCodeCheckAndAction(CxUiMsgType_t eUiMsgType, Handle_t hSvcHandle)
{
	return	xmgr_cas_CxPinCodeCheckAndAction(eUiMsgType, hSvcHandle);
}


// End of file

