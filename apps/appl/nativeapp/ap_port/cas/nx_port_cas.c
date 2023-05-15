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
 * @file	  		nx_core_cas.c
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

void	NX_PORT_CAS_Init(void)
{
	APK_CAS_ACTION_Init();
	APK_CAS_MMI_Init();
}

void	NX_PORT_CAS_ACTION_RegisterNotifier(APK_CAS_ACTION_Notifier_t fnNotfiler)
{
	APK_CAS_ACTION_RegisterNotifier(fnNotfiler);
}

void	NX_PORT_CAS_ACTION_UnRegisterNotifier(APK_CAS_ACTION_Notifier_t fnNotfiler)
{
	APK_CAS_ACTION_UnRegisterNotifier(fnNotfiler);
}

HERROR	NX_PORT_CAS_ACTION_GetActionType(HUINT8 *pucActionType)
{
	return	APK_CAS_ACTION_GetActionType(pucActionType);
}

HERROR	NX_PORT_CAS_ACTION_GetSlotId(HUINT8 *pucSlotId)
{
	return	APK_CAS_ACTION_GetSlotId(pucSlotId);
}

HERROR	NX_PORT_CAS_ACTION_GetCasId(HUINT8 *pucCasId)
{
	return	APK_CAS_ACTION_GetSlotId(pucCasId);
}

HERROR	NX_PORT_CAS_ACTION_GetData(HUINT8 *pucData)
{
	return	APK_CAS_ACTION_GetData(pucData);
}

Handle_t	NX_PORT_CAS_MMI_OpenSession(DxCAS_GroupId_e eModule, HUINT32 ulSlot, DxCAS_MmiSessionType_e eSessionType, HUINT8 *pszParamData)
{
	Handle_t	hSession = HANDLE_NULL;

	hSession = APK_CAS_MMI_OpenSession(eModule, ulSlot, eSessionType, (const char*)pszParamData);

	return	hSession;
}

void	NX_PORT_CAS_MMI_CloseSession(DxCAS_GroupId_e eModule, HUINT32 ulSlot, DxCAS_MmiSessionType_e eSessionType, Handle_t hSession)
{
	APK_CAS_MMI_CloseSession(eModule, ulSlot, eSessionType, hSession);
}

void	NX_PORT_CAS_MMI_RemoveEvtInfo(DxCAS_MMI_Event_t *pstEvent)
{
	APK_CAS_MMI_RemoveEvtInfo(pstEvent);
}

void	NX_PORT_CAS_MMI_GetEvtInfo(DxCAS_MMI_Event_t **ppstEvent)
{
	APK_CAS_MMI_GetEvtInfo(ppstEvent);
}

void	NX_PORT_CAS_MMI_RemoveUiEvent(DxCAS_MmiSessionType_e eSessionType, DxCAS_MMI_Event_t *pstEvent)
{
	APK_CAS_MMI_RemoveUiEvent(eSessionType, pstEvent);
}

void	NX_PORT_CAS_MMI_GetUiEvent(DxCAS_MmiSessionType_e eSession, Handle_t hSession, DxCAS_MMI_Event_t **ppstEvent)
{
	APK_CAS_MMI_GetUiEvent(eSession, hSession, ppstEvent);
}

void	NX_PORT_CAS_MMI_SendUiResponse(DxCAS_GroupId_e eModule, HUINT32 ulSlot, DxCAS_MmiSessionType_e eSession, Handle_t hSession, HUINT8 *pzInputData)
{
	APK_CAS_MMI_SendUiResponse(eModule, ulSlot, eSession, hSession, (HCHAR*)pzInputData);
}

void	NX_PORT_CAS_MMI_UnRegisterSessionNotifier(DxCAS_MmiSessionType_e eSession)
{
	APK_CAS_MMI_UnRegisterSessionNotifier(eSession);
}

void	NX_PORT_CAS_MMI_RegisterSessionNotifier(DxCAS_MmiSessionType_e eSession, APK_CAS_MMI_Notifier_t fnNotifier)
{
	APK_CAS_MMI_RegisterSessionNotifier(eSession, (APK_CAS_MMI_Notifier_t)fnNotifier);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
