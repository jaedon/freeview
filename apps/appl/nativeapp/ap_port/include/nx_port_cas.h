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
 * @file	  		nx_port_cas.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PORT_CAS_H__
#define __NX_PORT_CAS_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/

#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <hlib.h>
#include <dlib.h>
#include <apk.h>

#include <nx_port_type.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/
void		NX_PORT_CAS_Init(void);

void		NX_PORT_CAS_ACTION_RegisterNotifier(APK_CAS_ACTION_Notifier_t fnNotfiler);
void		NX_PORT_CAS_ACTION_UnRegisterNotifier(APK_CAS_ACTION_Notifier_t fnNotfiler);
HERROR		NX_PORT_CAS_ACTION_GetActionType(HUINT8 *pucActionType);
HERROR		NX_PORT_CAS_ACTION_GetSlotId(HUINT8 *pucSlotId);
HERROR		NX_PORT_CAS_ACTION_GetCasId(HUINT8 *pucCasId);
HERROR		NX_PORT_CAS_ACTION_GetData(HUINT8 *pucData);

Handle_t	NX_PORT_CAS_MMI_OpenSession(DxCAS_GroupId_e eModule, HUINT32 ulSlot, DxCAS_MmiSessionType_e eSessionType, HUINT8 *pszParamData);
void		NX_PORT_CAS_MMI_CloseSession(DxCAS_GroupId_e eModule, HUINT32 ulSlot, DxCAS_MmiSessionType_e eSessionType, Handle_t hSession);

void		NX_PORT_CAS_MMI_RemoveEvtInfo(DxCAS_MMI_Event_t *pstEvent);
void		NX_PORT_CAS_MMI_GetEvtInfo(DxCAS_MMI_Event_t **ppstEvent);
void		NX_PORT_CAS_MMI_RemoveUiEvent(DxCAS_MmiSessionType_e eSessionType, DxCAS_MMI_Event_t *pstEvent);
void		NX_PORT_CAS_MMI_GetUiEvent(DxCAS_MmiSessionType_e eSession, Handle_t hSession, DxCAS_MMI_Event_t **ppstEvent);
void		NX_PORT_CAS_MMI_SendUiResponse(DxCAS_GroupId_e eModule, HUINT32 ulSlot, DxCAS_MmiSessionType_e eSession, Handle_t hSession, HUINT8 *pzInputData);
void		NX_PORT_CAS_MMI_UnRegisterSessionNotifier(DxCAS_MmiSessionType_e eSession);
void		NX_PORT_CAS_MMI_RegisterSessionNotifier(DxCAS_MmiSessionType_e eSession, APK_CAS_MMI_Notifier_t fnNotifier);

#if defined(CONFIG_MW_CAS_NAGRA)
void		NX_PORT_CAS_NAGRA_Init(void);
HERROR		NX_PORT_CAS_NAGRA_RegisterNotifier(APK_CAS_NAGRA_Notifier_t fnNotifier);
HERROR		NX_PORT_CAS_NAGRA_UnregisterNotifier(void);
HERROR		NX_PORT_CAS_NAGRA_IsCWE(HBOOL *pbCWE);
HERROR		NX_PORT_CAS_NAGRA_IsJTAG(HBOOL *pbJTAG);
HERROR		NX_PORT_CAS_NAGRA_IsSBOOT(HBOOL *pbSBOOT);
HERROR		NX_PORT_CAS_NAGRA_IsOTP(HBOOL *pbOTP);
HERROR		NX_PORT_CAS_NAGRA_GetNUID(HUINT8 *pszNUID);
HERROR		NX_PORT_CAS_NAGRA_GetCASN(HUINT8 *pszCASN);
HERROR		NX_PORT_CAS_NAGRA_GetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE);
HBOOL		NX_PORT_CAS_NAGRA_IsSCInserted(void);
HBOOL		NX_PORT_CAS_NAGRA_PINCODE_Verify(HUINT8 *pszPinStr);
HBOOL		NX_PORT_CAS_NAGRA_GetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount);
HBOOL		NX_PORT_CAS_NAGRA_VerifyPincodeGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount, HUINT8 *pszPinStr);
HBOOL		NX_PORT_CAS_NAGRA_IsAccessibleState(HUINT8 *pnAccessState);
HBOOL		NX_PORT_CAS_NAGRA_PINCODE_Set(HUINT8 *pszOldPinStr, HUINT8 *pszNewPinStr);
#endif	// End of defined(CONFIG_MW_CAS_NAGRA)

#if defined(CONFIG_MW_CAS_IRDETO)
void		NX_PORT_CAS_IRDETO_Init(void);
HERROR		NX_PORT_CAS_IRDETO_RegisterNotifier(APK_CAS_IRDETO_Notifier_t fnNotifier);
HERROR		NX_PORT_CAS_IRDETO_UnregisterNotifier(void);
HERROR		NX_PORT_CAS_IRDETO_SetVirtualSecureCwMode(HBOOL bVirtualSecureCW);
HERROR		NX_PORT_CAS_IRDETO_GetVirturlSecureCwMode(HBOOL *pbVirtualSecureCwMode);
HERROR		NX_PORT_CAS_IRDETO_GetIrdetoClientVersionName(HUINT8 *pszVesionName);
HERROR		NX_PORT_CAS_IRDETO_IsCWE(HBOOL *pbCWE);
HERROR		NX_PORT_CAS_IRDETO_IsJTAG(HBOOL *pbJTAG);
HERROR		NX_PORT_CAS_IRDETO_IsSBOOT(HBOOL *pbSBOOT);
HERROR		NX_PORT_CAS_IRDETO_IsOTP(HBOOL *pbOTP);
HERROR		NX_PORT_CAS_IRDETO_GetIrdetoManCode(HUINT8 *pszMancode);
HERROR		NX_PORT_CAS_IRDETO_GetIrdetoHwCode(HUINT8 *pszHWCode);
HERROR		NX_PORT_CAS_IRDETO_GetIrdetoVariant(HUINT8 *pszVariant);
HERROR		NX_PORT_CAS_IRDETO_GetIrdetoSysId(HUINT8 *pszSysId);
HERROR		NX_PORT_CAS_IRDETO_GetIrdetoKeyVer(HUINT8 *pszKeyVer);
HERROR		NX_PORT_CAS_IRDETO_GetIrdetoSigVer(HUINT8 *pszSigVer);
HERROR		NX_PORT_CAS_IRDETO_GetControlByte(HUINT8 *pControlByte);
HERROR		NX_PORT_CAS_IRDETO_GetPrivateData(HUINT8 *pPrivateData);
#endif	// End of defined(CONFIG_MW_CAS_IRDETO)

#endif // __NX_PORT_CAS_H__

