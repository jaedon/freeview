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
 * @file	  		nx_core_cas.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_CAS_H__
#define __NX_CORE_CAS_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_port.h>
#if defined(CONFIG_MW_CAS_NAGRA)
#include <nx_core_cas_na.h>
#endif
#if defined(CONFIG_MW_CAS_IRDETO)
#include <nx_core_cas_ir.h>
#endif


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/

/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define	NX_CAS_TYPE_NAGRA_STRING_ID							"nagra"
#define	NX_CAS_TYPE_IRDETO_STRING_ID						"ir"
#define	NX_CAS_TYPE_CONAX_STRING_ID							"cx"
#define	NX_CAS_TYPE_CAM_STRING_ID							"ci"
#define	NX_CAS_TYPE_VERIMATRIX_STRING_ID					"vmx"

#define	NX_CAS_PARSE_UITYPE_ID								"uitype"
#define	NX_CAS_PARSE_UITYPE_GRID_ID							"grid"
#define	NX_CAS_PARSE_UITYPE_BANNER_ID						"banner"

#define	NX_CAS_PARSE_UIDATA_ID								"uidata"
#define	NX_CAS_PARSE_UIDATA_GRIDITME_ID						"griditem"

#define	NX_CAS_PARSE_CASTYPE_ID								"castype"
#define	NX_CAS_PARSE_SLOTNUMBER_ID							"slotnumber"
#define	NX_CAS_PARSE_INDEX_ID								"index"
#define	NX_CAS_PARSE_STRINGLENGTH_ID						"stringlength"
#define	NX_CAS_PARSE_STRING_ID								"string"
#define	NX_CAS_PARSE_ID_ID									"id"
#define	NX_CAS_PARSE_IN_ID									"in"

#define NX_CAS_PARSE_LINE_SPACE_STR							"%0A"
#define NX_CAS_PARSE_LINE_SPACE_STR_LF						"LF"

#define	NX_CAS_PARSE_GRIDITEM_MENUINFO_ITEMID_INDEX			0
#define	NX_CAS_PARSE_GRIDITEM_MENUINFO_VALUE_INDEX			1


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

typedef enum
{
	eNxCasSlot_0 = 0,
	eNxCasSlot_1,
	eNxCasSlot_Max
} NX_CasSlot_t;


typedef enum
{
	eNxCasStatus_None	= 0x00000000,
	eNxCasStatus_Clear	= 0x00000001,
	eNxCasStatus_Error	= 0x00000002,
	eNxCasStatus_Lock	= 0x00000004,
} NX_CasStatus_t;


/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/

typedef struct
{
	HUINT32					ulSessionHandleKey;
	DxCAS_GroupId_e			eCasGroupId;
	HUINT32					ulSlot;
	DxCAS_MmiSessionType_e	eSessionType;
	HUINT8					*pszParamData;
} Nx_CasSessionInfo_t;

typedef struct
{
	HUINT32						ulSessionKey;

	HUINT32						ulSlot;
	DxCAS_GroupId_e				eCasGroupId;
	DxCAS_MmiSessionType_e		eCasSessionType;
	HUINT32						ulSessionHandle;
} NX_CasSessionItemContext;


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/
HERROR	NX_CAS_Init(void);

HERROR	NX_CAS_OpenSession(HUINT32 ulSessionHandleKey, DxCAS_GroupId_e eCasGroupId, HUINT32 ulSlot, DxCAS_MmiSessionType_e eSessionType, HUINT8 *pszParamData);
HERROR	NX_CAS_CloseSession(HUINT32 ulSessionHandleKey, DxCAS_GroupId_e eCasGroupId);
HERROR	NX_CAS_CloseSessionWithHandleAndHandleKey(HUINT32 ulSessionHandleKey, HUINT32 ulSessionHandle, DxCAS_GroupId_e eCasGroupId);
NX_CasSessionItemContext	*NX_CAS_FindSessionItem(HUINT32 ulSessionHandleKey, DxCAS_GroupId_e eCasGroupId);
NX_CasSessionItemContext	*NX_CAS_FindSessionItemWithHandlekeyAndHandleInGroup(HUINT32 ulSessionHandleKey, HUINT32 ulSessionHandle, DxCAS_GroupId_e eCasGroupId);

