/*
 * (c) 2011-2012 Humax Co., Ltd.
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
	@file     xmgr_cas_va_pincode.c
	@brief    file_name & simple comment.

	Description: CONAX Pin code API  \n
	Module: Application(Layer), AP_CORE(module), CAS(module), VIACCESS(module) \n
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
#include <app.h>

#include "./local_include/_xmgr_cas_va_util.h"

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

STATIC HUINT32	xmgr_cas_VaGetPincodeFromString(HUINT8 *szPincode);
STATIC HERROR	xmgr_cas_VaChangeSmartCardPin(HUINT8* szOldPincode, HUINT8* szNewPincode);

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

STATIC HUINT32	xmgr_cas_VaGetPincodeFromString(HUINT8 *szPincode)
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

STATIC HERROR	xmgr_cas_VaChangeSmartCardPin(HUINT8* szOldPincode, HUINT8* szNewPincode)
{
	HERROR		hPinStatus = ERR_OK;
	HUINT32 	ulNewPinCode, ulOldPincode;

	if ((szOldPincode == NULL) || (szNewPincode == NULL))
	{
		return	ERR_FAIL;
	}

	ulOldPincode = xmgr_cas_VaGetPincodeFromString(szOldPincode);
	ulNewPinCode = xmgr_cas_VaGetPincodeFromString(szNewPincode);

	//hPinStatus = VaAPI_UiChangePin(VA_SC_SLOT_0, szOldPincode, szNewPincode, szNewPincode);
	//hPinStatus = VaAPI_UiChangePin(1, szOldPincode, szNewPincode, szNewPincode);
	HxLOG_Print("hPinStatus[%x] \n", hPinStatus);

	return	hPinStatus;
}


#define	________________Global_Function_______________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

#if 0 // 사용하는 함수 없음..ysmoon.2013.08.12
HERROR		AP_CAS_VA_ChangeSmartCardPin(HUINT8* szOldPincode, HUINT8* szNewPincode)
{
	return	xmgr_cas_VaChangeSmartCardPin(szOldPincode, szNewPincode);
}
#endif
// End of file
