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
 * @file	  		nx_core_cas_ir.c
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
#include <nx_common.h>
#include <nx_port_cas.h>
#include <nx_core_cas.h>
#include <nx_core_cas_ir.h>


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

HERROR	NX_CAS_IRDETO_Init(void)
{
	//NX_CAS_ACTION_RegisterNotifier(NX_CAS_IrdetoMessageNotifier);

	return	ERR_OK;
}

HERROR	NX_CAS_IRDETO_RegisterNotifier(APK_CAS_IRDETO_Notifier_t fnNotifier)
{
	return	NX_PORT_CAS_IRDETO_RegisterNotifier(fnNotifier);
}

HERROR	NX_CAS_IRDETO_UnregisterNotifier(void)
{
	NX_PORT_CAS_IRDETO_UnregisterNotifier();
	return	ERR_OK;
}

HERROR	NX_CAS_IRDETO_SetVirtualSecureCwMode(HBOOL bVirtualSecureCW)
{
	return	NX_PORT_CAS_IRDETO_SetVirtualSecureCwMode(bVirtualSecureCW);
}

HERROR	NX_CAS_IRDETO_GetVirturlSecureCwMode(HBOOL *pbVirtualSecureCwMode)
{
	return	NX_PORT_CAS_IRDETO_GetVirturlSecureCwMode(pbVirtualSecureCwMode);
}

HERROR	NX_CAS_IRDETO_GetIrdetoClientVersionName(HUINT8 *pszVesionName)
{
	return	NX_PORT_CAS_IRDETO_GetIrdetoClientVersionName(pszVesionName);
}

HERROR	NX_CAS_IRDETO_IsCWE(HBOOL *pbCWE)
{
	return	NX_PORT_CAS_IRDETO_IsCWE(pbCWE);
}

HERROR	NX_CAS_IRDETO_IsJTAG(HBOOL *pbJTAG)
{
	return	NX_PORT_CAS_IRDETO_IsJTAG(pbJTAG);
}

HERROR	NX_CAS_IRDETO_IsSBOOT(HBOOL *pbSBOOT)
{
	return	NX_PORT_CAS_IRDETO_IsSBOOT(pbSBOOT);
}

HERROR	NX_CAS_IRDETO_IsOTP(HBOOL *pbOTP)
{
	return	NX_PORT_CAS_IRDETO_IsOTP(pbOTP);
}

HERROR	NX_CAS_IRDETO_GetIrdetoManCode(HUINT8 *pszMancode)
{
	return NX_PORT_CAS_IRDETO_GetIrdetoManCode(pszMancode);
}

HERROR	NX_CAS_IRDETO_GetIrdetoHwCode(HUINT8 *pszHWCode)
{
	return	NX_PORT_CAS_IRDETO_GetIrdetoHwCode(pszHWCode);
}

HERROR	NX_CAS_IRDETO_GetIrdetoVariant(HUINT8 *pszVariant)
{
	return	NX_PORT_CAS_IRDETO_GetIrdetoVariant(pszVariant);
}

HERROR	NX_CAS_IRDETO_GetIrdetoSysId(HUINT8 *pszSysId)
{
	return	NX_PORT_CAS_IRDETO_GetIrdetoSysId(pszSysId);
}

HERROR	NX_CAS_IRDETO_GetIrdetoKeyVer(HUINT8 *pszKeyVer)
{
	return	NX_PORT_CAS_IRDETO_GetIrdetoKeyVer(pszKeyVer);
}

HERROR	NX_CAS_IRDETO_GetIrdetoSigVer(HUINT8 *pszSigVer)
{
	return	NX_PORT_CAS_IRDETO_GetIrdetoSigVer(pszSigVer);
}

HERROR	NX_CAS_IRDETO_GetControlByte(HUINT8 *pControlByte)
{
	return	NX_PORT_CAS_IRDETO_GetControlByte(pControlByte);
}

HERROR	NX_CAS_IRDETO_GetPrivateData(HUINT8 *pPrivateData)
{
	return	NX_PORT_CAS_IRDETO_GetPrivateData(pPrivateData);
}

HERROR	NX_CAS_IRDETO_GetFusingInfo(HBOOL *bIsSCS, HBOOL *bIsJTAG, HBOOL *bIsOTP, HBOOL *bIsCWE)
{
	HERROR	hError = ERR_FAIL;

	hError = NX_CAS_IRDETO_IsCWE(bIsCWE);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CAS_IRDETO_IsCWE() Fail!!\n");
	}

	hError = NX_CAS_IRDETO_IsJTAG(bIsJTAG);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CAS_IRDETO_IsJTAG() Fail!!\n");
	}

	hError = NX_CAS_IRDETO_IsSBOOT(bIsSCS);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CAS_IRDETO_IsSBOOT() Fail!!\n");
	}

	hError = NX_CAS_IRDETO_IsOTP(bIsOTP);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CAS_IRDETO_IsOTP() Fail!!\n");
	}

	return	ERR_OK;
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