HBOOL	NX_CAS_FindSessionWithHandleAndType(HUINT32 ulSessionHandle, DxCAS_MmiSessionType_e eSessionType);
HBOOL	NX_CAS_FindSessionWithHandleAndTypeInGroup(HUINT32 ulSessionHandle, DxCAS_MmiSessionType_e eSessionType, DxCAS_GroupId_e eCasGroupId);
HBOOL	NX_CAS_IsValidSession(HUINT32 ulSessionHandleKey, HUINT32 ulSessionHandle, DxCAS_GroupId_e eCasGroupId);
NX_CasSessionItemContext	*NX_CAS_GetSessionItem(HINT32 lIndex);
HINT32	NX_CAS_GetSessionCount(void);

HERROR	NX_CAS_GetCasMessage(HUINT32 ulSession, DxCAS_GroupId_e *peCasGroupId, HUINT32 *pulSlot, HUINT8 **ppszMessage);
HERROR	NX_CAS_GetCasUiEvtInfo(HUINT32 *pulzEvtType, DxCAS_GroupId_e *peCasGroupId, HUINT32 *pulSlot, HUINT32 *pulSession);
HERROR	NX_CAS_ReleaseCasMessage(HUINT8 *pszMessage);

HERROR	NX_CAS_RequestCasData(Nx_CasSessionInfo_t *pstCasSessionInfo);


/******************************************************************************/
#define __CAS_MMI_API__________________________________________________________
/******************************************************************************/
void	NX_CAS_MMI_RemoveEvtInfo(DxCAS_MMI_Event_t *pstEvent);
void	NX_CAS_MMI_GetEvtInfo(DxCAS_MMI_Event_t **ppstEvent);
void	NX_CAS_MMI_RemoveUiEvent(DxCAS_MmiSessionType_e eSessionType, DxCAS_MMI_Event_t *pstEvent);
void	NX_CAS_MMI_GetUiEvent(DxCAS_MmiSessionType_e eSession, Handle_t hSession, DxCAS_MMI_Event_t **ppstEvent);
void	NX_CAS_MMI_SendUiResponse(DxCAS_GroupId_e eCasGroupId, HUINT32 ulSlot, DxCAS_MmiSessionType_e eSession, Handle_t hSession, HUINT8 *pzInputData);
void	NX_CAS_MMI_UnRegisterSessionNotifier(DxCAS_MmiSessionType_e eSession);
void	NX_CAS_MMI_RegisterSessionNotifier(DxCAS_MmiSessionType_e eSession, APK_CAS_MMI_Notifier_t fnNotifier);


/******************************************************************************/
#define __CAS_ACTION_API_______________________________________________________
/******************************************************************************/
void	NX_CAS_ACTION_RegisterNotifier(APK_CAS_ACTION_Notifier_t fnNotfiler);
void	NX_CAS_ACTION_UnRegisterNotifier(APK_CAS_ACTION_Notifier_t fnNotfiler);
HERROR 	NX_CAS_ACTION_GetActionType(HUINT8 *pucActionType);
HERROR	NX_CAS_ACTION_GetSlotId(HUINT8 *pucSlotId);
HERROR 	NX_CAS_ACTION_GetCasId(HUINT8 *pucCasId);
HERROR 	NX_CAS_ACTION_GetData(HUINT8 *pucData);


/******************************************************************************/
#define __CAS_UTIL_API_______________________________________________________
/******************************************************************************/
DxCAS_GroupId_e		NX_CAS_GetCasGroupIdByCasTypeText(HUINT8 *pszCasType);


#endif // __NX_CORE_CAS_H__
