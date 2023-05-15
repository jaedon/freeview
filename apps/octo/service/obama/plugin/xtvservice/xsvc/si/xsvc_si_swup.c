/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_swup.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIVE			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

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
 * (c) 2011-2013 Humax Co., Ltd.
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <pal_param.h>

#include <isosvc.h>

#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_swup.h>
#include <_svc_si_lib.h>
#include <psvc_si.h>


#include "./local_include/_xsvc_si.h"
#include "./local_include/_xsvc_si_swup_base.h"
#include "./local_include/_xsvc_si_swup.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

HUINT32 xsvc_si_SwupCheckCvtVersionTypeToPalParam(VersionType_t eVersionType)
{
	switch (eVersionType)
	{
	case eVERSION_TYPE_NEXTOTA:			return (HUINT32)ePAL_PARAM_APP_VERSION_NEXT;
	case eVERSION_TYPE_LOADER:			return (HUINT32)ePAL_PARAM_LOADER_VERSION;
	case eVERSION_TYPE_APPL:			return (HUINT32)ePAL_PARAM_APP_VERSION;
	case eVERSION_TYPE_EEPROM:			return (HUINT32)ePAL_PARAM_MAX;
	case eVERSION_TYPE_KERNEL:			return (HUINT32)ePAL_PARAM_KERNEL_VERSION;
	case eVERSION_TYPE_ROOTFS:			return (HUINT32)ePAL_PARAM_ROOTFS_VERSION;
	case eVERSION_TYPE_RESOURCE:		return (HUINT32)ePAL_PARAM_RESOURCE_VERSION;
	case eVERSION_TYPE_DB:				return (HUINT32)ePAL_PARAM_DB_VERSION;
	case eVERSION_TYPE_BOOTLOADER:		return (HUINT32)ePAL_PARAM_BOOTLOADER_VERSION;

	/* unknown */
	case eVERSION_TYPE_UPGRADER:
	default:
		break;
	}

	return (HUINT32)ePAL_PARAM_MAX;
}

HERROR xsvc_si_SwupCheckGetSwupDownType(HUINT8 *pucDownType)
{
	if (NULL == pucDownType)
		return ERR_FAIL;

#if defined(CONFIG_OP_ERTELECOM)
	return xsvc_si_SwupCheckGetSwupDownType_Ertelecom(pucDownType);
#else
	*pucDownType = SvcSi_SWUP_TYPE_USER_SELECT;
	return ERR_OK;
#endif
}

HERROR xsvc_si_SwupCheckSetSwupDownType(HUINT8 ucDownType)
{
#if defined(CONFIG_OP_ERTELECOM)
	return xsvc_si_SwupCheckSetSwupDownType_Ertelecom(ucDownType);
#else
	return ERR_FAIL;
#endif
}

void xsvc_si_SwupCheckMsgEvtNitTable(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg)
{
	if (NULL == pstSwupCheck)
	{
		HxLOG_Error(" pstSwupCheck is NULL \n");
		return ;
	}

	if (NULL == pstMsg)
	{
		HxLOG_Error(" pstMsg is NULL \n");
		return ;
	}

#if defined(CONFIG_OTA_DVBSSU)
#if defined(CONFIG_OP_ERTELECOM)
	if(pstSwupCheck->stBase.bLiveSwupCheck== TRUE)
		xsvc_si_SwupCheckMsgEvtNitTable_Ertelecom(pstSwupCheck, pstMsg);
	else
		xsvc_si_SwupCheckMsgEvtNitTable_Dvbssu(pstSwupCheck, pstMsg);
#elif defined(CONFIG_OP_UK_DTT)
	if(pstSwupCheck->stBase.bLiveSwupCheck == TRUE)
		;
	else
		xsvc_si_SwupCheckMsgEvtNitTable_UkDtt(pstSwupCheck, pstMsg);
#else
	if(pstSwupCheck->stBase.bLiveSwupCheck == TRUE)
		;
	else
		xsvc_si_SwupCheckMsgEvtNitTable_Dvbssu(pstSwupCheck, pstMsg);
#endif
#endif

	return;
}

HERROR	xsvc_si_SwupCheckGetFilteringTypes(xsvcSi_SwupData_t *pstSwupCheck)
{
	HERROR hErr = ERR_FAIL;

	if (NULL == pstSwupCheck)
	{
		HxLOG_Error(" pstSwupCheck is NULL \n");
		return ERR_FAIL;
	}

#if defined(CONFIG_OTA_DVBSSU)
#if defined(CONFIG_OP_ERTELECOM)
	if(pstSwupCheck->stBase.bLiveSwupCheck== TRUE)
		hErr =  xsvc_si_SwupCheckGetFilteringType_Ertelecom(pstSwupCheck);
	else
		hErr =  xsvc_si_SwupCheckGetFilteringTypes_Dvbssu(pstSwupCheck);
#elif defined(CONFIG_OP_UK_DTT)
	if(pstSwupCheck->stBase.bLiveSwupCheck == TRUE)
		hErr =  xsvc_si_SwupCheckGetFilteringTypes_Dvbssu(pstSwupCheck);
	else
		hErr =  xsvc_si_SwupCheckGetFilteringTypes_UkDtt(pstSwupCheck);
#else
	hErr =  xsvc_si_SwupCheckGetFilteringTypes_Dvbssu(pstSwupCheck);
#endif
#endif

	return hErr;
}

HERROR	xsvc_si_SwupCheckMakeUntExtIdByOui(HUINT16 *pusUntExtId)
{
	HERROR hErr = ERR_FAIL;

#if defined(CONFIG_OTA_DVBSSU)
#if defined(CONFIG_OP_UK_DTT)
	hErr = xsvc_si_SwupCheckMakeUntExtIdByOui_UkDtt(pusUntExtId);
#else
	hErr = xsvc_si_SwupCheckMakeUntExtIdByOui_Dvbssu(pusUntExtId);
#endif
#elif defined(CONFIG_OTA_IRDETO)
#if defined(CONFIG_OTA_IRDETO_LOADER_V5)
	hErr = xsvc_si_SwupCheckMakeUntExtIdByOui_Irdeto(pusUntExtId);
#endif
#endif
	return hErr;
}

/* end of file */

