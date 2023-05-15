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

/******************************************************************************/
/**
 * @file	  		nx_port_cas_na.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_port.h>
#include <nx_port_cas.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

void	NX_PORT_CAS_NAGRA_Init(void)
{
	APK_CAS_NAGRA_Init();
}

HERROR	NX_PORT_CAS_NAGRA_RegisterNotifier(APK_CAS_NAGRA_Notifier_t fnNotifier)
{
	return	APK_CAS_NAGRA_RegisterNotifier(fnNotifier);
}

HERROR	NX_PORT_CAS_NAGRA_UnregisterNotifier(void)
{
	APK_CAS_NAGRA_UnregisterNotifier();
	return	ERR_OK;
}

HERROR	NX_PORT_CAS_NAGRA_IsCWE(HBOOL *pbCWE)
{
	return	APK_CAS_NAGRA_IsCWE(pbCWE);
}

HERROR	NX_PORT_CAS_NAGRA_IsJTAG(HBOOL *pbJTAG)
{
	return	APK_CAS_NAGRA_IsJTAG(pbJTAG);
}

HERROR	NX_PORT_CAS_NAGRA_IsSBOOT(HBOOL *pbSBOOT)
{
	return	APK_CAS_NAGRA_IsSBOOT(pbSBOOT);
}

HERROR	NX_PORT_CAS_NAGRA_IsOTP(HBOOL *pbOTP)
{
	return	APK_CAS_NAGRA_IsOTP(pbOTP);
}

HERROR	NX_PORT_CAS_NAGRA_GetNUID(HUINT8 *pszNUID)
{
	return	APK_CAS_NAGRA_GetNUID((HCHAR*)pszNUID);
}

HERROR	NX_PORT_CAS_NAGRA_GetCASN(HUINT8 *pszCASN)
{
	return APK_CAS_NAGRA_GetCASN((HCHAR*)pszCASN);
}

HERROR	NX_PORT_CAS_NAGRA_GetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE)
{
	return	APK_CAS_NAGRA_GetFusingInfo(pucSCS, pucJTAG, pucOTP, pucCWE);
}

HBOOL	NX_PORT_CAS_NAGRA_IsSCInserted(void)
{
	return	APK_CAS_NAGRA_IsSCInserted();
}

HBOOL	NX_PORT_CAS_NAGRA_PINCODE_Verify(HUINT8 *pszPinStr)
{
	return	APK_CAS_NAGRA_PINCODE_Verify((HCHAR*)pszPinStr);
}

HBOOL	NX_PORT_CAS_NAGRA_GetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount)
{
	return	APK_CAS_NAGRA_GetAccessibleState(pnAccessState, pulRemainTime, pnRetryCount);
}

HBOOL	NX_PORT_CAS_NAGRA_VerifyPincodeGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount, HUINT8 *pszPinStr)
{
	return	APK_CAS_NAGRA_VerifyPincodeGetAccessibleState(pnAccessState, pulRemainTime, pnRetryCount, (HCHAR*)pszPinStr);
}

//APK_CAS_NAGRA_GetAccessibleState(), APK_CAS_NAGRA_VerifyPincodeGetAccessibleState() 함수가 먼저 불린 후에, 이 함수 콜해야 한다.
HBOOL	NX_PORT_CAS_NAGRA_IsAccessibleState(HUINT8 *pnAccessState)
{
	return	APK_CAS_NAGRA_IsAccessibleState(pnAccessState);
}

HBOOL	NX_PORT_CAS_NAGRA_PINCODE_Set(HUINT8 *pszOldPinStr, HUINT8 *pszNewPinStr)
{
	return	APK_CAS_NAGRA_PINCODE_Set((HCHAR*)pszOldPinStr, (HCHAR*)pszNewPinStr);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
