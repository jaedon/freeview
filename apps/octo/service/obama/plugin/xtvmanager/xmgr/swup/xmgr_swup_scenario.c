/*****************************************************************************************
*	@file		xmgr_swup_scenario.c
*
*	http://www.humaxdigital.com
*
*	@date			2015/10/03
*	@author
*	@brief
******************************************************************************************/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <octo_common.h>
#include <papi.h>
#include <mgr_swup.h>

#include "local_include/_xmgr_swup.h"



/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___FUNCTION______________________________________________________________________________________
/* *************************************************************************************************** */

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

STATIC DxSwUpdate_Source_e xmgr_swup_GetNextSource_Tdc(DxSwUpdate_Source_e eCurSource)
{
	switch(eCurSource)
	{
	case eDxSWUPDATE_SOURCE_None:	return eDxSWUPDATE_SOURCE_Usb;
	case eDxSWUPDATE_SOURCE_Usb:	return eDxSWUPDATE_SOURCE_Rf;
	case eDxSWUPDATE_SOURCE_Rf:		return eDxSWUPDATE_SOURCE_None;	/* ORG: eDxSWUPDATE_SOURCE_Ip, 20151014 */
	case eDxSWUPDATE_SOURCE_Ip:		return eDxSWUPDATE_SOURCE_None;
	default:
		break;
	}

	return eDxSWUPDATE_SOURCE_None;
}

STATIC DxSwUpdate_Source_e xmgr_swup_GetNextSource_Base(DxSwUpdate_Source_e eCurSource)
{
	switch(eCurSource)
	{
	case eDxSWUPDATE_SOURCE_None:	return eDxSWUPDATE_SOURCE_Usb;
	case eDxSWUPDATE_SOURCE_Usb:	return eDxSWUPDATE_SOURCE_Ip;
	case eDxSWUPDATE_SOURCE_Ip:		return eDxSWUPDATE_SOURCE_Rf;
	case eDxSWUPDATE_SOURCE_Rf:		return eDxSWUPDATE_SOURCE_None;
	default:
		break;
	}

	return eDxSWUPDATE_SOURCE_None;
}

#if defined(CONFIG_OP_UK_DTT)
STATIC DxSwUpdate_Source_e xmgr_swup_GetNextSource_Ukdtt (DxSwUpdate_Source_e eCurSource)
{
	DxBatchType_e			 eBatType = MGR_ACTION_GetBatchType();

	switch(eBatType)
	{
	case eDxBAT_TYPE_COLDBOOT:
	case eDxBAT_TYPE_STANDBY:
		switch(eCurSource)
		{
		case eDxSWUPDATE_SOURCE_None:	return eDxSWUPDATE_SOURCE_Rf;
		case eDxSWUPDATE_SOURCE_Rf: 	return eDxSWUPDATE_SOURCE_Ip;
		case eDxSWUPDATE_SOURCE_Ip: 	return eDxSWUPDATE_SOURCE_None;
		default:
			break;
		}

	default:
		return xmgr_swup_GetNextSource_Base(eCurSource);
	}

	return eDxSWUPDATE_SOURCE_None;
}
#endif

DxSwUpdate_Source_e XMGR_SWUP_GetNextSource(DxSwUpdate_Source_e eCurSource)
{
#if defined(CONFIG_OP_TDC)
	return xmgr_swup_GetNextSource_Tdc(eCurSource);
#elif defined(CONFIG_OP_UK_DTT)
	return xmgr_swup_GetNextSource_Ukdtt(eCurSource);
#else
	return xmgr_swup_GetNextSource_Base(eCurSource);
#endif
}

STATIC MgrSwup_State_e xmgr_swup_GetNextState_AfterDownload_Ses(DxSwUpdate_Source_e eSource)
{
	MgrSwup_State_e		eNextState = eSWUP_STATE_Unknown;
	MgrAction_State_e	eActionState = MGR_ACTION_GetState();

	if ((eActionState == eAmState_ACTIVE_STANDBY) && (eSource == eDxSWUPDATE_SOURCE_Ip))
	{
		eNextState = eSWUP_STATE_Finish;
	}
	else
	{
		eNextState = eSWUP_STATE_Install;
	}

	return eNextState;
}

STATIC MgrSwup_State_e xmgr_swup_GetNextState_AfterDownload_Humax(void)
{
	MgrSwup_State_e		eNextState = eSWUP_STATE_Unknown;

#if defined(CONFIG_OP_MIDDLE_EAST)
	if (MGR_ACTION_GetState() != eAmState_ACTIVE_STANDBY)
	{
		eNextState = eSWUP_STATE_Install;
	}
	else
	{
		eNextState = eSWUP_STATE_Finish;
	}
#else
	eNextState = eSWUP_STATE_Install;
#endif

	return eNextState;
}

STATIC MgrSwup_State_e xmgr_swup_GetNextState_AfterDownload_Base(void)
{
	return eSWUP_STATE_Install;
}

MgrSwup_State_e XMGR_SWUP_GetNextState_AfterDownload(DxSwUpdate_Source_e eSource)
{
	MgrSwup_State_e		eNextState = eSWUP_STATE_Unknown;

#if defined(CONFIG_OTA_SES)
	eNextState = xmgr_swup_GetNextState_AfterDownload_Ses(eSource);
#elif defined(CONFIG_OTA_HUMAX)
	eNextState = xmgr_swup_GetNextState_AfterDownload_Humax();
#else
	eNextState = xmgr_swup_GetNextState_AfterDownload_Base();
#endif

	return eNextState;
}

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

STATIC DxSwUpdate_Source_e xmgr_swup_GetStartSource_Base(void)
{
	return eDxSWUPDATE_SOURCE_Usb;
}

DxSwUpdate_Source_e XMGR_SWUP_GetStartSource(void)
{
#if defined(CONFIG_OTA_SES)
	return (DxSwUpdate_Source_e)xmgr_swup_GetStartUpdateSource_Ses();
#else
	return xmgr_swup_GetStartSource_Base();
#endif
}

DxSwUpdate_Source_e XMGR_SWUP_GetStartSource_DvbSsu (DxBatchType_e eBatchType)
{
	switch(eBatchType)
	{
	// OTA (normal): USB -> IP -> RF
	case eDxBAT_TYPE_NONE:
		return (HUINT32)eDxSWUPDATE_SOURCE_Usb;

	// Standby: RF -> IP
	case eDxBAT_TYPE_STANDBY:
	case eDxBAT_TYPE_COLDBOOT:
		return (HUINT32)eDxSWUPDATE_SOURCE_Rf;

	default:
		return (HUINT32)eDxSWUPDATE_SOURCE_Ip;
	}

	return eDxSWUPDATE_SOURCE_Usb;
}


/* end of file */

