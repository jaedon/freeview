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
 * @file	  		nx_prism_cas_na_systemmessage.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_main.h>
#include <nx_common.h>

#include <nx_core.h>
#include <nx_port.h>
#include <nx_core_message.h>
#include <nx_core_cas.h>
#include <nx_core_cas_na.h>
#include <nx_prism_util_na.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_PRISM_UI_CASMSG_PARSE_ITEM_MAX					32
#define	NX_PRISM_UI_CASMSG_ERROR_CODE_LEN					9 // Error Code String + Error code 4digit


typedef enum
{
	eNxCAS_NA_SYSSTATUS_CLEAR			= 0x00000000,
	eNxCAS_NA_SYSSTATUS_ERROR			= 0x00000001,
	eNxCAS_NA_SYSSTATUS_LOCK			= 0x00010000,
} eNxUiCasNaSYSMsgStatus_e;


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct
{
	eNxUiCasNaSYSMsgStatus_e		eCasNaStatus;

	DxCAS_MMI_SessionEventType_e	eEventType;
	Handle_t						hSessionHandle;

	/* eDxCAS_SESSIONTYPE_BANNER */
	DxCAS_MmiSessionType_e			eSessionType;
	NX_CasNaMessage_Type 			eCasMessageType;
	HINT32				 			lErrorCode;

	HUINT8							*pszErrorMsg;
} NxUiCasNaSysMsgContext_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxUiCasNaSysMsgContext_t		s_stUiCasNaSysMsg;
static NX_PinDialogInstance_t		s_stUiSysMsg_PinCodeDialog;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_CasMsg_Na_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_CasMsg_Na_DestroyPinCodeDialog(void);
static void				nx_CasMsg_Na_CreatePinCodeDialog(HINT8 *pszStrId, NX_PINCODE_DATA_TYPE_t	pinType);

