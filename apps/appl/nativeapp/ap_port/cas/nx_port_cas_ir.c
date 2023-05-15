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
 * @file	  		nx_port_cas_ir.c
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

void	NX_PORT_CAS_IRDETO_Init(void)
{
	APK_CAS_IRDETO_Init();
}

HERROR	NX_PORT_CAS_IRDETO_RegisterNotifier(APK_CAS_IRDETO_Notifier_t fnNotifier)
{
	return	APK_CAS_IRDETO_RegisterNotifier(fnNotifier);
}

HERROR	NX_PORT_CAS_IRDETO_UnregisterNotifier(void)
{
	APK_CAS_IRDETO_UnregisterNotifier();
	return	ERR_OK;
}

HERROR	NX_PORT_CAS_IRDETO_SetVirtualSecureCwMode(HBOOL bVirtualSecureCW)
{
	return	APK_CAS_IRDETO_SetVirtualSecureCwMode(bVirtualSecureCW);
}

HERROR	NX_PORT_CAS_IRDETO_GetVirturlSecureCwMode(HBOOL *pbVirtualSecureCwMode)
{
	return	APK_CAS_IRDETO_GetVirturlSecureCwMode(pbVirtualSecureCwMode);
}

HERROR	NX_PORT_CAS_IRDETO_GetIrdetoClientVersionName(HUINT8 *pszVesionName)
{
	return	APK_CAS_IRDETO_GetIrdetoClientVersionName((HCHAR*)pszVesionName);
}

HERROR	NX_PORT_CAS_IRDETO_IsCWE(HBOOL *pbCWE)
{
	return	APK_CAS_IRDETO_IsCWE(pbCWE);
}

HERROR	NX_PORT_CAS_IRDETO_IsJTAG(HBOOL *pbJTAG)
{
	return	APK_CAS_IRDETO_IsJTAG(pbJTAG);
}

HERROR	NX_PORT_CAS_IRDETO_IsSBOOT(HBOOL *pbSBOOT)
{
	return	APK_CAS_IRDETO_IsSBOOT(pbSBOOT);
}

HERROR	NX_PORT_CAS_IRDETO_IsOTP(HBOOL *pbOTP)
{
	return	APK_CAS_IRDETO_IsOTP(pbOTP);
}

HERROR	NX_PORT_CAS_IRDETO_GetIrdetoManCode(HUINT8 *pszMancode)
{
	return APK_CAS_IRDETO_GetIrdetoManCode((HCHAR*)pszMancode);
}

HERROR	NX_PORT_CAS_IRDETO_GetIrdetoHwCode(HUINT8 *pszHWCode)
{
	return	APK_CAS_IRDETO_GetIrdetoHwCode((HCHAR*)pszHWCode);
}

HERROR	NX_PORT_CAS_IRDETO_GetIrdetoVariant(HUINT8 *pszVariant)
{
	return	APK_CAS_IRDETO_GetIrdetoVariant((HCHAR*)pszVariant);
}

HERROR	NX_PORT_CAS_IRDETO_GetIrdetoSysId(HUINT8 *pszSysId)
{
	return	APK_CAS_IRDETO_GetIrdetoSysId((HCHAR*) pszSysId);
}

HERROR	NX_PORT_CAS_IRDETO_GetIrdetoKeyVer(HUINT8 *pszKeyVer)
{
	return	APK_CAS_IRDETO_GetIrdetoKeyVer((HCHAR*)pszKeyVer);
}

HERROR	NX_PORT_CAS_IRDETO_GetIrdetoSigVer(HUINT8 *pszSigVer)
{
	return	APK_CAS_IRDETO_GetIrdetoSigVer((HCHAR*)pszSigVer);
}

HERROR	NX_PORT_CAS_IRDETO_GetControlByte(HUINT8 *pControlByte)
{
	return	APK_CAS_IRDETO_GetControlByte(pControlByte);
}

HERROR	NX_PORT_CAS_IRDETO_GetPrivateData(HUINT8 *pPrivateData)
{
	return	APK_CAS_IRDETO_GetPrivateData(pPrivateData);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