static NxUiCasNaSysMsgContext_t* 	nx_CasMsg_Na_GetContext(void);
static void 			nx_CasMsg_Na_StatusMsgCreate(HUINT8 *pMsg);
static void 			nx_CasMsg_Na_StatusMsgDestroy(void);
static HERROR			nx_CasMsg_Na_Update(NxUiCasNaSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_CasMsg_Na_ParseJsonString(NxUiCasNaSysMsgContext_t *pstContents, const HCHAR* pJsonString, NX_CasNaMessage_Type *peCasMessageType, HINT32 *plErrorCode);
static HERROR			nx_CasMsg_Na_ParseCustomString(HUINT8 *pszString, HUINT8 *pszMessage);
static HERROR			nx_CasMsg_Na_MakeErrorMessage(NxUiCasNaSysMsgContext_t *pstContents);
static HERROR			nx_CasMsg_Na_CheckErrorMessageState(NxUiCasNaSysMsgContext_t *pstContents, HUINT8 *pszMessage);

static HERROR			nx_CasMsg_Na_CheckValidCASMessageWithGroupIdAndSessionHandle(NxUiCasNaSysMsgContext_t *pstContents, DxCAS_GroupId_e eCasGroupId, Handle_t hCasSessionHandle);
static HERROR			nx_CasMsg_Na_ActionProcessBanner(NxUiCasNaSysMsgContext_t *pstContents, Handle_t hCasSessionHandle);
static HERROR			nx_CasMsg_Na_CheckLockState(NxUiCasNaSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2);

static ONDK_Result_t 	nx_CasMsg_Na_MsgGwmCreate(NxUiCasNaSysMsgContext_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_CasMsg_Na_MsgGwmDestroy(NxUiCasNaSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_CasMsg_Na_MsgGwmTimer(NxUiCasNaSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_CasMsg_Na_MsgGwmApkMsg(NxUiCasNaSysMsgContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_CasMsg_Na_MsgGwmApkMsgCasEvent(NxUiCasNaSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_CasMsg_Na_MsgGwmApkMsgCasUiTriggered(NxUiCasNaSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Pin_Code_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_CasMsg_Na_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_PinDlalog_ProtoProc(&s_stUiSysMsg_PinCodeDialog, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_CasMsg_Na_DestroyPinCodeDialog(void)
{
	return ONDK_GWM_APP_Destroy(nx_CasMsg_Na_DisplayPinCodeDialog);
}


static void		nx_CasMsg_Na_CreatePinCodeDialog(HINT8 *pszStrId, NX_PINCODE_DATA_TYPE_t	pinType)
{
	ONDK_Rect_t		stRect;
	HUINT32			ulAttribute = 0;
	ONDK_GWM_App_t					pGwmApp;

	pGwmApp = ONDK_GWM_APP_Get(nx_CasMsg_Na_DisplayPinCodeDialog);
	if(pGwmApp == NULL)
	{
		stRect = ONDK_Rect(NX_PRISM_UI_PIN_DIALOG_X, NX_PRISM_UI_PIN_DIALOG_Y, NX_PRISM_UI_PIN_DIALOG_WIDTH, NX_PRISM_UI_PIN_DIALOG_HEIGHT);
		ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE|NX_DLG_TIMEOUT_NOTIMEOUT);
		NX_PRISM_COMP_PinDlalog_SetDialog(&s_stUiSysMsg_PinCodeDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
									&stRect, ulAttribute, COL(C_T_Whistle_100), ONDK_GetString(pszStrId), pinType);
		/* Create Pop Up */
		ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_CasMsg_Na_DisplayPinCodeDialog", APP_DIALOGBOX_PRIORITY, nx_CasMsg_Na_DisplayPinCodeDialog, 0, 0, 0, 0);
	}
}


/******************************************************************************/
#define __Local_Functions_____________________________________________________
/******************************************************************************/

static NxUiCasNaSysMsgContext_t*	nx_CasMsg_Na_GetContext(void)
{
	return &s_stUiCasNaSysMsg;
}


static ONDK_Result_t	nx_CasMsg_Na_StatusMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_StatusMessage_Proc(message, hAction, p1, p2, p3);
}


static void		nx_CasMsg_Na_StatusMsgCreate(HUINT8 *pMsg)
{
	nx_CasMsg_Na_StatusMsgDestroy();

	if (pMsg == NULL)
	{
		NX_APP_Error("Error!!! (pMsg == NULL) \n");
		return;
	}

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW,	"nx_CasMsg_Na_StatusMsgProc", APP_ACTION_PRIORITY,
							nx_CasMsg_Na_StatusMsgProc, 0, (HINT32)pMsg, 0, 0);
}


static void		nx_CasMsg_Na_StatusMsgDestroy(void)
{
	ONDK_GWM_App_t	gwmApp = NULL;

	gwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)nx_CasMsg_Na_StatusMsgProc);
	if (gwmApp != NULL)
	{
		ONDK_GWM_APP_Destroy(nx_CasMsg_Na_StatusMsgProc);
	}
}


static HERROR	nx_CasMsg_Na_CheckLockState(NxUiCasNaSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, NULL);

	return ERR_OK;
}


static HERROR	nx_CasMsg_Na_Update(NxUiCasNaSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT8		*pSzStrID = NULL;

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	NX_APP_Trace("[+] nx_CasMsg_Na_Update() \n");
	NX_APP_Error("=====>[CAS-NA] nx_CasMsg_Na_Update() eCasNaStatus:(0x%x) \n", pstContents->eCasNaStatus);

	switch (pstContents->eCasNaStatus)
	{
		case eNxCAS_NA_SYSSTATUS_CLEAR:
			nx_CasMsg_Na_StatusMsgDestroy();
			nx_CasMsg_Na_MakeErrorMessage(pstContents);
			ONDK_GWM_SendMessage(NULL, MSG_APP_SYSTEM_CAS_STATE_UPDATE, (HINT32)NULL, eNxCasStatus_Clear, 0, 0);
			break;

		case eNxCAS_NA_SYSSTATUS_LOCK:
			nx_CasMsg_Na_StatusMsgDestroy();
			nx_CasMsg_Na_MakeErrorMessage(pstContents);
			nx_CasMsg_Na_CreatePinCodeDialog(pSzStrID, eNxPinCodeDataType_LockChannel);
			ONDK_GWM_SendMessage(NULL, MSG_APP_SYSTEM_CAS_STATE_UPDATE, (HINT32)NULL, eNxCasStatus_Lock, 0, 0);
			break;

		case eNxCAS_NA_SYSSTATUS_ERROR:
			nx_CasMsg_Na_StatusMsgDestroy();
			nx_CasMsg_Na_MakeErrorMessage(pstContents);
			nx_CasMsg_Na_StatusMsgCreate(pstContents->pszErrorMsg);
			ONDK_GWM_SendMessage(NULL, MSG_APP_SYSTEM_CAS_STATE_UPDATE, (HINT32)NULL, eNxCasStatus_Error, 0, 0);
			break;

		default:
			NX_APP_Error("Error!!! Invalid Status : (0x%x)\n", pstContents->eCasNaStatus);
			break;
	}

	return ERR_OK;
}


static ONDK_Result_t	nx_CasMsg_Na_ParseJsonString(NxUiCasNaSysMsgContext_t *pstContents, const HCHAR* pJsonString, NX_CasNaMessage_Type *peCasMessageType, HINT32 *plErrorCode)
{
	HERROR			hError = ERR_FAIL;
	HxJSON_t		root = NULL, szJsonUidata = NULL, szJsonStringitem = NULL, szJsonErrCodeItem = NULL, szJsonItem = NULL;
	const HCHAR 	*pszString = NULL, *pszCasType = NULL, *pszErrorItemId = NULL;
	HINT32			i = 0, lBannerItemCnt = 0, lErrorCode = -1, lSlotNum = 0, lErrorCodeItemCnt = 0, j = 0;
	DxCAS_GroupId_e 	eCasGroupId = eDxCAS_GROUPID_NONE;
	NX_CasNaMessage_Type eCasMessageType = eNxCasNaMessage_Type_None;

	if (pJsonString == NULL)
	{
		NX_APP_Error("Error!!! (pJsonString == NULL) \n");
		return	ONDK_RET_INITFAIL;
	}

	/*
		[ ] : Array
		{ } : Object
	*/

	root = HLIB_JSON_Decode(pJsonString);
	if (root == NULL)
	{
		NX_APP_Error("Error!!! (root == NULL) \n");
		return	ONDK_RET_INITFAIL;
	}

	pszString = HLIB_JSON_Encode(root, NULL, 0);
	NX_APP_Info("\n\n=====>[JSON Data] root : (%s) \n\n", pszString);

	// Get CasType
	pszCasType = HLIB_JSON_Object_GetString(root, (const HCHAR*)NX_CAS_PARSE_CASTYPE_ID);
	NX_APP_Print("[BANNER Info] - castype : (%s)\n", pszCasType);

	eCasGroupId = NX_CAS_GetCasGroupIdByCasTypeText((HUINT8*)pszCasType);
	if (eCasGroupId != eDxCAS_GROUPID_NA)
	{
		NX_APP_Error("Error!!! Not Support Cas Group:(0x%x)\n", eCasGroupId);
		return	ONDK_RET_INITFAIL;
	}

	// Get Slot Number
	lSlotNum = HLIB_JSON_Object_GetInteger(root, NX_CAS_PARSE_SLOTNUMBER_ID);
	NX_APP_Print("[BANNER Info] - slotnumber : (%d) \n\n", lSlotNum);

	// Check UI Type : Banner
	if (0 == HxSTD_StrNCmp(HLIB_JSON_Object_GetString(root, (HCHAR*)NX_CAS_PARSE_UITYPE_ID), (char*)NX_CAS_PARSE_UITYPE_BANNER_ID,  NX_PRISM_UI_CASMSG_PARSE_ITEM_MAX - 1))
	{
		// Get UI Data
		szJsonUidata = HLIB_JSON_Object_GetObject(root, (HCHAR*)NX_CAS_PARSE_UIDATA_ID);
		if (szJsonUidata != NULL)
		{
			pszString = HLIB_JSON_Encode(szJsonUidata, NULL, 0);
			NX_APP_Info("\n\n=====>[JSON Data] uidata : (%s) \n\n", pszString);

			// Get Banner Item
			szJsonStringitem = HLIB_JSON_Object_GetArray(szJsonUidata, (HCHAR*)NX_CAS_PARSE_STRING_ID);
			if (szJsonStringitem != NULL)
			{
				pszString = HLIB_JSON_Encode(szJsonStringitem, NULL, 0);
				NX_APP_Info("\n\n=====>[JSON Data] String Data : (%s) \n\n", pszString);

				lBannerItemCnt = HLIB_JSON_GetSize(szJsonStringitem);
				NX_APP_Print("[BANNER Info] - lMenuItemCnt :(%d)\n", lBannerItemCnt);
				for (i = 0; i < lBannerItemCnt; ++i)
				{
					// Get Item
					szJsonItem = HxJSON_Array_Get(szJsonStringitem, i);
					if (szJsonItem != NULL)
					{
						pszString = HLIB_JSON_Encode(szJsonItem, NULL, 0);
						NX_APP_Info("=====>[JSON Data] i:[%d], pszString:(%s)\n", i, pszString);

						pszErrorItemId = HLIB_JSON_Object_GetString(szJsonItem, NX_CAS_PARSE_ID_ID);
							NX_APP_Print("[BANNER Info] - pszMenuItemId:(%s)\n", pszErrorItemId);

						hError = NX_CAS_NAGRA_GetErrorMessage((HCHAR*)pszErrorItemId, &eCasMessageType);
						if (hError != ERR_OK)
						{
							NX_APP_Error("Error!!! NX_CAS_NAGRA_GetErrorMessage() - pszErrorItemId : (%s) \n", pszErrorItemId);
						}

						szJsonErrCodeItem = HLIB_JSON_Object_GetArray(szJsonItem, (HCHAR*)NX_CAS_PARSE_IN_ID);
						pszString = HLIB_JSON_Encode(szJsonErrCodeItem, NULL, 0);
						NX_APP_Info("=====>[JSON Data] Error Code pszString:(%s)\n", pszString);

						lErrorCodeItemCnt = HLIB_JSON_GetSize(szJsonErrCodeItem);
						NX_APP_Print("[BANNER Info] - lErrorCodeItemCnt :(%d)\n", lErrorCodeItemCnt);
						for (j = 0; j < lErrorCodeItemCnt; j++)
						{
							szJsonItem = HxJSON_Array_Get(szJsonErrCodeItem, j);
							if (szJsonItem != NULL)
							{
								lErrorCode = (HINT32)HLIB_JSON_Get(szJsonItem, NULL);
								NX_APP_Print("[BANNER Info] j:[%d], lErrorCode:(%d)\n", j, lErrorCode);
							}
						}
					}
				}
			}
		}
	}

	*peCasMessageType = eCasMessageType;
	*plErrorCode = lErrorCode;

	return	ONDK_RET_OK;
}


static HERROR	nx_CasMsg_Na_ParseCustomString(HUINT8 *pszString, HUINT8 *pszMessage)
{
	HCHAR		*pszStrDump = NULL;

	if (pszString == NULL)
	{
		NX_APP_Error("Error !!! (pszString == NULL) \n");
		return	ERR_FAIL;
	}

	pszStrDump = HLIB_STD_StrDup((HCHAR*)pszString);
	if (NULL == pszStrDump)
	{
		NX_APP_Error("HLIB_STD_StrDup error!\r\n");
		return ERR_FAIL;
	}

	NX_APP_Info("Original pszMessage:(%s)\n", pszMessage);

#if 0
	if (HxSTD_StrStr((char*)pszString, (char*)NX_CAS_PARSE_LINE_SPACE_STR))
	{
		HLIB_STD_StrReplaceChar((HCHAR*)pszString, (HCHAR*)pszStrDump, '%0A', '\n');
	}
	else
	{
		HLIB_STD_StrReplaceChar((HCHAR*)pszString, (HCHAR*)pszStrDump, 'LF', '\n');
	}
#endif
	NX_APP_Info("Replace pszMessage:(%s)\n", pszString);

	if (pszStrDump)
	{
		HLIB_STD_MemFree(pszStrDump);
		pszStrDump = NULL;
	}

	NX_COMMON_NOT_USED_PARAM(pszString);
	NX_COMMON_NOT_USED_PARAM(pszMessage);

	return	ERR_OK;
}


static HERROR	nx_CasMsg_Na_MakeErrorMessage(NxUiCasNaSysMsgContext_t *pstContents)
{
	HINT8	*pszStrId = NULL;
	HUINT8	*pszText = NULL;
	HINT32	lStrLen = 0;

	if (pstContents->pszErrorMsg)
	{
		NX_APP_Free(pstContents->pszErrorMsg);
		pstContents->pszErrorMsg = NULL;
	}

	if (pstContents->eCasNaStatus == eNxCAS_NA_SYSSTATUS_ERROR)
	{
		pszStrId = NX_PRISM_UTIL_NA_GetResStringIdByErrorType(pstContents->eCasMessageType);
		if (pszStrId != NULL)
		{
			pszText = ONDK_GetString(pszStrId);
			if (pszText)
			{
				lStrLen = HxSTD_StrLen((HCHAR*)pszText);
				lStrLen += NX_PRISM_UI_CASMSG_ERROR_CODE_LEN;
				pstContents->pszErrorMsg = NX_APP_Calloc(lStrLen);
				if (pstContents->pszErrorMsg == NULL)
				{
					NX_APP_Error("Error!!! NX_APP_Calloc() \n");
				}
				else
				{

					HxSTD_snprintf((char*)pstContents->pszErrorMsg, lStrLen, (char*)"(E-%d) %s", pstContents->lErrorCode, (char*)pszText);
				}
			}
		}
	}

	return	ERR_OK;
}


static HERROR	nx_CasMsg_Na_CheckErrorMessageState(NxUiCasNaSysMsgContext_t *pstContents, HUINT8 *pszMessage)
{
	HINT32		lStrLen = 0, lErrorCode = -1;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	NX_CasNaMessage_Type eCasMessageType = eNxCasNaMessage_Type_None;

	lStrLen = HxSTD_StrLen((HCHAR*)pszMessage);
	if (lStrLen > 0)
	{
		eResult = nx_CasMsg_Na_ParseJsonString(pstContents, (const HCHAR*)pszMessage, &eCasMessageType, &lErrorCode);
		if (eResult != ONDK_RET_OK)
		{
			NX_APP_Error("Error!!! nx_CasMsg_Na_ParseJsonString() \n");
			return	ERR_FAIL;
		}

		if ((eCasMessageType == eNxCasNaMessage_Type_None) || (eCasMessageType == eNxCasNaMessage_SMARTCARD_OK))
		{
			pstContents->eCasNaStatus = eNxCAS_NA_SYSSTATUS_CLEAR;
			pstContents->eCasMessageType = eNxCasNaMessage_Type_None;
			pstContents->lErrorCode = -1;
		}
		else
		{
			if ((pstContents->eCasMessageType != eCasMessageType) || (pstContents->lErrorCode != lErrorCode))
			{
				pstContents->eCasNaStatus = eNxCAS_NA_SYSSTATUS_ERROR;
				pstContents->eCasMessageType = eCasMessageType;
				pstContents->lErrorCode = lErrorCode;
			}
		}
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(pszMessage);

	return ERR_OK;
}


static HERROR	nx_CasMsg_Na_CheckValidCASMessageWithGroupIdAndSessionHandle(NxUiCasNaSysMsgContext_t *pstContents, DxCAS_GroupId_e eCasGroupId, Handle_t hCasSessionHandle)
{
	HBOOL	bValidSession = FALSE;


	NX_COMMON_NOT_USED_PARAM(pstContents);

	// Check Group
	if (eCasGroupId != eDxCAS_GROUPID_NA)
	{
		NX_APP_Error("Error!!! Only process NAGRA CAS - eCasGroupId:(0x%x)!! \n", eCasGroupId);
		return	ERR_FAIL;
	}

	// Chedk Handle
	bValidSession = NX_CAS_IsValidSession(eNxCasNAHandleKey_Banner, (HUINT32)hCasSessionHandle, eCasGroupId);
	if (bValidSession == TRUE)
	{
		return	ERR_OK;
	}
	else
	{
		NX_APP_Error("Error!!! NX_CAS_IsValidSession() Fail- eNxCasNAHandleKey_Banner, hCasSessionHandle(0x%x), eCasGroupId:(0x%x)!! \n", hCasSessionHandle, eCasGroupId);
		return	ERR_FAIL;
	}
}


static HERROR	nx_CasMsg_Na_ActionProcessBanner(NxUiCasNaSysMsgContext_t *pstContents, Handle_t hCasSessionHandle)
{
	HERROR		hError = ERR_FAIL;
	HUINT8 		*pszMessage = NULL;
	HUINT32		ulSlot = 0;
	DxCAS_GroupId_e 	eCasGroupId = eDxCAS_GROUPID_NONE;

	NX_APP_Trace("[+] nx_CasMsg_Na_ActionProcessBanner() \n");

	hError = NX_CAS_GetCasMessage(hCasSessionHandle, &eCasGroupId, &ulSlot, (HUINT8 **)&pszMessage);
	if (hError == ERR_OK)
	{
		NX_APP_Info("[CAS-NA] Banner Message - eCasGroupId:(0x%x), ulSlot:(0x%x), pszMessage:(%x)\n", eCasGroupId, ulSlot, pszMessage);
		hError = nx_CasMsg_Na_CheckValidCASMessageWithGroupIdAndSessionHandle(pstContents, eCasGroupId, hCasSessionHandle);
		if (hError == ERR_OK)
		{
			hError = nx_CasMsg_Na_CheckErrorMessageState(pstContents, pszMessage);
			if (hError == ERR_OK)
			{
				nx_CasMsg_Na_Update(pstContents, 0, 0, 0);
			}
		}
		else
		{
			NX_APP_Error("Error!!! nx_CasMsg_Na_CheckValidCASMessageWithGroupIdAndSessionHandle() Fail!! \n");
			hError = ERR_FAIL;
		}
	}
	else
	{
		hError = ERR_FAIL;
	}

	if (pszMessage)
	{
		NX_CAS_ReleaseCasMessage(pszMessage);
		pszMessage = NULL;
	}

	return	hError;
}


static HERROR	nx_CasMsg_Na_Initilization(NxUiCasNaSysMsgContext_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	NX_COMMON_NOT_USED_PARAM(hAct);

	HxSTD_MemSet(pstContents, 0x00, sizeof(NxUiCasNaSysMsgContext_t));
	pstContents->eCasNaStatus = eNxCAS_NA_SYSSTATUS_CLEAR;
	pstContents->eEventType = eDxCASMMI_EVENTTYPE_NONE;
	pstContents->eSessionType = eDxCAS_SESSIONTYPE_NONE;
	pstContents->hSessionHandle = HANDLE_NULL;
	pstContents->eCasMessageType = eNxCasNaMessage_Type_None;
	pstContents->lErrorCode = -1;

	return ERR_OK;
}


static ONDK_Result_t	nx_CasMsg_Na_MsgGwmCreate(NxUiCasNaSysMsgContext_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	nx_CasMsg_Na_Initilization(pstContents, hAct, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_CasMsg_Na_MsgGwmDestroy(NxUiCasNaSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Trace();

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	nx_CasMsg_Na_StatusMsgDestroy();

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_CasMsg_Na_MsgGwmTimer(NxUiCasNaSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (p1)
	{
		default:
			break;
	}

	return GWM_RET_OK;
}


static ONDK_Result_t	nx_CasMsg_Na_MsgGwmApkMsg(NxUiCasNaSysMsgContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	ONDK_Result_t			eResult = ONDK_RET_FAIL;
	GWM_APK_MSG_Type_e		eAPK_Type = (GWM_APK_MSG_Type_e)handle;


	switch(eAPK_Type)
	{
		case GWM_APK_CAS_MESSAGE_EVENT:
			nx_CasMsg_Na_MsgGwmApkMsgCasEvent(pstContents, p1, p2, p3);
			break;

		case GWM_APK_CAS_UI_TRIGGERED:
			nx_CasMsg_Na_MsgGwmApkMsgCasUiTriggered(pstContents, p1, p2, p3);
			break;

		default:
			NX_APP_Error("Error!!! SKIP APK Message : (0x%x) \n", eAPK_Type);
			break;
	}

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_CasMsg_Na_MsgGwmApkMsgCasEvent(NxUiCasNaSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Handle_t						hSession = HANDLE_NULL;
	DxCAS_MMI_SessionEventType_e	eEventType = eDxCASMMI_EVENTTYPE_NONE;
	DxCAS_MmiSessionType_e			eSessionType = eDxCAS_SESSIONTYPE_NONE;


	eEventType = (DxCAS_MMI_SessionEventType_e)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;
	hSession = (Handle_t)p3;

	switch (eEventType)
	{
		case eDxCASMMI_EVENTTYPE_DISPLAYUI:
			switch (eSessionType)
			{
				case eDxCAS_SESSIONTYPE_BANNER:
					pstContents->eEventType = eEventType;
					pstContents->eSessionType = eSessionType;
					pstContents->hSessionHandle = hSession;
					nx_CasMsg_Na_ActionProcessBanner(pstContents, hSession);
					break;
				default:
					NX_APP_Error("Error!!! Not Support eSession Type : (0x%x) \n", eSessionType);
					break;
			}
			break;

		default:
			NX_APP_Error("Error!!! Not Support Event Type : (0x%x) \n", eEventType);
			break;
	}

	return GWM_RET_OK;
}


static ONDK_Result_t	nx_CasMsg_Na_MsgGwmApkMsgCasUiTriggered(NxUiCasNaSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Handle_t						hSession = HANDLE_NULL;
	DxCAS_MMI_SessionEventType_e	eEventType = eDxCASMMI_EVENTTYPE_NONE;
	DxCAS_MmiSessionType_e			eSessionType = eDxCAS_SESSIONTYPE_NONE;


	eEventType = (DxCAS_MMI_SessionEventType_e)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;
	hSession = (Handle_t)p3;

	switch (eEventType)
	{
		case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTART:
			switch (eSessionType)
			{
				case eDxCAS_SESSIONTYPE_EXTENSION:
					break;
				default:
					NX_APP_Error("Error!!! Not Support eSession Type : (0x%x) \n", eSessionType);
					break;
			}
			break;

		default:
			NX_APP_Error("Error!!! Not Support Event Type : (0x%x) \n", eEventType);
			break;
	}

	return GWM_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_CASMESSAGE_NA_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t				eResult = ONDK_RET_OK;
	NxUiCasNaSysMsgContext_t	*pstContents = nx_CasMsg_Na_GetContext();

	ONDK_Print("Message(%d) : %s, handle: %p p1(0x%x) p2(0x%x) p3(0x%x) \n", lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			nx_CasMsg_Na_MsgGwmCreate(pstContents, hHandle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			eResult = nx_CasMsg_Na_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYDOWN:
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_APK_MESSAGE:
			eResult = nx_CasMsg_Na_MsgGwmApkMsg(pstContents, hHandle, p1, p2, p3);
			break;

		case MSG_APP_SYSTEM_PIN_OK:
			NX_APP_Message("MSG_APP_SYSTEM_PIN_OK\n");
			nx_CasMsg_Na_DestroyPinCodeDialog();
			eResult = ONDK_MESSAGE_BREAK;
			break;

		case MSG_APP_SYSTEM_PIN_DENIED:
			NX_APP_Message("MSG_APP_SYSTEM_PIN_DENIED\n");
			nx_CasMsg_Na_DestroyPinCodeDialog();
			eResult = ONDK_MESSAGE_BREAK;
			break;

		/********************************************************
		* Live
		********************************************************/
		case MSG_APP_SYSTEM_SVC_CHANGED:
			NX_APP_Message("____MSG_APP_SYSTEM_SVC_CHANGED\n");
			pstContents->eCasNaStatus = eNxCAS_NA_SYSSTATUS_CLEAR;
			nx_CasMsg_Na_DestroyPinCodeDialog();
			nx_CasMsg_Na_Update(pstContents, p1, p2, p3);
			break;

		case MSG_APP_CHANNEL_TUNE_LOCKED:
			NX_APP_Message("____MSG_APP_CHANNEL_TUNE_LOCKED\n");
			break;

		case MSG_APP_CHANNEL_TUNE_NOSIGNAL:
			NX_APP_Message("____MSG_APP_CHANNEL_TUNE_NOSIGNAL\n");
			break;

		case MSG_APP_CHANNEL_LIVE_VIEWSTATECHANGED:
			NX_APP_Message("____MSG_APP_CHANNEL_LIVE_VIEWSTATECHANGED [%d]\n");
			break;

		case MSG_APP_CHANNEL_LIVE_LOCKSTATECHANGED:
			NX_APP_Message("____MSG_APP_CHANNEL_LIVE_LOCKSTATECHANGED [%d]\n");
			break;

		case MSG_APP_CHANNEL_PINCTRLCHANGED:
			NX_APP_Message("____MSG_APP_CHANNEL_PINCTRLCHANGED\n");
			break;

		case MSG_GWM_STACK_CHANGED :
			NX_APP_Message("____MSG_GWM_STACK_CHANGED\n");
			break;

		/********************************************************
		* PVR
		********************************************************/
		case MSG_GWM_DESTROY:
			eResult = nx_CasMsg_Na_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
	}

	if (eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
