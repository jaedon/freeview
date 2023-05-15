/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_cas_mmi.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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
#include <hlib.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>
#include <svc_pipe.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_cas.h>

#include <otl.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	OM_CASMMI_MAX_MAIL_SIZE			1024
#define	OM_CASMMI_MAX_MAIL_COUNT		20
#define	OM_CASMMI_MAX_MAIL_LIST_SIZE			OM_CASMMI_MAX_MAIL_SIZE*OM_CASMMI_MAX_MAIL_COUNT
#define	OM_CASMMI_DEFAULT_SIZE			512

#define 	OM_CASMMI_DECODE_OPT_DELIM						"|"

STATIC HULONG					s_ulOmCasMmiSemId;

#define	ENTER_OM_CAS_MMI_CACHE	VK_SEM_Get(s_ulOmCasMmiSemId)
#define	LEAVE_OM_CAS_MMI_CACHE	VK_SEM_Release(s_ulOmCasMmiSemId)



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct{
	DxCAS_GroupId_e				eCasGrpId;
	BUS_Callback_t					fnCasUiProc;
	HxList_t						*pstSessionList;
	Handle_t						hAction;
} OmCasMmiInstanceInfo_t;


typedef struct
{
	DxCAS_GroupId_e					eCasGrpId;
	DxCAS_MmiSessionType_e				eSessionType;
	HUINT32							nSlot;
	HBOOL							bStarted;
	Handle_t							hSession;
} OmCasMmiConnectionInfo_t;

typedef struct{
	DxCAS_MmiSessionType_e			eSessionType;
	HxList_t						*pstConn;
	Handle_t							hSession;
	HUINT32						uiHandle;
} OmCasMmiSessionInfo_t;

typedef struct{
	OxMgrCasUiType_e			eUiType;
	void						*pvUiData;
} OmCasMmiUiDataEventInfo_t;



typedef	enum
{
	eTAG_Pair,
	eTAG_Integer,
	eTAG_Float,
	eTAG_String,
	eTAG_Bool,
	eTAG_Binary
} OmCasMmiTagType_e;

typedef	struct
{
	HCHAR		*tag;
	HUINT32		value;
} OmCasMmiTag2Enum_t;

typedef	struct
{
	OmCasMmiTagType_e	tag_type;
	OmCasMmiTag2Enum_t	taglist[];
} _OmCasMmiTag2Enum_t;

typedef	struct	_PROP2ENUM
{
	HCHAR			*property;
	_OmCasMmiTag2Enum_t		*enumtable;
	HUINT32			offset;

	HINT32				sizeofstructure;
	struct	_PROP2ENUM	*propTable;

	HINT32				ObjectCount;
} _OmCasMmiProp2Enum_t;



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HxList_t				*_pstSubCasProcList = NULL;

STATIC _OmCasMmiTag2Enum_t		GTAG_INT_TABLE =
{
	eTAG_Integer,
	{
		{	NULL,			eMgrCasInputType_MAX		}
	}
};

STATIC _OmCasMmiTag2Enum_t		GTAG_STRING_TABLE =
{
	eTAG_String,
	{
		{	NULL,			eMgrCasInputType_MAX		}
	}
};


STATIC _OmCasMmiTag2Enum_t		_casui_app_inputtype_table =
{
	eTAG_Pair,
	{
		{	"button",		eMgrCasInputType_Button		},
		{	"number",		eMgrCasInputType_Number		},
		{	"string",		eMgrCasInputType_String		},
		{	"selection",	eMgrCasInputType_Selection	},
		{	"etc",			eMgrCasInputType_Etc		},
		{	NULL,			eMgrCasInputType_MAX		}
	}
};

STATIC	_OmCasMmiTag2Enum_t		_casui_app_inputdata_buttontype_enumTable =
{
	eTAG_Pair,
	{
		{	"ok",			eMgrCasBtn_OK				},
		{	"yes",			eMgrCasBtn_YES				},
		{	"no",			eMgrCasBtn_NO				},
		{	"cancel",		eMgrCasBtn_CANCEL			},
		{	"1st",			eMgrCasBtn_1st				},
		{	"2nd",			eMgrCasBtn_2nd				},
		{	"3rd",			eMgrCasBtn_3rd				},
		{	NULL,			0 							}
	}
};

STATIC	_OmCasMmiTag2Enum_t		_casui_app_inputdata_etctype_enumTable =
{
	eTAG_Pair,
	{
		{	"previous",		eMgrCasEtc_Previous			},
		{	"exit",			eMgrCasEtc_Exit				},
		{	NULL,			0 							}
	}
};



STATIC _OmCasMmiTag2Enum_t		_oapi_cas_mmi_menugroup_table =
{
	eTAG_Pair,
	{
		{	"none",					eOxCASMMI_MENUGROUP_NONE		},
		{	"menugroup1",				eOxCASMMI_MENUGROUP_1			}, //IRDETO: SmartCardStatus,
		{	"menugroup2",				eOxCASMMI_MENUGROUP_2			}, //IRDETO: SoftCellServiceStatus,
		{	"menugroup3",				eOxCASMMI_MENUGROUP_3			}, //IRDETO: LoaderStatus,
		{	"menugroup4",				eOxCASMMI_MENUGROUP_4			}, //IRDETO: Components,
		{	"menugroup5",				eOxCASMMI_MENUGROUP_5			}, //IRDETO: MailMessages,
		{	NULL,					eOxCASMMI_MENUGROUP_NONE		}
	}
};

STATIC	_OmCasMmiProp2Enum_t		_casui_app_inputdata_button_propertyTable[] =
{
	{"buttontype",		&_casui_app_inputdata_buttontype_enumTable, offsetof(OxMgrCasUiNotifyButton_t, eMgrCasBtnType), 0, NULL, 4},
	{NULL, NULL, 0, 0, NULL, 0}
};

STATIC	_OmCasMmiProp2Enum_t		_casui_app_inputdata_number_propertyTable[] =
{
	{"number",			&GTAG_INT_TABLE,							offsetof(OxMgrCasUiNotifyNumber_t, ulNumber), 0, NULL, 4},
	{"numberlength",	&GTAG_INT_TABLE,							offsetof(OxMgrCasUiNotifyNumber_t, ulNumberLength), 0, NULL, 4},
	{"numberstring",	&GTAG_STRING_TABLE,							offsetof(OxMgrCasUiNotifyNumber_t, pszNumber), MGR_CASINPUT_NUMBER_MAX_LENGTH, NULL, 4},
	{NULL, NULL, 0, 0, NULL, 0}
};

STATIC	_OmCasMmiProp2Enum_t		_casui_app_inputdata_string_propertyTable[] =
{
	{"stringlength",	&GTAG_INT_TABLE,							offsetof(OxMgrCasUiNotifyString_t, ulStringLength), 0, NULL, 4},
	{"string",			&GTAG_STRING_TABLE,							offsetof(OxMgrCasUiNotifyString_t, szString), 0, NULL, 4},
	{NULL, NULL, 0, 0, NULL, 0}
};

STATIC	_OmCasMmiProp2Enum_t		_casui_app_inputdata_selection_propertyTable[] =
{
	{"itemidx",			&GTAG_INT_TABLE,							offsetof(OxMgrCasUiNotifySelection_t, ulItemIdx), 0, NULL, 4},
	{"datalength",		&GTAG_INT_TABLE,							offsetof(OxMgrCasUiNotifySelection_t, ulDataLength), 0, NULL, 4},
	{"data",			&GTAG_STRING_TABLE,							offsetof(OxMgrCasUiNotifySelection_t, aucData), MGR_CAS_STR_LENGTH_SHORT, NULL, 4},
	{NULL, NULL, 0, 0, NULL, 0}
};

STATIC	_OmCasMmiProp2Enum_t		_casui_app_inputdata_etc_propertyTable[] =
{
	{"etctype",			&_casui_app_inputdata_etctype_enumTable,	offsetof(OxMgrCasUiNotifyEtc_t, eMgrCasEtcType), 0, NULL, 4},
	{NULL, NULL, 0, 0, NULL, 0}
};


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
STATIC OmCasMmiSessionInfo_t *om_cas_mmi_GetSessionInfoByTypeAndUiHandle(HxList_t *pstSessionList, DxCAS_MmiSessionType_e eSessionType, HUINT32 ulUiHandle);
STATIC HERROR om_cas_mmi_RemoveSessionInfo(HxList_t **ppstSessionList, OmCasMmiSessionInfo_t *pstSessionInfo);
STATIC HERROR om_cas_mmi_AppendSessionInfo(HxList_t **ppstSessionList,
	HUINT32 ulUiHandle, DxCAS_MmiSessionType_e eSessionType, Handle_t hSession, OxCasMmiSessionStatus_e eSessionStatus, OmCasMmiSessionInfo_t **ppstSessionInfo);
STATIC OmCasMmiSessionInfo_t *om_cas_mmi_GetSessionInfoBySessionTypeAndSessionHandle(HxList_t *pstSessionList, DxCAS_MmiSessionType_e eSessionType, Handle_t hSession);

STATIC OmCasMmiInstanceInfo_t *om_cas_mmi_AppendSubCasInst(DxCAS_GroupId_e eCasGrpId, BUS_Callback_t fnCasUiProc);
STATIC OmCasMmiInstanceInfo_t *om_cas_mmi_GetSubCasInstInfo(DxCAS_GroupId_e eCasGrpId);
STATIC HERROR	om_cas_mmi_SendCasEventToCasUiProc(DxCAS_GroupId_e eCasGrpId, HUINT32 ulSlotNum, Handle_t hAction, HINT32 nMessage, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t	om_cas_mmi_PostCasEventDataToCasUiProc(DxCAS_GroupId_e eCasGrpId, HUINT32 ulSlotNum, Handle_t hAction, HINT32 nMessage, HINT32 p1, HINT32 p2, void* pDATA, HUINT32 ulSize);
STATIC OmCasMmiSessionInfo_t *om_cas_mmi_GetSessionInfoByType(HxList_t *pstSessionList, DxCAS_MmiSessionType_e eSessionType);


STATIC HERROR om_cas_mmi_SessionProcEvent(HINT32 event, DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, void *pvSendData);
STATIC HERROR om_cas_mmi_GetUiType(HINT32 eEvent, OxMgrCasUiType_e *pUiType);
STATIC HERROR om_cas_mmi_UiDataProcEvent(HINT32 event, DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, void *pvPostData);
STATIC OmCasMmiSessionInfo_t *om_cas_mmi_GetSessionInfoBySessionType(HxList_t *pstSessionList, DxCAS_MmiSessionType_e eSessionType);
STATIC void om_cas_mmi_ChangeSessionState(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData);
STATIC void om_cas_mmi_OpenSession(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData);
STATIC void om_cas_mmi_CloseSession(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData);
STATIC void om_cas_mmi_UiResponse(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData);
STATIC HCHAR	*om_cas_mmi_EncodeUiDataProperty(DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, OxMgrCasUiType_e eUiType, void *pvUiData, OxMgrCasUIAttribute_e *pulAttr);

#define ___LOCAL_FUNCTIONS___






STATIC OmCasMmiInstanceInfo_t *om_cas_mmi_GetSubCasInstInfo(DxCAS_GroupId_e eCasGrpId)
{
	HxList_t					*pstInstItem = NULL;
	OmCasMmiInstanceInfo_t	*pstInstInfo = NULL;

	pstInstItem = _pstSubCasProcList;
	while(pstInstItem != NULL)
	{
		pstInstInfo = (OmCasMmiInstanceInfo_t *)HLIB_LIST_Data(pstInstItem);
		if(pstInstInfo != NULL)
		{
			if((pstInstInfo->eCasGrpId == eCasGrpId))
			{
				break;
			}
		}

		pstInstItem = pstInstItem->next;
		pstInstInfo = NULL;
	}

	return pstInstInfo;
}

STATIC OmCasMmiSessionInfo_t *om_cas_mmi_GetSessionInfoByType(HxList_t *pstSessionList, DxCAS_MmiSessionType_e eSessionType)
{
	HxList_t					*pstSessionItem = NULL;
	OmCasMmiSessionInfo_t	*pstSessionInfo = NULL;


	if(pstSessionList == NULL)
	{
		goto END_FUNC;
	}

	pstSessionItem = pstSessionList;
	while(pstSessionItem != NULL)
	{
		pstSessionInfo = (OmCasMmiSessionInfo_t *)HLIB_LIST_Data(pstSessionItem);
		if(pstSessionInfo != NULL)
		{
			if(pstSessionInfo->eSessionType == eSessionType)
			{
				break;
			}
		}

		pstSessionItem = pstSessionItem->next;
		pstSessionInfo = NULL;
	}

END_FUNC:
	return pstSessionInfo;
}

STATIC OmCasMmiSessionInfo_t *om_cas_mmi_GetSessionInfoByTypeAndUiHandle(HxList_t *pstSessionList, DxCAS_MmiSessionType_e eSessionType, HUINT32 ulUiHandle)
{
	HxList_t					*pstSessionItem = NULL;
	OmCasMmiSessionInfo_t	*pstSessionInfo = NULL;


	if(pstSessionList == NULL)
	{
		goto END_FUNC;
	}

	pstSessionItem = pstSessionList;
	while(pstSessionItem != NULL)
	{
		pstSessionInfo = (OmCasMmiSessionInfo_t *)HLIB_LIST_Data(pstSessionItem);
		if(pstSessionInfo != NULL)
		{
			if(pstSessionInfo->eSessionType == eSessionType && pstSessionInfo->uiHandle == ulUiHandle)
			{
				break;
			}
		}

		pstSessionItem = pstSessionItem->next;
		pstSessionInfo = NULL;
	}

END_FUNC:
	return pstSessionInfo;
}

STATIC HERROR om_cas_mmi_RemoveSessionInfo(HxList_t **ppstSessionList, OmCasMmiSessionInfo_t *pstSessionInfo)
{
	HERROR							hError = ERR_OK;

	HLIB_LIST_RemoveAll(pstSessionInfo->pstConn);
	*ppstSessionList = HLIB_LIST_Remove(*ppstSessionList, (void *)pstSessionInfo);

	return hError;
}

STATIC HERROR om_cas_mmi_AppendSessionInfo(HxList_t **ppstSessionList,
	HUINT32 ulUiHandle, DxCAS_MmiSessionType_e eSessionType, Handle_t hSession, OxCasMmiSessionStatus_e eSessionStatus, OmCasMmiSessionInfo_t **ppstSessionInfo)
{
	HERROR							hError = ERR_OK;
	OmCasMmiSessionInfo_t		*pstSessionInfo = NULL;

	pstSessionInfo = (OmCasMmiSessionInfo_t *)HLIB_STD_MemCalloc(sizeof(OmCasMmiSessionInfo_t));

	if(pstSessionInfo != NULL)
	{
		pstSessionInfo->eSessionType = eSessionType;

		pstSessionInfo->uiHandle = ulUiHandle;
		pstSessionInfo->hSession = hSession;

		*ppstSessionList = HLIB_LIST_Append(*ppstSessionList, (void *)pstSessionInfo);
	}

	if(pstSessionInfo == NULL)
	{
		hError = ERR_FAIL;
	}
	else
	{
		(*ppstSessionInfo) = pstSessionInfo;
	}

	return hError;
}




STATIC HERROR om_cas_mmi_GetUiType(HINT32 eEvent, OxMgrCasUiType_e *pUiType)
{
	HERROR				hError = ERR_OK;
	OxMgrCasUiType_e	eUiType = eMgrCasUiType_MAX;

	if(pUiType == NULL)
	{
		HxLOG_NullPrint("UITYPE NULL\n");
		goto END_FUNC;
	}

	switch(eEvent)
	{
		case eMEVT_CASUI_TYPE_BANNER_DATA:	eUiType = eMgrCasUiType_Banner;		break;
		case eMEVT_CASUI_TYPE_TEXTBOX_DATA:	eUiType = eMgrCasUiType_TextBox;	break;
		case eMEVT_CASUI_TYPE_MAIL_DATA:		eUiType = eMgrCasUiType_MailMessage;break;
		case eMEVT_CASUI_TYPE_MAILLIST_DATA:		eUiType = eMgrCasUiType_MailMessageList;break;
		case eMEVT_CASUI_TYPE_MENULIST_DATA:	eUiType = eMgrCasUiType_MenuList;	break;
		case eMEVT_CASUI_TYPE_GRID_DATA:		eUiType = eMgrCasUiType_Grid;		break;
		case eMEVT_CASUI_TYPE_INPUT_DATA:		eUiType = eMgrCasUiType_InputDialog;break;
		case eMEVT_CASUI_TYPE_INPUT_EX_DATA:	eUiType = eMgrCasUiType_InputDialog_Ex;break;
		case eMEVT_CASUI_TYPE_CLOSE:			eUiType = eMgrCasUiType_MAX;		break;		// 임시 코드 임. 곧 제거 되어야 함.
		default:
			HxLOG_Error("unknow type\n");
			hError = ERR_FAIL;

			goto END_FUNC;
			break;
	}

	*pUiType = eUiType;

END_FUNC:
	return hError;
}


STATIC OmCasMmiInstanceInfo_t *om_cas_mmi_AppendSubCasInst(DxCAS_GroupId_e eCasGrpId, BUS_Callback_t fnCasUiProc)
{
	OmCasMmiInstanceInfo_t	*pstInstInfo = NULL;

	HxLOG_Trace();

	pstInstInfo = (OmCasMmiInstanceInfo_t *)HLIB_STD_MemCalloc(sizeof(OmCasMmiInstanceInfo_t));
	if(pstInstInfo != NULL)
	{
		pstInstInfo->eCasGrpId = eCasGrpId;
		pstInstInfo->fnCasUiProc = fnCasUiProc;
		pstInstInfo->hAction = (Handle_t)NULL;
		pstInstInfo->pstSessionList = NULL;

		_pstSubCasProcList = HLIB_LIST_Append(_pstSubCasProcList, pstInstInfo);
	}

	return pstInstInfo;
}




STATIC HERROR	om_cas_mmi_SendCasEventToCasUiProc(DxCAS_GroupId_e eCasGrpId, HUINT32 ulSlotNum, Handle_t hAction, HINT32 nMessage, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR						hError = ERR_OK;
//	HUINT32 					ulSlotNum = 0;
	OmCasMmiInstanceInfo_t	*pstInstInfo = NULL;

//	ulSlotNum = (HUINT32)p1;
	HxLOG_Info("eCasGrpId: 0x%x, ulSlotNum: %d, hAction: 0x%x, nMessage: 0x%x, p1: 0x%x, p2: 0x%x, p3: 0x%x \n", eCasGrpId, ulSlotNum, hAction, nMessage, p1, p2, p3);
	pstInstInfo = om_cas_mmi_GetSubCasInstInfo(eCasGrpId);
	if((pstInstInfo == NULL) || (pstInstInfo->fnCasUiProc == NULL) || (BUS_MGR_Get(pstInstInfo->fnCasUiProc) == NULL))
	{
		HxLOG_Error("not found inst 0x%x slot %d\n", eCasGrpId, ulSlotNum);
		hError = ERR_FAIL;

		goto END_FUNC;
	}

	BUS_SendMessage(pstInstInfo->fnCasUiProc, nMessage, hAction, p1, p2, p3);

END_FUNC:
	return hError;
}


// om_cas_mmi_PostCasEventDataToCasUiProc 를 부르는 친구가 pData 를 free하도록 되어서...여기서는 free 안함. 추후 수정.
STATIC BUS_Result_t	om_cas_mmi_PostCasEventDataToCasUiProc(DxCAS_GroupId_e eCasGrpId, HUINT32 ulSlotNum, Handle_t hAction, HINT32 nMessage, HINT32 p1, HINT32 p2, void* pDATA, HUINT32 ulSize)
{
	BUS_Result_t					eRsult = ERR_BUS_INITIAL_FAILED;
//	HUINT32 					ulSlotNum = 0;
	OmCasMmiInstanceInfo_t	*pstInstInfo = NULL;

//	ulSlotNum = (HUINT32)p1;
	pstInstInfo = om_cas_mmi_GetSubCasInstInfo(eCasGrpId);
	if((pstInstInfo == NULL) || (pstInstInfo->fnCasUiProc == NULL) || (BUS_MGR_Get(pstInstInfo->fnCasUiProc) == NULL))
	{
		HxLOG_Error("not found inst 0x%x slot %d\n", eCasGrpId, ulSlotNum);

		goto END_FUNC;
	}

	// TODO: sendData 작성.
	eRsult = BUS_PostData(pstInstInfo->fnCasUiProc, nMessage, hAction, p1, p2, pDATA, ulSize);



END_FUNC:

	return eRsult;
}

STATIC OmCasMmiSessionInfo_t *om_cas_mmi_GetSessionInfoBySessionTypeAndSessionHandle(HxList_t *pstSessionList, DxCAS_MmiSessionType_e eSessionType, Handle_t hSession)
{
	HBOOL						bFound = FALSE;
	HxList_t					*pstSessionItem = NULL;
	OmCasMmiSessionInfo_t	*pstSessionInfo = NULL;
	HxList_t					*pstConnItem = NULL;
	OmCasMmiConnectionInfo_t 	*pstConnInfo = NULL;

	if(pstSessionList == NULL)
	{
		HxLOG_NullPrint("session list NULL\n");
		goto END_FUNC;
	}

	pstSessionItem = pstSessionList;
	while(pstSessionItem != NULL)
	{
		pstSessionInfo = (OmCasMmiSessionInfo_t *)HLIB_LIST_Data(pstSessionItem);
		if(pstSessionInfo != NULL)
		{

			pstConnItem = pstSessionInfo->pstConn;
			while(pstConnItem != NULL)
			{
				pstConnInfo = (OmCasMmiConnectionInfo_t *)HLIB_LIST_Data(pstConnItem);
				if(pstConnInfo != NULL)
				{
					HxLOG_Debug("pstConnInfo: 0x%x, pstConnInfo->hSession: 0x%x \n", pstConnInfo, pstConnInfo->hSession);
					if(pstConnInfo->eSessionType == eSessionType && pstConnInfo->hSession == hSession)
					{
						bFound = TRUE;
						break;
					}
				}

				pstConnItem = pstConnItem->next;
				pstConnInfo = NULL;
			}

			if(bFound == TRUE)
			{
				break;
			}
		}

		pstSessionItem = pstSessionItem->next;
		pstSessionInfo = NULL;
	}

END_FUNC:
	return pstSessionInfo;
}


STATIC OmCasMmiSessionInfo_t *om_cas_mmi_GetSessionInfoBySessionType(HxList_t *pstSessionList, DxCAS_MmiSessionType_e eSessionType)
{
	HBOOL						bFound = FALSE;
	HxList_t					*pstSessionItem = NULL;
	OmCasMmiSessionInfo_t	*pstSessionInfo = NULL;
	HxList_t					*pstConnItem = NULL;
	OmCasMmiConnectionInfo_t 	*pstConnInfo = NULL;

	if(pstSessionList == NULL)
	{
		HxLOG_NullPrint("session list NULL\n");
		goto END_FUNC;
	}

	pstSessionItem = pstSessionList;
	while(pstSessionItem != NULL)
	{
		pstSessionInfo = (OmCasMmiSessionInfo_t *)HLIB_LIST_Data(pstSessionItem);
		if(pstSessionInfo != NULL)
		{

			pstConnItem = pstSessionInfo->pstConn;
			while(pstConnItem != NULL)
			{
				pstConnInfo = (OmCasMmiConnectionInfo_t *)HLIB_LIST_Data(pstConnItem);
				if(pstConnInfo != NULL)
				{
					if(pstConnInfo->eSessionType == eSessionType)
					{
						bFound = TRUE;
						break;
					}
				}

				pstConnItem = pstConnItem->next;
				pstConnInfo = NULL;
			}

			if(bFound == TRUE)
			{
				break;
			}
		}

		pstSessionItem = pstSessionItem->next;
		pstSessionInfo = NULL;
	}

END_FUNC:
	return pstSessionInfo;
}


#define _____PROC_EVENT_HANDLER_____


STATIC HERROR om_cas_mmi_SessionProcEvent(HINT32 event, DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, void *pvSendData)
{
	HERROR							hError = ERR_OK;
	HUINT32							ulUiHandle = 0;
	Handle_t							hSession = HANDLE_NULL;
	OxMgrCasUiEvtSession_t			*pstSessionEvent = NULL;
	DxCAS_MmiSessionType_e				eSessionType = eDxCAS_SESSIONTYPE_NONE;
	DxCAS_MMI_SessionEventType_e				eMgrEventType = eDxCASMMI_EVENTTYPE_NONE;
	OmCasMmiInstanceInfo_t		*pstInst = NULL;
	OmCasMmiSessionInfo_t		*pstSessionInfo = NULL;
	HxList_t						*pstConnItem = NULL;
	OmCasMmiConnectionInfo_t 		*pstConnInfo = NULL;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error! OBAMA_RpcProtocol_GetRpcHandle() failed. \n");
	}

	if(pvSendData == NULL)
	{
		HxLOG_Error("data NULL\n");
		hError = ERR_FAIL;

		goto END_FUNC;
	}

	pstInst = om_cas_mmi_GetSubCasInstInfo(eCasGrpId);
	if(pstInst == NULL)
	{
		HxLOG_Error("instance NULL\n");
		hError = ERR_FAIL;

		goto END_FUNC;
	}

	pstSessionEvent = (OxMgrCasUiEvtSession_t *)pvSendData;
	ulUiHandle = (HUINT32)pstSessionEvent->ulUiHandle;
	eSessionType = pstSessionEvent->eSessionType;

	hSession = (Handle_t)pstSessionEvent->hSession;
	HxLOG_Debug("hSession: 0x%x \n", hSession);
	pstSessionInfo = om_cas_mmi_GetSessionInfoByTypeAndUiHandle(pstInst->pstSessionList, eSessionType, ulUiHandle);



	if(pstSessionInfo == NULL)
	{
		hError = om_cas_mmi_AppendSessionInfo(&(pstInst->pstSessionList), ulUiHandle, eSessionType, hSession, eOxCASMMI_SESSIONSTATUS_STOPPED, &pstSessionInfo);
		if((hError != ERR_OK) || (pstSessionInfo == NULL))
		{
			HxLOG_Error("\n");
			hError = ERR_FAIL;

			goto END_FUNC;
		}
	}

	pstConnItem = pstSessionInfo->pstConn;
	while(pstConnItem != NULL)
	{
		pstConnInfo = (OmCasMmiConnectionInfo_t *)HLIB_LIST_Data(pstConnItem);
		if(pstConnInfo != NULL)
		{
			if((pstConnInfo->nSlot == ulCasSlotNum) && (pstConnInfo->eCasGrpId == eCasGrpId)
			&& (pstConnInfo->hSession == hSession)// || pstConnInfo->ulUiHandle == ulUiHandle)
			)
			{
				break;
			}
		}

		pstConnItem = pstConnItem->next;
		pstConnInfo = NULL;
	}


	if(pstConnInfo == NULL && event != eMEVT_CASUI_REQ_OPEN_SESSION)
	{
		HxLOG_Info("pstConnInfo is NULL...\n");
		pstConnInfo = (OmCasMmiConnectionInfo_t *)HLIB_STD_MemCalloc(sizeof(OmCasMmiConnectionInfo_t));
		pstConnInfo->bStarted = FALSE;
		pstConnInfo->nSlot = ulCasSlotNum;
		pstConnInfo->eCasGrpId = eCasGrpId;
		pstConnInfo->eSessionType = eSessionType;
		pstConnInfo->hSession = hSession;
		HxLOG_Debug("pstConnInfo: 0x%x, hSession: 0x%x, ulUiHandle: 0x%x \n", pstConnInfo, hSession, ulUiHandle);
		pstSessionInfo->pstConn = HLIB_LIST_Append(pstSessionInfo->pstConn, pstConnInfo);
	}

	switch(event)
	{
		case eMEVT_CASUI_REQ_OPEN_SESSION:
			HxLOG_Info("EVT session req start 0x%x %d, eSessionType %d\n", eCasGrpId, ulCasSlotNum, eSessionType);
			eMgrEventType = eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTART;
			break;

		case eMEVT_CASUI_REQ_CLOSE_SESSION:
			HxLOG_Info("EVT session req stop\n");
			if(pstConnInfo->bStarted == TRUE)
			{
				if (nRpcHandle)
				{
				}
				pstConnInfo->bStarted = FALSE;
			}
			eMgrEventType = eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTOP;
			break;

		case eMEVT_CASUI_SESSION_OPENED:
			HxLOG_Info("EVT session started\n");
			eMgrEventType = eDxCASMMI_EVENTTYPE_SESSIONSTARTED;
			pstConnInfo->bStarted = TRUE;
			break;



		default:
			HxLOG_Info("invalid event type\n");
			hError = ERR_FAIL;
			goto END_FUNC;
			break;
	}
	HxLOG_Info("%s, nRpcHandle: %d\n", RPC_OBAMA_CAS_MMI_onSessionStatusChanged, nRpcHandle);

	if (nRpcHandle)
	{
		HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_CAS_MMI_onSessionStatusChanged, "iiiiii", hSession, ulUiHandle, pstInst->eCasGrpId, ulCasSlotNum, eMgrEventType, pstSessionInfo->eSessionType, NULL);
	}

END_FUNC:


	if(pvSendData != NULL)
	{
		HLIB_STD_MemFree(pvSendData);
	}

	return hError;
}


STATIC HERROR om_cas_mmi_UiDataProcEvent(HINT32 event, DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, void *pvPostData)
{
	HERROR									hError = ERR_OK;
	OxMgrCasUiType_e					eUiType = eMgrCasUiType_MAX;
	DxCAS_MmiSessionType_e					eSessionType = eDxCAS_SESSIONTYPE_NONE;
	HUINT32							ulUiHandle;
	OmCasMmiSessionInfo_t				*pstSessionInfo = NULL;
	OmCasMmiInstanceInfo_t				*pstInst = NULL;
	HxList_t							*pstConnItem = NULL;
	OmCasMmiConnectionInfo_t 			*pstConnInfo = NULL;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;
	OxMgrCasUiEvtSession_t			stSessionInfo = {0, };
	Handle_t							hSession;
	HCHAR							*pzProperty = NULL;
	OxMgrCasUIAttribute_e				ulAttr = 0;

	if(pvPostData == NULL)
	{
		HxLOG_Error("data NULL\n");
		hError = ERR_FAIL;

		goto END_FUNC;
	}

	hError = om_cas_mmi_GetUiType(event, &eUiType);

	HxLOG_Error("[%s][%d] event = %d, eSessionType = %d \n", __FUNCTION__, __LINE__, event, eSessionType);

	if(hError != ERR_OK)
	{
		HxLOG_Error("0x%x NOT found ui type\n", event);
		hError = ERR_FAIL;

		goto END_FUNC;
	}

	pstInst = om_cas_mmi_GetSubCasInstInfo(eCasGrpId);
	if(pstInst == NULL)
	{
		HxLOG_Error("NOT found instance 0x%x %d\n", eCasGrpId, ulCasSlotNum);
		hError = ERR_FAIL;

		goto END_FUNC;
	}
	HxSTD_MemCopy(&stSessionInfo, pvPostData, sizeof(OxMgrCasUiEvtSession_t));

	eSessionType = stSessionInfo.eSessionType;
	ulUiHandle = stSessionInfo.ulUiHandle;
	hSession = stSessionInfo.hSession;
	HxLOG_Info("hSession: 0x%x \n", hSession);
	pstSessionInfo = om_cas_mmi_GetSessionInfoByTypeAndUiHandle(pstInst->pstSessionList, eSessionType, ulUiHandle);


	HxLOG_Info("session type %d\n", eSessionType);

	if(pstSessionInfo == NULL)
	{
		HxLOG_Error("NOT found session handle, eSessionType: %d \n", eSessionType);
		hError = ERR_FAIL;

		goto END_FUNC;
	}

	//HxLOG_Info("\n");fflush(stdout);
	pstConnItem = pstSessionInfo->pstConn;
	while(pstConnItem != NULL)
	{
		pstConnInfo = (OmCasMmiConnectionInfo_t *)HLIB_LIST_Data(pstConnItem);
		if(pstConnInfo != NULL)
		{
			if(pstConnInfo->nSlot == ulCasSlotNum)
			{
				break;
			}
		}

		pstConnItem = pstConnItem->next;
		pstConnInfo = NULL;
	}

	if((pstConnInfo != NULL) && (pstConnInfo->bStarted == TRUE))
	{
		hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
		if (hErr == ERR_OK)
		{
			pzProperty = om_cas_mmi_EncodeUiDataProperty(eCasGrpId, ulCasSlotNum, eUiType, pvPostData, &ulAttr);

			HxLOG_Info("%s, nRpcHandle: %d\n", RPC_OBAMA_CAS_MMI_onGetEventData, nRpcHandle);

			hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

			if (hErr == ERR_OK)
			{
				//HxLOG_Info("strlen(pstEventInfo->pvUiData): %d, pstEventInfo->pvUiData: %s \n  ", strlen(pstEventInfo->pvUiData), pstEventInfo->pvUiData);
				HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_CAS_MMI_onGetEventData, "iiiiiib",
											hSession,
											eDxCASMMI_EVENTTYPE_DISPLAYUI,
											eCasGrpId,
											ulCasSlotNum,
											eSessionType,
											ulAttr,
											pzProperty, strlen(pzProperty));
			}
			if(pzProperty)
			{
				HLIB_STD_MemFree(pzProperty);
			}
		}
		else
		{
			HxLOG_Error("Can't Notify Msg - OBAMA_RpcProtocol_GetRpcHandle() error.\n");
		}
	}
	else
	{
		if(pstConnInfo == NULL)
		{
			HxLOG_Error("Can't Notify Msg - pstConnInfo is NULL\n");
		}
		else
		{
			HxLOG_Error("Can't Notify Msg - pstConnInfo(0x%x), pstConnInfo->bStarted = %d\n", pstConnInfo, pstConnInfo->bStarted);
		}
	}

END_FUNC:
	if(pvPostData != NULL)
	{
		HLIB_STD_MemFree(pvPostData);
	}

	return hError;
}

#define _____JSON_ENCODING_DECODING_FUNCTIONS_____

STATIC HINT32 om_cas_mmi_CreateStream(OxMgrCasUiType_e eUiType)
{
	HINT32			slStream;
	HUINT32			ulBufSize = 0;


	if(eUiType == eMgrCasUiType_MailMessage)
	{
		ulBufSize = OM_CASMMI_MAX_MAIL_SIZE;
	}
	else if(eUiType == eMgrCasUiType_MailMessageList)
	{
		ulBufSize = OM_CASMMI_MAX_MAIL_LIST_SIZE;
	}
	else
	{
		ulBufSize = OM_CASMMI_DEFAULT_SIZE;
	}

	slStream = HLIB_RWSTREAM_OpenEx(NULL, ulBufSize, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if(slStream == 0)
	{
		HxLOG_Error("open stream failed\n");
		HxLOG_Assert(FALSE);
	}

	return slStream;
}

STATIC HCHAR *om_cas_mmi_Closestream(HINT32 slStream)
{
	HCHAR *pzProperties = NULL;
//	HUINT32		ulStreamSize = 0;

//	ulStreamSize = HLIB_RWSTREAM_GetBufSize(slStream);
	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	return pzProperties;
}


STATIC HCHAR *om_cas_mmi_getCasTypeText(DxCAS_GroupId_e eCasGrpId)
{
	HCHAR *pscResult = NULL;

	switch(eCasGrpId)
	{
		case eDxCAS_GROUPID_CAM:			pscResult = "ci";						break;
		case eDxCAS_GROUPID_NA:				pscResult = "nagra";					break;
		case eDxCAS_GROUPID_VX:				pscResult = "vmx";						break;

		case eDxCAS_GROUPID_NONE:		pscResult = "unkown";					break;
		case eDxCAS_GROUPID_MG:			pscResult = "unkown";					break;
		case eDxCAS_GROUPID_VA:			pscResult = "unkown";					break;
		case eDxCAS_GROUPID_IR:			pscResult = "ir";						break;
		case eDxCAS_GROUPID_CX:			pscResult = "cx";					break;
		case eDxCAS_GROUPID_CR:			pscResult = "unkown";					break;
		case eDxCAS_GROUPID_ND:			pscResult = "unkown";					break;
		case eDxCAS_GROUPID_SATSA:		pscResult = "unkown";					break;
		case eDxCAS_GROUPID_COMMON: 	pscResult = "unkown";					break;
		default:					pscResult = "unkown";					break;
	}

	return pscResult;
}

STATIC HCHAR *om_cas_mmi_getInputTypeText(OxMgrCasInputType_e eInputType)
{
	HCHAR *pscResult = NULL;

	switch(eInputType)
	{
		case eMgrCasInputType_Button:				pscResult = "button";			break;
		case eMgrCasInputType_Number:				pscResult = "number";			break;
		case eMgrCasInputType_String:				pscResult = "string";			break;
		case eMgrCasInputType_Selection:			pscResult = "selection";		break;
		case eMgrCasInputType_Etc:					pscResult = "etc";				break;

		case eMgrCasInputType_MAX:
		default:
			pscResult = "unkowntype";
			break;
	}

	return pscResult;
}

STATIC HUINT32			om_cas_mmi_dec_value_to_enum(OmCasMmiTag2Enum_t *table, const HCHAR *value)
{
	while (table->tag)
	{
		if (HLIB_STD_StrCaseCmp(table->tag, value) == 0)
			return table->value;

		table++;
	}

	return table->value;
}


/**************************************************************
	private functions
 **************************************************************/

static void			om_cas_mmi_dec_value_to_enum_adv(_OmCasMmiTag2Enum_t *table, HxJSON_t object, HUINT32 write_addr, HUINT32 sizeofstructure)
{
	HUINT32		size = sizeofstructure;
	OmCasMmiTag2Enum_t	*taglist = table->taglist;

	if (size == 0)
		size = 4;
	switch (table->tag_type)
	{
		case	eTAG_Pair:
			{
				HCHAR	*value;

				value = (HCHAR *)HxJSON_String_Get(object);
				while (taglist->tag)
				{
					if (HLIB_STD_StrCaseCmp(taglist->tag, value) == 0)
					{
						HxSTD_MemCopy((HUINT32*)write_addr, &taglist->value, size);
						return;
					}
					taglist++;
				}
			}
			break;
		case	eTAG_Integer:
			{
				HUINT32		integer;

				integer = HxJSON_Integer_Get(object);
				HxSTD_MemCopy((HUINT32*)write_addr, &integer, size);
			}
			break;
		case	eTAG_Float:
			{
				float		value;

				value = HxJSON_Real_Get(object);
				HxSTD_MemCopy((float*)write_addr, &value, size);
			}
			break;
		case	eTAG_String:
			{
				HCHAR		*value;

				value = (HCHAR *)HxJSON_String_Get(object);
				HLIB_STD_StrNCpySafe((HCHAR*)write_addr, value, size);
			}
			break;
		case	eTAG_Bool:
			{
				HBOOL		value;

				value = HxJSON_Boolean_Get(object);
				HxSTD_MemCopy((HBOOL*)write_addr, &value, size);
			}
			break;
		case	eTAG_Binary:
			{
				HCHAR		*value;

				value = (HCHAR *)HxJSON_String_Get(object);
				if (value)
				{
					HLIB_MATH_DecodeBase64Ex((void *)write_addr, size, value, strlen(value));
				}
			}
			break;
	}
}


STATIC HERROR			om_cas_mmi_dec_parse_property(void *pInfo, HxJSON_t root, _OmCasMmiProp2Enum_t *table)
{
	HxJSON_t	object;

	HUINT32		write_addr, base_addr = (HUINT32)pInfo;

	while (table->property)
	{
		write_addr = (base_addr + table->offset);

		object = HxJSON_Object_Get(root, table->property);
		if (object)
		{
			if (HxJSON_IsArray(object) == FALSE)
			{
				if (table->enumtable)
				{
					om_cas_mmi_dec_value_to_enum_adv(table->enumtable, object, write_addr, table->sizeofstructure);
				} else
				if (table->propTable)
				{
					om_cas_mmi_dec_parse_property((void*)write_addr, object, table->propTable);
				}
			} else
			{
				HINT32		i, array_count;
				HxJSON_t	array_object;
				HUINT32		array_addr, str_size;

				if (table->propTable)
					str_size = table->sizeofstructure;
				else
					str_size = 4;

				array_count = HLIB_JSON_GetSize(object);
				array_count = MIN(array_count, table->ObjectCount);
				for (i = 0; i < array_count; i++)
				{
					array_addr = write_addr + (i * str_size);
					array_object = HxJSON_Array_Get(object, i);
					om_cas_mmi_dec_parse_property((void*)array_addr, array_object, table->propTable);
				}
			}
		}

		table++;
	}

	return ERR_OK;
}


STATIC void om_cas_mmi_DecodeUserInputData(OxMgrCasInputType_e eInputType, HxJSON_t json_inputData, OxMgrCasUiInputNotify_t *putData)
{
	HERROR		hError = ERR_OK;
	_OmCasMmiProp2Enum_t *pstPropertyTable = NULL;

	switch(eInputType)
	{
		case eMgrCasInputType_Button:
			pstPropertyTable = _casui_app_inputdata_button_propertyTable;
			break;

		case eMgrCasInputType_Number:
			pstPropertyTable = _casui_app_inputdata_number_propertyTable;
			break;

		case eMgrCasInputType_String:
			pstPropertyTable = _casui_app_inputdata_string_propertyTable;
			break;

		case eMgrCasInputType_Selection:
			pstPropertyTable = _casui_app_inputdata_selection_propertyTable;
			break;

		case eMgrCasInputType_Etc:
			pstPropertyTable = _casui_app_inputdata_etc_propertyTable;
			break;

		case eMgrCasInputType_MAX:
		default:
			pstPropertyTable = NULL;
			hError = ERR_FAIL;
			break;
	}

	if(hError == ERR_OK)
	{
		if(pstPropertyTable != NULL)
		{
			om_cas_mmi_dec_parse_property(putData, json_inputData, pstPropertyTable);
		}
	}

	return;
}




STATIC HUINT32 om_cas_mmi_EncodeAttribute(HINT32 slStream, OxMgrCasUIAttribute_e eAttribute)
{
	HUINT32					ulSearchBit = 0x80000000;
	HUINT32					ulAttribute = 0;
	HUINT32					ulAttributeCnt = 0;
	HCHAR					*pszAttributeName = NULL;

	ulAttribute = (HUINT32)eAttribute;
	while(ulSearchBit != 0)
	{
		pszAttributeName = NULL;
		switch(ulSearchBit&ulAttribute)
		{
			case eMgrCasUiAttr_Flashing: pszAttributeName = "flashing"; break;
			case eMgrCasUiAttr_Scrollable: pszAttributeName = "scrollable"; break;
			case eMgrCasUiAttr_Timeout: pszAttributeName = "timeout"; break;
			case eMgrCasUiAttr_SetPosition: pszAttributeName = "setposition"; break;
			case eMgrCasUiAttr_RandomPosition: pszAttributeName = "randomposition"; break;
			case eMgrCasUiAttr_SetSize: pszAttributeName = "setsize"; break;
			case eMgrCasUiAttr_SetFgColor: pszAttributeName = "setfgcolor"; break;
			case eMgrCasUiAttr_SetBgColor: pszAttributeName = "setbgcolor"; break;
			case eMgrCasUiAttr_Title: pszAttributeName = "title"; break;
			case eMgrCasUiAttr_Subtitle: pszAttributeName = "subtitle"; break;
			case eMgrCasUiAttr_Bottom: pszAttributeName = "bottom"; break;
			case eMgrCasUiAttr_Multiline: pszAttributeName = "multiline"; break;
			case eMgrCasUiAttr_HiddenInput: pszAttributeName = "hiddeninput"; break;
			case eMgrCasUiAttr_ProgressBar: pszAttributeName = "progressbar"; break;
			case eMgrCasUiAttr_InitValue: pszAttributeName = "initvalue"; break;
			case eMgrCasUiAttr_Selectable: pszAttributeName = "selectable"; break;
			case eMgrCasUiAttr_IgnoreKey: pszAttributeName = "ignorekey"; break;
			case eMgrCasUiAttr_AlwaysOnTop: pszAttributeName = "alwaysontop"; break;
			case eMgrCasUiAttr_Highlight: pszAttributeName = "highlight"; break;
			case eMgrCasUiAttr_FingerPrint: pszAttributeName = "fingerprint"; break;
			case eMgrCasUiAttr_SetPrintTime: pszAttributeName = "setprinttime"; break;
			case eMgrCasUiAttr_Tenable: pszAttributeName = "tenable"; break;
			default:
				break;
		}

		if(pszAttributeName != NULL)
		{
			if(ulAttributeCnt == 0)
			{
				HLIB_RWSTREAM_Print(slStream, "\"attribute\":[");
				HLIB_RWSTREAM_Print(slStream, "\"%s\"", pszAttributeName);
			}
			else
			{
				HLIB_RWSTREAM_Print(slStream, ",\"%s\"", pszAttributeName);
			}
			ulAttributeCnt++;
		}
		ulSearchBit = ulSearchBit >> 1;
	}

	if(ulAttributeCnt != 0)
	{
		HLIB_RWSTREAM_Print(slStream, "],");
	}

	return ulAttributeCnt;
}

STATIC HUINT32 om_cas_mmi_EncodeListItemAttribute(HINT32 slStream, OxMgrCasListItemAttribute_e eAttribute)
{
	HUINT32					ulSearchBit = 0x80000000;
	HUINT32					ulAttribute = 0;
	HUINT32					ulAttributeCnt = 0;
	HCHAR					*pszAttributeName = NULL;

	ulAttribute = (HUINT32)eAttribute;
	while(ulSearchBit != 0)
	{
		pszAttributeName = NULL;
		switch(ulSearchBit&ulAttribute)
		{
			case eMgrCasUiAttr_Disabled: pszAttributeName = "disabled"; break;
			case eMgrCasUiAttr_Hide: pszAttributeName = "hide"; break;
			case eMgrCasUiAttr_Selected: pszAttributeName = "selected"; break;
			default:
				break;
		}

		if(pszAttributeName != NULL)
		{
			if(ulAttributeCnt == 0)
			{
				HLIB_RWSTREAM_Print(slStream, ",\"attribute\":[");
				HLIB_RWSTREAM_Print(slStream, "\"%s\"", pszAttributeName);
			}
			else
			{
				HLIB_RWSTREAM_Print(slStream, ",\"%s\"", pszAttributeName);
			}
			ulAttributeCnt++;
		}
		ulSearchBit = ulSearchBit >> 1;
	}

	if(ulAttributeCnt != 0)
	{
		HLIB_RWSTREAM_Print(slStream, "]");
	}

	return ulAttributeCnt;
}

STATIC HUINT32 om_cas_mmi_EncodeInputButtonList(HINT32 slStream,
	OxMgrCasInputButtonType_e eButtonType, HCHAR (*ppButtonName)[MGR_CAS_STR_LENGTH_SHORT])
{
	HUINT32					ulSearchBit = 0x80000000;
	HUINT32					ulAttribute = 0;
	HUINT32					ulAttributeCnt = 0;
	HCHAR					*pszAttributeName = NULL;

	ulAttribute = (HUINT32)eButtonType;
	while(ulSearchBit != 0)
	{
		pszAttributeName = NULL;
		switch(ulSearchBit&ulAttribute)
		{
			case eMgrCasBtn_OK:					pszAttributeName = "ok";			break;
			case eMgrCasBtn_YES:				pszAttributeName = "yes";			break;
			case eMgrCasBtn_NO:					pszAttributeName = "no";			break;
			case eMgrCasBtn_CANCEL:				pszAttributeName = "cancel";		break;
			case eMgrCasBtn_1st:				pszAttributeName = "1st";			break;
			case eMgrCasBtn_2nd:				pszAttributeName = "2nd";			break;
			case eMgrCasBtn_3rd:				pszAttributeName = "3rd";			break;
			default:
				pszAttributeName = NULL;
				break;
		}

		if((pszAttributeName != NULL) && (ppButtonName[ulAttributeCnt] != NULL))
		{
			if(ulAttributeCnt == 0)
			{
				HLIB_RWSTREAM_Print(slStream, "\"attribute\":[");
				HLIB_RWSTREAM_Print(slStream, "{\"%s\":%s}", pszAttributeName, ppButtonName[ulAttributeCnt]);
			}
			else
			{
				HLIB_RWSTREAM_Print(slStream, ",{\"%s\":%s}", pszAttributeName, ppButtonName[ulAttributeCnt]);
			}
			ulAttributeCnt++;
		}
		ulSearchBit = ulSearchBit >> 1;
	}

	if(ulAttributeCnt != 0)
	{
		HLIB_RWSTREAM_Print(slStream, "]");
	}

	return ulAttributeCnt;
}


STATIC HCHAR *om_cas_mmi_EncodeUiDataClose(DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum)
{
	HCHAR					*pzProperties = NULL;
	HINT32					slStream = HLIB_RWSTREAM_OpenEx(NULL, OM_CASMMI_DEFAULT_SIZE, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);

	HLIB_RWSTREAM_Print(slStream, "{\"uitype\":\"undefine\"");
	HLIB_RWSTREAM_Print(slStream, ",\"uidata\":{\"string\":\"\"}");
	HLIB_RWSTREAM_Print(slStream, ",\"castype\":\"%s\"", om_cas_mmi_getCasTypeText(eCasGrpId));
	HLIB_RWSTREAM_Print(slStream, ",\"slotnumber\":%d}", ulCasSlotNum);

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	return pzProperties;
}

/*******************************************************************
"uitype":"banner"
,"uidata":{
	"attribute":
		[
			"flashing","scrollable","timeout","setposition","randomposition","setsize","setfgcolor",
			"setbgcolor","title","subtitle","bottom","multiline","hiddeninput","progressbar","initvalue",
			"selectable","ignorekey","alwaysontop"
		],
	"timeout":0,
	"stringlength":25,
	"string":"please insert card.."
}
*******************************************************************/
STATIC HCHAR *om_cas_mmi_EncodeUiDataBanner(DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, void *pvUiData, OxMgrCasUIAttribute_e *pulAttr)
{
	OxMgrCasUiEvtBanner_t	*pstBannerEvt = (OxMgrCasUiEvtBanner_t *)pvUiData;
	HCHAR					*pzProperties = NULL;
	HINT32					slStream = HLIB_RWSTREAM_OpenEx(NULL, OM_CASMMI_DEFAULT_SIZE, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);

	HLIB_RWSTREAM_Print(slStream, "{\"uitype\":\"banner\"");
	HLIB_RWSTREAM_Print(slStream, ",\"uidata\":{");

	om_cas_mmi_EncodeAttribute(slStream, pstBannerEvt->stBanner.ulAttr);
	HLIB_RWSTREAM_Print(slStream, "\"timeout\":%d", pstBannerEvt->stBanner.ulTimeoutSecond);
	if(pstBannerEvt->stBanner.stString.ulStringLength == 0){
		HLIB_RWSTREAM_Print(slStream, ",\"string\":\"\"");
	} else {
		HLIB_RWSTREAM_Print(slStream, ",\"string\":%s", pstBannerEvt->stBanner.stString.szString);
	}
	HLIB_RWSTREAM_Print(slStream, "}");
	HLIB_RWSTREAM_Print(slStream, ",\"castype\":\"%s\"", om_cas_mmi_getCasTypeText(eCasGrpId));
	HLIB_RWSTREAM_Print(slStream, ",\"slotnumber\":%d}", ulCasSlotNum);

	*pulAttr = pstBannerEvt->stBanner.ulAttr;

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	return pzProperties;
}

/*******************************************************************
"uitype":"textbox"
,"uidata":{
	"attribute":
		[
			"flashing","scrollable","timeout","setposition","randomposition","setsize","setfgcolor",
			"setbgcolor","title","subtitle","bottom","multiline","hiddeninput","progressbar","initvalue",
			"selectable","ignorekey","alwaysontop"
		],
	"stringlength":25,
	"string":"please insert card.."
}
*******************************************************************/
STATIC HCHAR *om_cas_mmi_EncodeUiDataTextBox(DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, void *pvUiData, OxMgrCasUIAttribute_e *pulAttr)
{
	OxMgrCasUiEvtTextbox_t	*pstTextBoxEvt = (OxMgrCasUiEvtTextbox_t *)pvUiData;
	HCHAR					*pzProperties = NULL;
	HINT32					slStream = HLIB_RWSTREAM_OpenEx(NULL, OM_CASMMI_DEFAULT_SIZE, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	OxMgrCasUIAttribute_e	ulAttr = 0;
	HUINT32					ulAttrCnt = 0;
	HUINT32					ulCnt = 0;

	ulAttr = pstTextBoxEvt->stTextbox.stHeader.ulAttr;

	HLIB_RWSTREAM_Print(slStream, "{\"uitype\":\"textbox\"");
	HLIB_RWSTREAM_Print(slStream, ",\"uidata\":{");

	/* header OxMgrCasUiMenuHeader_t */
	ulAttrCnt = om_cas_mmi_EncodeAttribute(slStream, ulAttr);
	if(ulAttr&eMgrCasUiAttr_Title)
	{
		HLIB_RWSTREAM_Print(slStream, "\"title\":%s", pstTextBoxEvt->stTextbox.stHeader.szTitle);
		ulCnt++;
	}
	if(ulAttr&eMgrCasUiAttr_Subtitle)
	{
		if(ulCnt != 0)
		{
			HLIB_RWSTREAM_Print(slStream, ",");
		}
		HLIB_RWSTREAM_Print(slStream, "\"subtitle\":%s",pstTextBoxEvt->stTextbox.stHeader.szSubTitle);
		ulCnt++;
	}
	if(ulAttr&eMgrCasUiAttr_Bottom)
	{
		if(ulCnt != 0)
		{
			HLIB_RWSTREAM_Print(slStream, ",");
		}
		HLIB_RWSTREAM_Print(slStream, "\"bottom\":%s", pstTextBoxEvt->stTextbox.stHeader.szBottom);
		ulCnt++;
	}
	if(ulAttr&eMgrCasUiAttr_SetPosition)
	{
		if(ulCnt != 0)
		{
			HLIB_RWSTREAM_Print(slStream, ",");
		}
		switch(pstTextBoxEvt->stTextbox.stHeader.ulPosition)
		{
			case eMgrCasUiPosition_Center:
				HLIB_RWSTREAM_Print(slStream, "\"position\":\"center\"");
				break;
			case eMgrCasUiPosition_Top:
				HLIB_RWSTREAM_Print(slStream, "\"position\":\"top\"");
				break;
			case eMgrCasUiPosition_Bottom:
				HLIB_RWSTREAM_Print(slStream, "\"position\":\"bottom\"");
				break;
			case eMgrCasUiPosition_Coordinate:
				HLIB_RWSTREAM_Print(slStream, "\"position\":\"coordinate\",\"posx\":%d,\"posy\":%d",pstTextBoxEvt->stTextbox.stHeader.ulPosX,pstTextBoxEvt->stTextbox.stHeader.ulPosY);
				break;
			default:
				HxLOG_Error("ERROR...\n");
				break;
		}
		ulCnt++;
	}
	if(ulAttr&eMgrCasUiAttr_Timeout)
	{
		if(ulCnt != 0)
		{
			HLIB_RWSTREAM_Print(slStream, ",");
		}
		HLIB_RWSTREAM_Print(slStream, "\"timeout\":%d", pstTextBoxEvt->stTextbox.stHeader.ulTimeoutMilliSecond);
		ulCnt++;
	}
	if(ulAttr&eMgrCasUiAttr_SetSize)
	{
		if(ulCnt != 0)
		{
			HLIB_RWSTREAM_Print(slStream, ",");
		}
		HLIB_RWSTREAM_Print(slStream, "\"height\":%d,\"width\":%d", pstTextBoxEvt->stTextbox.stHeader.ulHeight, pstTextBoxEvt->stTextbox.stHeader.ulWidth);
		ulCnt++;
	}
	if(ulAttr&eMgrCasUiAttr_SetPrintTime)
	{
		if(ulCnt != 0)
		{
			HLIB_RWSTREAM_Print(slStream, ",");
		}
		HLIB_RWSTREAM_Print(slStream, "\"printtime\":%d", pstTextBoxEvt->stTextbox.stHeader.ulPrintTime);
		ulCnt++;
	}
	if(ulAttr&eMgrCasUiAttr_Highlight)
	{
		//추가로 인자를 넣어야 할 사항 없음.
		ulCnt++;
	}
	if(ulAttr&eMgrCasUiAttr_FingerPrint)
	{
		//추가로 인자를 넣어야 할 사항 없음.
		ulCnt++;
	}
	if(ulAttr&eMgrCasUiAttr_Tenable)
	{
		//추가로 인자를 넣어야 할 사항 없음.
		ulCnt++;
	}

	if(ulCnt != ulAttrCnt)
	{
		HxLOG_Error("ERROR...\n");
	}

	if (ulAttrCnt == 0)
	{
		HLIB_RWSTREAM_Print(slStream, "\"attribute\":[]");
	}

	HLIB_RWSTREAM_Print(slStream, ",\"stringlength\":%d", pstTextBoxEvt->stTextbox.ulStringLength);
	HLIB_RWSTREAM_Print(slStream, ",\"string\":%s", pstTextBoxEvt->stTextbox.pszString);
	HLIB_RWSTREAM_Print(slStream, "}");
	HLIB_RWSTREAM_Print(slStream, ",\"castype\":\"%s\"", om_cas_mmi_getCasTypeText(eCasGrpId));
	HLIB_RWSTREAM_Print(slStream, ",\"slotnumber\":%d}", ulCasSlotNum);

	*pulAttr = ulAttr;

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	return pzProperties;
}

/*******************************************************************
"uitype":"mailmessage"
,"uidata":{
	"attribute":
		[
			"flashing","scrollable","timeout","setposition","randomposition","setsize","setfgcolor",
			"setbgcolor","title","subtitle","bottom","multiline","hiddeninput","progressbar","initvalue",
			"selectable","ignorekey","alwaysontop"
		],
	"timeoutsecond":10,
	"fontsize":25,
	"posx":100,
	"posy":100,
	"width":200,
	"height":200,
	"bgcolor":255,
	"fgcolor":65280,
	"progress":11,
	"title":"-OP Mail-",
	"subtitle":"Agent",
	"bottom":"Back button",
	"textlength":16,
	"text":"hello"
}
*******************************************************************/
STATIC HCHAR *om_cas_mmi_EncodeUiDataMailMessage(DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, void *pvUiData, OxMgrCasUIAttribute_e *pulAttr)
{
	HCHAR					*pzProperties = NULL;
	HINT32					slStream = HLIB_RWSTREAM_OpenEx(NULL, OM_CASMMI_MAX_MAIL_SIZE, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	OxMgrCasUiEvtMail_t		*pstMailMessageEvt = (OxMgrCasUiEvtMail_t *)pvUiData;

	HLIB_RWSTREAM_Print(slStream, "{\"uitype\":\"mailmessage\"");
	HLIB_RWSTREAM_Print(slStream, ",\"uidata\":{");

	/* header */

	om_cas_mmi_EncodeAttribute(slStream, pstMailMessageEvt->stMail.stHeader.ulAttr);


	HLIB_RWSTREAM_Print(slStream, "\"mailindex\":%d", pstMailMessageEvt->stMail.stHeader.nMailIdx);
	if(HxSTD_StrLen(pstMailMessageEvt->stMail.stHeader.strReceivedDate) == 0)
	{
		HLIB_RWSTREAM_Print(slStream, ",\"receiveddate\":\"\"", pstMailMessageEvt->stMail.stHeader.strReceivedDate);
	}
	else
	{
		HLIB_RWSTREAM_Print(slStream, ",\"receiveddate\":%s", pstMailMessageEvt->stMail.stHeader.strReceivedDate);
	}
	if(HxSTD_StrLen(pstMailMessageEvt->stMail.stHeader.strReceivedTime) == 0)
	{
		HLIB_RWSTREAM_Print(slStream, ",\"receivedtime\":\"\"", pstMailMessageEvt->stMail.stHeader.strReceivedTime);
	}
	else
	{
		HLIB_RWSTREAM_Print(slStream, ",\"receivedtime\":%s", pstMailMessageEvt->stMail.stHeader.strReceivedTime);
	}
	HLIB_RWSTREAM_Print(slStream, ",\"mailtype\":%d", pstMailMessageEvt->stMail.stHeader.nMailType);

	HLIB_RWSTREAM_Print(slStream, ",\"timeoutsecond\":%d", pstMailMessageEvt->stMail.stHeader.ulTimeoutSecond);
	HLIB_RWSTREAM_Print(slStream, ",\"fontsize\":%d", pstMailMessageEvt->stMail.stHeader.ulFontSize);
	HLIB_RWSTREAM_Print(slStream, ",\"coverage\":%d", pstMailMessageEvt->stMail.stHeader.ulCoverage);
	HLIB_RWSTREAM_Print(slStream, ",\"posx\":%d", pstMailMessageEvt->stMail.stHeader.ulPosX);
	HLIB_RWSTREAM_Print(slStream, ",\"posy\":%d", pstMailMessageEvt->stMail.stHeader.ulPosY);
	HLIB_RWSTREAM_Print(slStream, ",\"width\":%d", pstMailMessageEvt->stMail.stHeader.ulWidth);
	HLIB_RWSTREAM_Print(slStream, ",\"height\":%d", pstMailMessageEvt->stMail.stHeader.ulHeight);
	HLIB_RWSTREAM_Print(slStream, ",\"bgcolor\":%d", pstMailMessageEvt->stMail.stHeader.ulBgColor);
	HLIB_RWSTREAM_Print(slStream, ",\"fgcolor\":%d", pstMailMessageEvt->stMail.stHeader.ulFgColor);
	HLIB_RWSTREAM_Print(slStream, ",\"progress\":%d", pstMailMessageEvt->stMail.stHeader.ulProgress);

	HLIB_RWSTREAM_Print(slStream, ",\"read\":%s", pstMailMessageEvt->stMail.stHeader.bRead ? "true":"false");
	HLIB_RWSTREAM_Print(slStream, ",\"fp\":%s", pstMailMessageEvt->stMail.stHeader.bIsFp ? "true":"false");
	HLIB_RWSTREAM_Print(slStream, ",\"attributed\":%s", pstMailMessageEvt->stMail.stHeader.bIsAttributed? "true":"false");
	HLIB_RWSTREAM_Print(slStream, ",\"fptype\":%d", pstMailMessageEvt->stMail.stHeader.ulFpType);
	HLIB_RWSTREAM_Print(slStream, ",\"icon\":%s", pstMailMessageEvt->stMail.stHeader.bUseIcon ? "true":"false");
	HLIB_RWSTREAM_Print(slStream, ",\"icontype\":%d", pstMailMessageEvt->stMail.stHeader.ulMailIconType);

	if(HxSTD_StrLen(pstMailMessageEvt->stMail.stHeader.szTitle) == 0)
	{
		HLIB_RWSTREAM_Print(slStream, ",\"title\":\"\"", pstMailMessageEvt->stMail.stHeader.szTitle);
	}
	else
	{
		HLIB_RWSTREAM_Print(slStream, ",\"title\":%s", pstMailMessageEvt->stMail.stHeader.szTitle);
	}
	if(HxSTD_StrLen(pstMailMessageEvt->stMail.stHeader.szSubTitle) == 0)
	{
		HLIB_RWSTREAM_Print(slStream, ",\"subtitle\":\"\"", pstMailMessageEvt->stMail.stHeader.szSubTitle);
	}
	else
	{
		HLIB_RWSTREAM_Print(slStream, ",\"subtitle\":%s", pstMailMessageEvt->stMail.stHeader.szSubTitle);
	}
	if(HxSTD_StrLen(pstMailMessageEvt->stMail.stHeader.szBottom) == 0)
	{
		HLIB_RWSTREAM_Print(slStream, ",\"bottom\":\"\"", pstMailMessageEvt->stMail.stHeader.szBottom);
	}
	else
	{
		HLIB_RWSTREAM_Print(slStream, ",\"bottom\":%s", pstMailMessageEvt->stMail.stHeader.szBottom);
	}

	HLIB_RWSTREAM_Print(slStream, ",\"textlength\":%d", pstMailMessageEvt->stMail.ulTextLength);
	if(pstMailMessageEvt->stMail.ulTextLength == 0)
	{
		HLIB_RWSTREAM_Print(slStream, ",\"text\":\"\"");
	}
	else
	{
		HLIB_RWSTREAM_Print(slStream, ",\"text\":%s", pstMailMessageEvt->stMail.pszText);
	}

	HLIB_RWSTREAM_Print(slStream, "}");

	HLIB_RWSTREAM_Print(slStream, ",\"castype\":\"%s\"", om_cas_mmi_getCasTypeText(eCasGrpId));
	HLIB_RWSTREAM_Print(slStream, ",\"slotnumber\":%d}", ulCasSlotNum);

	*pulAttr = pstMailMessageEvt->stMail.stHeader.ulAttr;

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	return pzProperties;
}

STATIC HCHAR *om_cas_mmi_EncodeUiDataMailMessageList(DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, void *pvUiData, OxMgrCasUIAttribute_e *pulAttr)
{
	HCHAR					*pzProperties = NULL;
	HINT32					slStream = HLIB_RWSTREAM_OpenEx(NULL, OM_CASMMI_MAX_MAIL_LIST_SIZE, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	OxMgrCasUiEvtMailList_t		*pstMailMessageListEvt = (OxMgrCasUiEvtMailList_t *)pvUiData;
	OxMgrCasUiMail_t			*pstMailMessage = NULL;
	HUINT32					ulMailIdx = 0;
	HUINT32					ulNumOfMail = 0;
	OxMgrCasUIAttribute_e		ulAttr = 0;



	HLIB_RWSTREAM_Print(slStream, "{\"uitype\":\"mailmessagelist\"");
	HLIB_RWSTREAM_Print(slStream, ",\"uidata\":{");


	ulAttr = pstMailMessageListEvt->stMailList.stHeader.ulAttr;

	/* header OxMgrCasUiMenuHeader_t */
	om_cas_mmi_EncodeAttribute(slStream, ulAttr);
	if(ulAttr&eMgrCasUiAttr_Title)
	{
		HLIB_RWSTREAM_Print(slStream, "\"title\":%s", pstMailMessageListEvt->stMailList.stHeader.szTitle);
	}

	if(ulAttr&eMgrCasUiAttr_Subtitle)
	{
		if(!(ulAttr&eMgrCasUiAttr_Title))
		{
			HLIB_RWSTREAM_Print(slStream, "\"subtitle\":%s", pstMailMessageListEvt->stMailList.stHeader.szSubTitle);
		}
		else
		{
			HLIB_RWSTREAM_Print(slStream, ",\"subtitle\":%s",pstMailMessageListEvt->stMailList.stHeader.szSubTitle);
		}
	}

	if(ulAttr&eMgrCasUiAttr_Bottom)
	{
		if(!(ulAttr&eMgrCasUiAttr_Title) && !(ulAttr&eMgrCasUiAttr_Subtitle))
		{
			HLIB_RWSTREAM_Print(slStream, "\"bottom\":%s", pstMailMessageListEvt->stMailList.stHeader.szBottom);
		}
		else
		{
			HLIB_RWSTREAM_Print(slStream, ",\"bottom\":%s", pstMailMessageListEvt->stMailList.stHeader.szBottom);
		}
	}



	ulNumOfMail = pstMailMessageListEvt->stMailList.ulTotalNum;




	if(ulNumOfMail != 0)
	{
		HxLOG_Info("ulNumOfMail %d\n", ulNumOfMail);

		if ((ulAttr&eMgrCasUiAttr_Title) || (ulAttr&eMgrCasUiAttr_Subtitle) || (ulAttr&eMgrCasUiAttr_Bottom))
		{
			HLIB_RWSTREAM_Print(slStream, ",\"listitem\":[");
		}
		else
		{
			HLIB_RWSTREAM_Print(slStream, "\"attribute\":[],\"listitem\":[");
		}


		do
		{
			HxLOG_Info("ulMailIdx %d\n", ulMailIdx);

			pstMailMessage = pstMailMessageListEvt->stMailList.astListItem + ulMailIdx;

			HLIB_RWSTREAM_Print(slStream, "{");

			om_cas_mmi_EncodeAttribute(slStream, pstMailMessage->stHeader.ulAttr);

			HLIB_RWSTREAM_Print(slStream, "\"mailindex\":%d", pstMailMessage->stHeader.nMailIdx);

			if(HxSTD_StrLen(pstMailMessage->stHeader.strReceivedDate) == 0)
			{
				HLIB_RWSTREAM_Print(slStream, ",\"receiveddate\":\"\"", pstMailMessage->stHeader.strReceivedDate);
			}
			else
			{
				HLIB_RWSTREAM_Print(slStream, ",\"receiveddate\":%s", pstMailMessage->stHeader.strReceivedDate);
			}
			if(HxSTD_StrLen(pstMailMessage->stHeader.strReceivedTime) == 0)
			{
				HLIB_RWSTREAM_Print(slStream, ",\"receivedtime\":\"\"", pstMailMessage->stHeader.strReceivedTime);
			}
			else
			{
				HLIB_RWSTREAM_Print(slStream, ",\"receivedtime\":%s", pstMailMessage->stHeader.strReceivedTime);
			}
			HLIB_RWSTREAM_Print(slStream, ",\"mailtype\":%d", pstMailMessage->stHeader.nMailType);

			HLIB_RWSTREAM_Print(slStream, ",\"timeoutsecond\":%d", pstMailMessage->stHeader.ulTimeoutSecond);
			HLIB_RWSTREAM_Print(slStream, ",\"fontsize\":%d", pstMailMessage->stHeader.ulFontSize);
			HLIB_RWSTREAM_Print(slStream, ",\"coverage\":%d", pstMailMessage->stHeader.ulCoverage);
			HLIB_RWSTREAM_Print(slStream, ",\"posx\":%d", pstMailMessage->stHeader.ulPosX);
			HLIB_RWSTREAM_Print(slStream, ",\"posy\":%d", pstMailMessage->stHeader.ulPosY);
			HLIB_RWSTREAM_Print(slStream, ",\"width\":%d", pstMailMessage->stHeader.ulWidth);
			HLIB_RWSTREAM_Print(slStream, ",\"height\":%d", pstMailMessage->stHeader.ulHeight);
			HLIB_RWSTREAM_Print(slStream, ",\"bgcolor\":%d", pstMailMessage->stHeader.ulBgColor);
			HLIB_RWSTREAM_Print(slStream, ",\"fgcolor\":%d", pstMailMessage->stHeader.ulFgColor);
			HLIB_RWSTREAM_Print(slStream, ",\"progress\":%d", pstMailMessage->stHeader.ulProgress);

			HLIB_RWSTREAM_Print(slStream, ",\"read\":%s", pstMailMessage->stHeader.bRead ? "true":"false");
			HLIB_RWSTREAM_Print(slStream, ",\"fp\":%s", pstMailMessage->stHeader.bIsFp ? "true":"false");
			HLIB_RWSTREAM_Print(slStream, ",\"attributed\":%s", pstMailMessage->stHeader.bIsAttributed? "true":"false");
			HLIB_RWSTREAM_Print(slStream, ",\"fptype\":%d", pstMailMessage->stHeader.ulFpType);
			HLIB_RWSTREAM_Print(slStream, ",\"icon\":%s", pstMailMessage->stHeader.bUseIcon ? "true":"false");
			HLIB_RWSTREAM_Print(slStream, ",\"icontype\":%d", pstMailMessage->stHeader.ulMailIconType);

			if(HxSTD_StrLen(pstMailMessage->stHeader.szTitle) == 0)
			{
				HLIB_RWSTREAM_Print(slStream, ",\"title\":\"\"", pstMailMessage->stHeader.szTitle);
			}
			else
			{
				HLIB_RWSTREAM_Print(slStream, ",\"title\":%s", pstMailMessage->stHeader.szTitle);
			}
			if(HxSTD_StrLen(pstMailMessage->stHeader.szSubTitle) == 0)
			{
				HLIB_RWSTREAM_Print(slStream, ",\"subtitle\":\"\"", pstMailMessage->stHeader.szSubTitle);
			}
			else
			{
				HLIB_RWSTREAM_Print(slStream, ",\"subtitle\":%s", pstMailMessage->stHeader.szSubTitle);
			}
			if(HxSTD_StrLen(pstMailMessage->stHeader.szBottom) == 0)
			{
				HLIB_RWSTREAM_Print(slStream, ",\"bottom\":\"\"", pstMailMessage->stHeader.szBottom);
			}
			else
			{
				HLIB_RWSTREAM_Print(slStream, ",\"bottom\":%s", pstMailMessage->stHeader.szBottom);
			}

			HLIB_RWSTREAM_Print(slStream, ",\"textlength\":%d", pstMailMessage->ulTextLength);
			if(pstMailMessage->ulTextLength == 0)
			{
				HLIB_RWSTREAM_Print(slStream, ",\"text\":\"\"");
			}
			else
			{
				HLIB_RWSTREAM_Print(slStream, ",\"text\":%s", pstMailMessage->pszText);
			}


			ulMailIdx = ulMailIdx + 1;
			if(ulMailIdx == ulNumOfMail)
			{
				HLIB_RWSTREAM_Print(slStream, "}");
				break;
			}

			HLIB_RWSTREAM_Print(slStream, "},");
		}while(1);
		HLIB_RWSTREAM_Print(slStream, "]");
	}

	HLIB_RWSTREAM_Print(slStream, "}");
	HLIB_RWSTREAM_Print(slStream, ",\"castype\":\"%s\"", om_cas_mmi_getCasTypeText(eCasGrpId));
	HLIB_RWSTREAM_Print(slStream, ",\"slotnumber\":%d}", ulCasSlotNum);

	*pulAttr = ulAttr;

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	return pzProperties;
}


/*******************************************************************
"uitype":"menulist"
,"uidata":{
	"attribute":
		[
			"flashing","scrollable","timeout","setposition","randomposition","setsize","setfgcolor",
			"setbgcolor","title","subtitle","bottom","multiline","hiddeninput","progressbar","initvalue",
			"selectable","ignorekey","alwaysontop"
		],
	"title":"card menu",
	"subtitle":"SN Info",
	"bottom":"Back button",
	"focusindex":1,
	"startindex":0,

	"listitem":[
		{
			"index":0,
			"type":"str",
			"stringlength":10,
			"string":"serial number"
		},
		{
			"index":1,
			"type":"bin",
			"datalength":10,
			"data":"base64 encoded data"
		}
		{},{},{}
	]
}
*******************************************************************/
STATIC HCHAR *om_cas_mmi_EncodeUiDataMenuList(DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, void *pvUiData, OxMgrCasUIAttribute_e *pulAttr)
{
	HUINT32 					ulTotalItem = 0, ulItemLoop = 0;
	HCHAR						*pzProperties = NULL;
	HINT32						slStream = HLIB_RWSTREAM_OpenEx(NULL, OM_CASMMI_DEFAULT_SIZE, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	OxMgrCasUiMenuItem_t		*pstListItem = NULL;
	OxMgrCasUiEvtMenulist_t		*pstMenuListEvt = (OxMgrCasUiEvtMenulist_t *)pvUiData;
	OxMgrCasUIAttribute_e		ulAttr = 0;

	ulAttr = pstMenuListEvt->stMenulist.stHeader.ulAttr;

	HLIB_RWSTREAM_Print(slStream, "{\"uitype\":\"menulist\"");
	HLIB_RWSTREAM_Print(slStream, ",\"uidata\":{");

	/* header OxMgrCasUiMenuHeader_t */
	om_cas_mmi_EncodeAttribute(slStream, ulAttr);
	if(ulAttr&eMgrCasUiAttr_Title)
	{
		HLIB_RWSTREAM_Print(slStream, "\"title\":%s", pstMenuListEvt->stMenulist.stHeader.szTitle);
	}

	if(ulAttr&eMgrCasUiAttr_Subtitle)
	{
		if(!(ulAttr&eMgrCasUiAttr_Title))
		{
			HLIB_RWSTREAM_Print(slStream, "\"subtitle\":%s",pstMenuListEvt->stMenulist.stHeader.szSubTitle);
		}
		else
		{
			HLIB_RWSTREAM_Print(slStream, ",\"subtitle\":%s",pstMenuListEvt->stMenulist.stHeader.szSubTitle);
		}
	}

	if(ulAttr&eMgrCasUiAttr_Bottom)
	{
		if(!(ulAttr&eMgrCasUiAttr_Title) && !(ulAttr&eMgrCasUiAttr_Subtitle))
		{
			HLIB_RWSTREAM_Print(slStream, "\"bottom\":%s", pstMenuListEvt->stMenulist.stHeader.szBottom);
		}
		else
		{
			HLIB_RWSTREAM_Print(slStream, ",\"bottom\":%s", pstMenuListEvt->stMenulist.stHeader.szBottom);
		}
	}

	if ((ulAttr&eMgrCasUiAttr_Title) || (ulAttr&eMgrCasUiAttr_Subtitle) || (ulAttr&eMgrCasUiAttr_Bottom))
	{
		HLIB_RWSTREAM_Print(slStream, ",\"focusindex\":%d", pstMenuListEvt->stMenulist.stHeader.ulFocusIndex);
	}
	else
	{
		HLIB_RWSTREAM_Print(slStream, "\"attribute\":[],\"focusindex\":%d", pstMenuListEvt->stMenulist.stHeader.ulFocusIndex);
	}

	HLIB_RWSTREAM_Print(slStream, ",\"startindex\":%d", pstMenuListEvt->stMenulist.stHeader.ulStartIndex);

	/* item OxMgrCasUiMenuItem_t */
	HxLOG_Info("pstMenuListEvt->stMenulist.ulTotalNum %d\n", pstMenuListEvt->stMenulist.ulTotalNum);
	ulTotalItem = pstMenuListEvt->stMenulist.ulTotalNum;
	if(ulTotalItem != 0)
	{
		HxLOG_Info("ulTotalItem %d\n", ulTotalItem);
		HLIB_RWSTREAM_Print(slStream, ",\"listitem\":[");

		do
		{
			/* OxMgrCasUiMenuItem_t */
			HxLOG_Info("ulItemLoop %d\n", ulItemLoop);

			pstListItem = &(pstMenuListEvt->stMenulist.astListItem[ulItemLoop]);

			HLIB_RWSTREAM_Print(slStream, "{\"index\":%d", pstListItem->ulIndex);
			om_cas_mmi_EncodeListItemAttribute(slStream, pstListItem->ulAttr);
			switch(pstListItem->ulType)
			{
				case eMgrCasUiItemType_Str:
					if(pstListItem->utData.stString.ulStringLength == 0){
						HLIB_RWSTREAM_Print(slStream, ",\"type\":\"%s\",\"stringlength\":0,\"string\":\"\"","str");
					} else {
						HLIB_RWSTREAM_Print(slStream, ",\"type\":\"%s\",\"stringlength\":%d,\"string\":%s",
							"str", pstListItem->utData.stString.ulStringLength, pstListItem->utData.stString.szString);
					}
					break;

				case eMgrCasUiItemType_Bin:
					{
						HUINT32		ulDataLength = pstListItem->utData.stBinary.ulDataLength;
						HCHAR 		*endcodedBuffer = NULL;

						endcodedBuffer = HLIB_STD_MemCalloc(pstListItem->utData.stBinary.ulDataLength+1);

						HLIB_RWSTREAM_Print(slStream, ",\"type\":\"%s\"", "bin");
						HLIB_RWSTREAM_Print(slStream, ",\"datalength\":%d", ulDataLength);
						HLIB_MATH_EncodeBase64(endcodedBuffer, (const void *)pstListItem->utData.stBinary.aucData, ulDataLength);
						HLIB_RWSTREAM_Print(slStream, ",\"data\":\"%s\"", endcodedBuffer);

						HLIB_STD_MemFree(endcodedBuffer);
					}
					break;

				default:
					break;
			}

			ulItemLoop = ulItemLoop + 1;
			if(ulItemLoop == ulTotalItem)
			{
				HLIB_RWSTREAM_Print(slStream, "}");
				break;
			}

			HLIB_RWSTREAM_Print(slStream, "},");
		}while(1);
		HLIB_RWSTREAM_Print(slStream, "]");
	}

	HLIB_RWSTREAM_Print(slStream, "}");

	//HxLOG_Info("%s\n", HLIB_RWSTREAM_GetBuf(slStream));

#if 0 // 나중에 om_cas_mmi_FreeUiData에서 한번에 free
	if(pstMenuListEvt->stMenulist.astListItem != NULL)
	{
		HLIB_STD_MemFree(pstMenuListEvt->stMenulist.astListItem);
		pstMenuListEvt->stMenulist.astListItem = NULL;
	}
#endif

	HLIB_RWSTREAM_Print(slStream, ",\"castype\":\"%s\"", om_cas_mmi_getCasTypeText(eCasGrpId));
	HLIB_RWSTREAM_Print(slStream, ",\"slotnumber\":%d}", ulCasSlotNum);

	*pulAttr = ulAttr;

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	return pzProperties;
}

/*******************************************************************
"uitype":"grid"
,"uidata":{
	"attribute":
		[
			"flashing","scrollable","timeout","setposition","randomposition","setsize","setfgcolor",
			"setbgcolor","title","subtitle","bottom","multiline","hiddeninput","progressbar","initvalue",
			"selectable","ignorekey","alwaysontop"
		],
	"numberofcolumn":10,
	"titlealign":["left","right"],
	"title":"mail info",
	"subtitle":"recently",
	"bottom":"Back button",
	"itemalign":["center","right"],
	"focustype":"column",
	"focusposition":{
		"horidx":1,
		"veridx":2
	},
	"startposition":{
		"horidx":1,
		"veridx":2
	},
	"griditem":[
		{
			"index":0,
			"attribute":["disabled","hide","selected"],
			"stringlength":10,
			"string":"SN:"
		},
		{
			"index":1,
			"attribute":["disabled","hide","selected"],
			"stringlength":10,
			"string":"Token"
		},
		{
			"index":2,
			"attribute":["disabled","hide","selected"],
			"stringlength":10,
			"string":"JJAJNG"
		},{},{},{}
	]

}
*******************************************************************/
STATIC HCHAR *om_cas_mmi_EncodeUiDataGrid(DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, void *pvUiData, OxMgrCasUIAttribute_e *pulAttr)
{
	HUINT32 				ulTotalItem = 0, ulItemLoop = 0;
	HCHAR					*pzProperties = NULL;
	HINT32					slStream = HLIB_RWSTREAM_OpenEx(NULL, OM_CASMMI_DEFAULT_SIZE, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	OxMgrCasUiGridItem_t	*pstGridItem = NULL;
	OxMgrCasUiEvtGrid_t		*pstGridEvt = (OxMgrCasUiEvtGrid_t *)pvUiData;

	HLIB_RWSTREAM_Print(slStream, "{\"uitype\":\"grid\"");
	HLIB_RWSTREAM_Print(slStream, ",\"uidata\":{");

	/* OxMgrCasUiGrid_t */

	/* header OxMgrCasUiGridHeader_t */
	om_cas_mmi_EncodeAttribute(slStream, pstGridEvt->stGrid.stHeader.ulAttr);
	HLIB_RWSTREAM_Print(slStream, "\"numberofcolumn\":%d", pstGridEvt->stGrid.stHeader.ulNumOfColumn);
	{
		HCHAR *pzToken = NULL, *pPointPtr = NULL;
		HCHAR *pszTitleAlign = pstGridEvt->stGrid.stHeader.szTitleAlign;

		pzToken = HxSTD_strtok_r(pszTitleAlign, OM_CASMMI_DECODE_OPT_DELIM, &pPointPtr);
		if(pzToken != NULL)
		{
			HLIB_RWSTREAM_Print(slStream, ",\"titlealign\":[\"%s\"", pzToken);

			while(pzToken != NULL)
			{
				pzToken = HxSTD_strtok_r(NULL, OM_CASMMI_DECODE_OPT_DELIM, &pPointPtr);
				HLIB_RWSTREAM_Print(slStream, ",\"%s\"", pzToken);
			}

			HLIB_RWSTREAM_Print(slStream, "]");
		}
	}

	if(HxSTD_StrLen(pstGridEvt->stGrid.stHeader.szTitle) == 0)
		HLIB_RWSTREAM_Print(slStream, ",\"title\":\"\"", pstGridEvt->stGrid.stHeader.szTitle);
	else
		HLIB_RWSTREAM_Print(slStream, ",\"title\":%s", pstGridEvt->stGrid.stHeader.szTitle);

	if(HxSTD_StrLen(pstGridEvt->stGrid.stHeader.szSubTitle) == 0)
		HLIB_RWSTREAM_Print(slStream, ",\"subtitle\":\"\"", pstGridEvt->stGrid.stHeader.szSubTitle);
	else
		HLIB_RWSTREAM_Print(slStream, ",\"subtitle\":%s", pstGridEvt->stGrid.stHeader.szSubTitle);

	if(HxSTD_StrLen(pstGridEvt->stGrid.stHeader.szBottom) == 0)
		HLIB_RWSTREAM_Print(slStream, ",\"bottom\":\"\"", pstGridEvt->stGrid.stHeader.szBottom);
	else
		HLIB_RWSTREAM_Print(slStream, ",\"bottom\":%s", pstGridEvt->stGrid.stHeader.szBottom);

	{
		HCHAR *pzToken = NULL, *pPointPtr = NULL;
		HCHAR *pszItemAlign = pstGridEvt->stGrid.stHeader.szItemAlign;

		pzToken = HxSTD_strtok_r(pszItemAlign, OM_CASMMI_DECODE_OPT_DELIM, &pPointPtr);
		if(pzToken != NULL)
		{
			HLIB_RWSTREAM_Print(slStream, ",\"itemalign\":[\"%s\"", pzToken);

			while(pzToken != NULL)
			{
				pzToken = HxSTD_strtok_r(NULL, OM_CASMMI_DECODE_OPT_DELIM, &pPointPtr);
				if(pzToken != NULL)
				{
					HLIB_RWSTREAM_Print(slStream, ",\"%s\"", pzToken);
				}
			}

			HLIB_RWSTREAM_Print(slStream, "]");
		}
	}

	if(pstGridEvt->stGrid.stHeader.ulFocusType == eMgrCasUiFocusType_Column)
	{
		HLIB_RWSTREAM_Print(slStream, ",\"focustype\":\"%s\"", "column");
	}
	else if(pstGridEvt->stGrid.stHeader.ulFocusType == eMgrCasUiFocusType_Cell)
	{
		HLIB_RWSTREAM_Print(slStream, ",\"focustype\":\"%s\"", "cell");
	}

	HLIB_RWSTREAM_Print(slStream, ",\"focusposition\":{\"horidx\":%d, \"veridx\":%d}",
		pstGridEvt->stGrid.stHeader.stFocus.ulHorIdx, pstGridEvt->stGrid.stHeader.stFocus.ulVerIdx);
	HLIB_RWSTREAM_Print(slStream, ",\"startposition\":{\"horidx\":%d, \"veridx\":%d}",
		pstGridEvt->stGrid.stHeader.stStartPos.ulHorIdx, pstGridEvt->stGrid.stHeader.stStartPos.ulVerIdx);

	/* item OxMgrCasUiMenuItem_t */
	ulTotalItem = pstGridEvt->stGrid.ulTotalNum;
	if(ulTotalItem != 0)
	{
		HLIB_RWSTREAM_Print(slStream, ",\"griditem\":[");
		do
		{
			/* OxMgrCasUiGridItem_t */
			pstGridItem = &(pstGridEvt->stGrid.astGridItem[ulItemLoop]);

			HLIB_RWSTREAM_Print(slStream, "{\"index\":%d", pstGridItem->ulIndex);
			om_cas_mmi_EncodeListItemAttribute(slStream, pstGridItem->ulAttr);
			HLIB_RWSTREAM_Print(slStream, ",\"stringlength\":%d", pstGridItem->ulStringLength);
			HLIB_RWSTREAM_Print(slStream, ",\"string\":%s", pstGridItem->szString);

			ulItemLoop = ulItemLoop + 1;
			if(ulItemLoop == ulTotalItem)
			{
				HLIB_RWSTREAM_Print(slStream, "}");
				break;
			}

			HLIB_RWSTREAM_Print(slStream, "},");
		}while(1);
		HLIB_RWSTREAM_Print(slStream, "]");
	}

	HLIB_RWSTREAM_Print(slStream, "}");
	HLIB_RWSTREAM_Print(slStream, ",\"castype\":\"%s\"", om_cas_mmi_getCasTypeText(eCasGrpId));
	HLIB_RWSTREAM_Print(slStream, ",\"slotnumber\":%d}", ulCasSlotNum);

	*pulAttr = pstGridEvt->stGrid.stHeader.ulAttr;

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	return pzProperties;
}

/*******************************************************************
"uitype":"inputdialog"
,"uidata":{
	"attribute":
		[
			"flashing","scrollable","timeout","setposition","randomposition","setsize","setfgcolor",
			"setbgcolor","title","subtitle","bottom","multiline","hiddeninput","progressbar","initvalue",
			"selectable","ignorekey","alwaysontop"
		],
	"timeoutsecond":10,
	"fontsize":20,
	"posx":10,
	"posy":20,
	"width":20,
	"height":20,
	"bgcolor":20,
	"fgcolor":20,
	"progress":20,
	"title":"insert PIN",
	"subtitle":"ppv channel",
	"bottom":"back button",
	"inputtype":"string",
	"inputdata":{
		"attribute":
			[
				"flashing","scrollable","timeout","setposition","randomposition","setsize","setfgcolor",
				"setbgcolor","title","subtitle","bottom","multiline","hiddeninput","progressbar","initvalue",
				"selectable","ignorekey","alwaysontop"
			],
		"stringlength":10,
		"string":"ABC"
	}
}
*******************************************************************/

STATIC HCHAR *om_cas_mmi_EncodeUiDataInputDialog(DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, void *pvUiData, OxMgrCasUIAttribute_e *pulAttr)
{
	HCHAR						*pzProperties = NULL;
	HINT32						slStream = HLIB_RWSTREAM_OpenEx(NULL, OM_CASMMI_DEFAULT_SIZE, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	OxMgrCasUiEvtInput_t		*pstInputDialogEvt = (OxMgrCasUiEvtInput_t *)pvUiData;
	OxMgrCasUiInputData_t		*pstInputData = NULL;
	OxMgrCasUIAttribute_e		ulAttr = 0;

	HLIB_RWSTREAM_Print(slStream, "{\"uitype\":\"inputdialog\"");
	HLIB_RWSTREAM_Print(slStream, ",\"uidata\":{");

	/* OxMgrCasUiEvtInput_t */

	/* header OxMgrCasUiMenuHeader_t */
	ulAttr = pstInputDialogEvt->stHeader.ulAttr;
	om_cas_mmi_EncodeAttribute(slStream, ulAttr);
	HLIB_RWSTREAM_Print(slStream, "\"timeoutsecond\":%d", pstInputDialogEvt->stHeader.ulTimeoutSecond);
	HLIB_RWSTREAM_Print(slStream, ",\"fontsize\":%d", pstInputDialogEvt->stHeader.ulFontSize);
	HLIB_RWSTREAM_Print(slStream, ",\"posx\":%d", pstInputDialogEvt->stHeader.ulPosX);
	HLIB_RWSTREAM_Print(slStream, ",\"posy\":%d", pstInputDialogEvt->stHeader.ulPosY);
	HLIB_RWSTREAM_Print(slStream, ",\"width\":%d", pstInputDialogEvt->stHeader.ulWidth);
	HLIB_RWSTREAM_Print(slStream, ",\"height\":%d", pstInputDialogEvt->stHeader.ulHeight);
	HLIB_RWSTREAM_Print(slStream, ",\"bgcolor\":%d", pstInputDialogEvt->stHeader.ulBgColor);
	HLIB_RWSTREAM_Print(slStream, ",\"fgcolor\":%d", pstInputDialogEvt->stHeader.ulFgColor);
	HLIB_RWSTREAM_Print(slStream, ",\"progress\":%d", pstInputDialogEvt->stHeader.ulProgress);
	if(ulAttr&eMgrCasUiAttr_Title){
		HLIB_RWSTREAM_Print(slStream, ",\"title\":%s", pstInputDialogEvt->stHeader.szTitle);
	}
	if(ulAttr&eMgrCasUiAttr_Subtitle){
		HLIB_RWSTREAM_Print(slStream, ",\"subtitle\":%s", pstInputDialogEvt->stHeader.szSubTitle);
	}
	if(ulAttr&eMgrCasUiAttr_Bottom){
		HLIB_RWSTREAM_Print(slStream, ",\"bottom\":%s", pstInputDialogEvt->stHeader.szBottom);
	}
	/* data OxMgrCasUiInput_t */
	HLIB_RWSTREAM_Print(slStream, ",\"inputtype\":\"%s\"", om_cas_mmi_getInputTypeText(pstInputDialogEvt->stInputData.eInputType));
	HLIB_RWSTREAM_Print(slStream, ",\"inputdata\":{");

	pstInputData = &pstInputDialogEvt->stInputData.utInputData;
	switch(pstInputDialogEvt->stInputData.eInputType)
	{
		case eMgrCasInputType_Button:
			/* OxMgrCasUiInputButton_t */
			om_cas_mmi_EncodeAttribute(slStream, pstInputData->stBtn.ulAttr);
			om_cas_mmi_EncodeInputButtonList(slStream, pstInputData->stBtn.eMgrCasBtnType, pstInputData->stBtn.aszButtonName);
			break;

		case eMgrCasInputType_Number:
			/* OxMgrCasUiInputNumber_t */
			om_cas_mmi_EncodeAttribute(slStream, pstInputData->stNumber.ulAttr);
			HLIB_RWSTREAM_Print(slStream, "\"number\":%d", pstInputData->stNumber.ulNumber);
			HLIB_RWSTREAM_Print(slStream, ",\"answerlength\":%d", pstInputData->stNumber.ulNumberLength);
			HLIB_RWSTREAM_Print(slStream, ",\"answerstring\":\"%s\"", pstInputData->stNumber.pszNumber);
			break;

		case eMgrCasInputType_String:
			/* OxMgrCasUiInputString_t */
			om_cas_mmi_EncodeAttribute(slStream, pstInputData->stString.ulAttr);
			HLIB_RWSTREAM_Print(slStream, "\"stringlength\":%d", pstInputData->stString.ulStringLength);
			HLIB_RWSTREAM_Print(slStream, ",\"string\":%s", pstInputData->stString.szString);
			break;

		case eMgrCasInputType_Selection:
			/* OxMgrCasUiInputSelection_t */
			pstInputData->stSelection;
			om_cas_mmi_EncodeAttribute(slStream, pstInputData->stSelection.ulAttr);
			HLIB_RWSTREAM_Print(slStream, "\"itemidx\":%d", pstInputData->stSelection.ulItemIdx);
			HLIB_RWSTREAM_Print(slStream, ",\"datalength\":%d", pstInputData->stSelection.ulDataLength);
			HLIB_RWSTREAM_Print(slStream, ",\"data\":%s", pstInputData->stSelection.aucData);
			break;

		case eMgrCasInputType_Etc:
		case eMgrCasInputType_MAX:
		default:
			break;
	}

	HLIB_RWSTREAM_Print(slStream, "}}");
	HLIB_RWSTREAM_Print(slStream, ",\"castype\":\"%s\"", om_cas_mmi_getCasTypeText(eCasGrpId));
	HLIB_RWSTREAM_Print(slStream, ",\"slotnumber\":%d}", ulCasSlotNum);

	*pulAttr = ulAttr;

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	return pzProperties;
}

/*******************************************************************
"uitype":"inputdialogex"
,"uidata":{
	"attribute":
		[
			"flashing","scrollable","timeout","setposition","randomposition","setsize","setfgcolor",
			"setbgcolor","title","subtitle","bottom","multiline","hiddeninput","progressbar","initvalue",
			"selectable","ignorekey","alwaysontop"
		],
	"timeoutsecond":10,
	"fontsize":20,
	"posx":10,
	"posy":20,
	"width":20,
	"height":20,
	"bgcolor":20,
	"fgcolor":20,
	"progress":20,
	"title":"insert PIN",
	"subtitle":"ppv channel",
	"bottom":"back button",
	"inputtype":"string",
	"inputdata":{
		"attribute":
			[
				"flashing","scrollable","timeout","setposition","randomposition","setsize","setfgcolor",
				"setbgcolor","title","subtitle","bottom","multiline","hiddeninput","progressbar","initvalue",
				"selectable","ignorekey","alwaysontop"
			],
		"stringlength":10,
		"string":"ABC"
	}
}
*******************************************************************/

STATIC HCHAR *om_cas_mmi_EncodeUiDataInputDialogEx(DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, void *pvUiData, OxMgrCasUIAttribute_e *pulAttr)
{
	HCHAR						*pzProperties = NULL;
	HINT32						slStream = HLIB_RWSTREAM_OpenEx(NULL, OM_CASMMI_DEFAULT_SIZE, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	OxMgrCasUiEvtInput_t		*pstInputDialogEvt = (OxMgrCasUiEvtInput_t *)pvUiData;
	OxMgrCasUiInputData_t		*pstInputData = NULL;
	OxMgrCasUIAttribute_e		ulAttr = 0;

	HLIB_RWSTREAM_Print(slStream, "{\"uitype\":\"inputdialogex\"");
	HLIB_RWSTREAM_Print(slStream, ",\"uidata\":{");

	/* OxMgrCasUiEvtInput_t */

	/* header OxMgrCasUiMenuHeader_t */
	ulAttr = pstInputDialogEvt->stHeader.ulAttr;
	om_cas_mmi_EncodeAttribute(slStream, ulAttr);
	HLIB_RWSTREAM_Print(slStream, "\"timeoutsecond\":%d", pstInputDialogEvt->stHeader.ulTimeoutSecond);
	HLIB_RWSTREAM_Print(slStream, ",\"fontsize\":%d", pstInputDialogEvt->stHeader.ulFontSize);
	HLIB_RWSTREAM_Print(slStream, ",\"posx\":%d", pstInputDialogEvt->stHeader.ulPosX);
	HLIB_RWSTREAM_Print(slStream, ",\"posy\":%d", pstInputDialogEvt->stHeader.ulPosY);
	HLIB_RWSTREAM_Print(slStream, ",\"width\":%d", pstInputDialogEvt->stHeader.ulWidth);
	HLIB_RWSTREAM_Print(slStream, ",\"height\":%d", pstInputDialogEvt->stHeader.ulHeight);
	HLIB_RWSTREAM_Print(slStream, ",\"bgcolor\":%d", pstInputDialogEvt->stHeader.ulBgColor);
	HLIB_RWSTREAM_Print(slStream, ",\"fgcolor\":%d", pstInputDialogEvt->stHeader.ulFgColor);
	HLIB_RWSTREAM_Print(slStream, ",\"progress\":%d", pstInputDialogEvt->stHeader.ulProgress);
	if(ulAttr&eMgrCasUiAttr_Title){
		HLIB_RWSTREAM_Print(slStream, ",\"title\":%s", pstInputDialogEvt->stHeader.szTitle);
	}
	if(ulAttr&eMgrCasUiAttr_Subtitle){
		HLIB_RWSTREAM_Print(slStream, ",\"subtitle\":%s", pstInputDialogEvt->stHeader.szSubTitle);
	}
	if(ulAttr&eMgrCasUiAttr_Bottom){
		HLIB_RWSTREAM_Print(slStream, ",\"bottom\":%s", pstInputDialogEvt->stHeader.szBottom);
	}
	/* data OxMgrCasUiInput_t */
	HLIB_RWSTREAM_Print(slStream, ",\"inputtype\":\"%s\"", om_cas_mmi_getInputTypeText(pstInputDialogEvt->stInputData.eInputType));
	HLIB_RWSTREAM_Print(slStream, ",\"inputdata\":{");

	pstInputData = &pstInputDialogEvt->stInputData.utInputData;
	switch(pstInputDialogEvt->stInputData.eInputType)
	{
		case eMgrCasInputType_Button:
			/* OxMgrCasUiInputButton_t */
			om_cas_mmi_EncodeAttribute(slStream, pstInputData->stBtn.ulAttr);
			om_cas_mmi_EncodeInputButtonList(slStream, pstInputData->stBtn.eMgrCasBtnType, pstInputData->stBtn.aszButtonName);
			break;

		case eMgrCasInputType_Number:
			/* OxMgrCasUiInputNumber_t */
			om_cas_mmi_EncodeAttribute(slStream, pstInputData->stNumber.ulAttr);
			HLIB_RWSTREAM_Print(slStream, "\"number\":%d", pstInputData->stNumber.ulNumber);
			HLIB_RWSTREAM_Print(slStream, ",\"answerlength\":%d", pstInputData->stNumber.ulNumberLength);
			HLIB_RWSTREAM_Print(slStream, ",\"answerstring\":\"%s\"", pstInputData->stNumber.pszNumber);
			break;

		case eMgrCasInputType_String:
			/* OxMgrCasUiInputString_t */
			om_cas_mmi_EncodeAttribute(slStream, pstInputData->stString.ulAttr);
			HLIB_RWSTREAM_Print(slStream, "\"stringlength\":%d", pstInputData->stString.ulStringLength);
			HLIB_RWSTREAM_Print(slStream, ",\"string\":%s", pstInputData->stString.szString);
			break;

		case eMgrCasInputType_Selection:
			/* OxMgrCasUiInputSelection_t */
			pstInputData->stSelection;
			om_cas_mmi_EncodeAttribute(slStream, pstInputData->stSelection.ulAttr);
			HLIB_RWSTREAM_Print(slStream, "\"itemidx\":%d", pstInputData->stSelection.ulItemIdx);
			HLIB_RWSTREAM_Print(slStream, ",\"datalength\":%d", pstInputData->stSelection.ulDataLength);
			HLIB_RWSTREAM_Print(slStream, ",\"data\":%s", pstInputData->stSelection.aucData);
			break;

		case eMgrCasInputType_Etc:
		case eMgrCasInputType_MAX:
		default:
			break;
	}

	HLIB_RWSTREAM_Print(slStream, "}}");
	HLIB_RWSTREAM_Print(slStream, ",\"castype\":\"%s\"", om_cas_mmi_getCasTypeText(eCasGrpId));
	HLIB_RWSTREAM_Print(slStream, ",\"slotnumber\":%d}", ulCasSlotNum);

	*pulAttr = ulAttr;

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	return pzProperties;
}

STATIC HINT32 om_cas_mmi_EncodeUiData(HINT32 slStream,
	DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, OxMgrCasUiType_e eUiType, void *pvUiData, OxMgrCasUIAttribute_e *pulAttr)
{
	HUINT32					ulStreamSize = 0, ulNeed = 0;
	HCHAR 					*endcodedBuffer = NULL;
	HCHAR					*pzProperties = NULL;

	switch(eUiType)
	{
		case eMgrCasUiType_Banner:
			pzProperties = om_cas_mmi_EncodeUiDataBanner(eCasGrpId, ulCasSlotNum, pvUiData, pulAttr);
			break;

		case eMgrCasUiType_TextBox:
			pzProperties = om_cas_mmi_EncodeUiDataTextBox(eCasGrpId, ulCasSlotNum, pvUiData, pulAttr);
			break;

		case eMgrCasUiType_MailMessage:
			pzProperties = om_cas_mmi_EncodeUiDataMailMessage(eCasGrpId, ulCasSlotNum, pvUiData, pulAttr);
			break;

		case eMgrCasUiType_MailMessageList:
			pzProperties = om_cas_mmi_EncodeUiDataMailMessageList(eCasGrpId, ulCasSlotNum, pvUiData, pulAttr);
			break;

		case eMgrCasUiType_MenuList:
			pzProperties = om_cas_mmi_EncodeUiDataMenuList(eCasGrpId, ulCasSlotNum, pvUiData, pulAttr);
			break;

		case eMgrCasUiType_Grid:
			pzProperties = om_cas_mmi_EncodeUiDataGrid(eCasGrpId, ulCasSlotNum, pvUiData, pulAttr);
			break;

		case eMgrCasUiType_InputDialog:
			pzProperties = om_cas_mmi_EncodeUiDataInputDialog(eCasGrpId, ulCasSlotNum, pvUiData, pulAttr);
			break;

		case eMgrCasUiType_InputDialog_Ex:
			pzProperties = om_cas_mmi_EncodeUiDataInputDialogEx(eCasGrpId, ulCasSlotNum, pvUiData, pulAttr);
			break;

		case eMgrCasUiType_MAX:
			pzProperties = om_cas_mmi_EncodeUiDataClose(eCasGrpId, ulCasSlotNum);
			break;

		default:
			break;
	}


	if(pzProperties != NULL)
	{
		if(pzProperties != NULL)
		{
			ulStreamSize = HxSTD_StrLen(pzProperties);
			ulNeed = HLIB_MATH_EncodeBase64(NULL, (const void *)pzProperties, ulStreamSize);
			endcodedBuffer = HLIB_STD_MemCalloc(ulNeed+1);
			if(endcodedBuffer != NULL)
			{
				HLIB_MATH_EncodeBase64(endcodedBuffer, (const void *)pzProperties, ulStreamSize);
			}
			HLIB_STD_MemFree(pzProperties);
		}
		HLIB_RWSTREAM_Print(slStream, endcodedBuffer);
		HLIB_STD_MemFree(endcodedBuffer);
	}


	return slStream;
}


STATIC HCHAR	*om_cas_mmi_EncodeUiDataProperty(
	DxCAS_GroupId_e eCasGrpId, HUINT32 ulCasSlotNum, OxMgrCasUiType_e eUiType, void *pvUiData, OxMgrCasUIAttribute_e *pulAttr)
{
	HINT32		slStream = 0;
	HCHAR		*pzProperties = NULL;

	slStream = om_cas_mmi_CreateStream(eUiType);
	if(slStream == 0)
	{
		HxLOG_Error("\n");
		HxLOG_Assert(FALSE);
	}

	om_cas_mmi_EncodeUiData(slStream, eCasGrpId, ulCasSlotNum, eUiType, pvUiData, pulAttr);

	pzProperties = om_cas_mmi_Closestream(slStream);

	return pzProperties;
}


#define _____OAPI_COMMAND_FUNCTIONS_____





STATIC void om_cas_mmi_ChangeSessionState(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	DxCAS_MmiSessionType_e eSessionType;
	DxCAS_GroupId_e eCasGrpId;
	HUINT32 ulCasSlotNum;
	OxCasMmiSessionStatus_e			eAppSessionStatus;
	OmCasMmiInstanceInfo_t		*pstInst = NULL;
	OmCasMmiSessionInfo_t		*pstSessionInfo = NULL;
	HERROR							hError = ERR_OK;
	HxList_t						*pstConnItem = NULL;
	OmCasMmiConnectionInfo_t		*pstConnInfo = NULL;
//	HERROR			hErr = ERR_FAIL;
//	HINT32			nRpcHandle = 0;
	Handle_t			hSession;

	HxLOG_Trace();

	eCasGrpId = HxOBJECT_INT(argv[0]);
	ulCasSlotNum = HxOBJECT_INT(argv[1]);
	eSessionType = HxOBJECT_INT(argv[2]);
	eAppSessionStatus = HxOBJECT_INT(argv[3]);
	hSession = (Handle_t)HxOBJECT_INT(argv[4]);
	HxLOG_Debug("hSession: 0x%x \n", hSession);


	pstInst = om_cas_mmi_GetSubCasInstInfo(eCasGrpId);
	if(pstInst == NULL)
	{
		HxLOG_Error("instance NULL\n");
		hError = ERR_FAIL;

		goto END_FUNC;
	}
	pstSessionInfo = om_cas_mmi_GetSessionInfoBySessionTypeAndSessionHandle(pstInst->pstSessionList, eSessionType, hSession);
	if(pstSessionInfo == NULL)
	{
		HxLOG_Error("session handle NULL\n");
		hError = ERR_FAIL;

		goto END_FUNC;
	}

	if(eAppSessionStatus == eOxCASMMI_SESSIONSTATUS_MAX)
	{
		HxLOG_Error("NOT found status\n");
		hError = ERR_FAIL;

		goto END_FUNC;
	}

	switch(eAppSessionStatus)
	{

		case eOxCASMMI_SESSIONSTATUS_STARTED:
			HxLOG_Info("eCASUI_STATUS_SessionStarted...\n");
			pstConnItem = pstSessionInfo->pstConn;
			while(pstConnItem != NULL)
			{
				pstConnInfo = (OmCasMmiConnectionInfo_t *)HLIB_LIST_Data(pstConnItem);
				if(pstConnInfo != NULL)
				{
					if(pstConnInfo->nSlot == ulCasSlotNum)
					{
						break;
					}
				}

				pstConnItem = pstConnItem->next;
				pstConnInfo = NULL;
			}

			HxLOG_Info("pstConnInfo[%08x]\n", pstConnInfo);
			if(pstConnInfo != NULL)
			{
				pstConnInfo->bStarted = TRUE;
			}
			break;

		default:
			break;
	}

END_FUNC:

	if (hError != ERR_OK)
	{
		HxLOG_Error("Something wrong..!! \n");
	}

	return ;

}

STATIC void om_cas_mmi_OpenSession(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	DxCAS_MmiSessionType_e		eReqSessionType = eDxCAS_SESSIONTYPE_NONE;

	DxCAS_GroupId_e				 eCasGrpId;
	HUINT32						 ulCasSlotNum;
	OxMgrCasMenuGroupId_e		 eCasMenuGroupId = eMgrCasMenuGroup_None;
	HERROR						 hError = ERR_OK;
	HCHAR						*pzMenuGroup = NULL;
	HUINT32						 ulLength = 0;
	Handle_t					 hSession;
	HUINT32						 ulUiHandle;
	OxMgrCasSessionEvent_t		 stSessionEvent;
	HxLOG_Trace();

	hSession = (Handle_t)HxOBJECT_INT(argv[0]);
	ulUiHandle = (Handle_t)HxOBJECT_INT(argv[1]);
	eCasGrpId = HxOBJECT_INT(argv[2]);
	ulCasSlotNum = HxOBJECT_INT(argv[3]);
	eReqSessionType = HxOBJECT_INT(argv[4]);
	pzMenuGroup = HxOBJECT_BIN_DATA(argv[5]);

	if(hSession == (Handle_t)NULL)
	{
		HxLOG_Error("hSession is NULL \n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	HxLOG_Debug("hSession: %p \n", hSession);

	// web ap 에서 json type의 param 을 "param:param1" 형태로 내려주는 경우가 존재.
	if(HxSTD_StrCmp(pzMenuGroup, "NULL") != 0)
	{
		ulLength = HxOBJECT_BIN_SIZE(argv[5]);
		pzMenuGroup[ulLength] = '\0';
		HxLOG_Info("pzMenuGroup: %s \n", pzMenuGroup);
		eCasMenuGroupId = om_cas_mmi_dec_value_to_enum(_oapi_cas_mmi_menugroup_table.taglist , pzMenuGroup);
		HxLOG_Info("eCasMenuGroupId: %d \n", eCasMenuGroupId);
	}


	if(eReqSessionType == eDxCAS_SESSIONTYPE_MAX)
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	if(eCasGrpId == eDxCAS_GROUPID_NONE)
	{
		HxLOG_Error("eCasGrpId: 0x%x\n", eCasGrpId);
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	stSessionEvent.ulCasSlotNum = ulCasSlotNum;
	stSessionEvent.eSessionType = eReqSessionType;
	stSessionEvent.hSession = hSession;
	stSessionEvent.ulUiHandle = ulUiHandle;
	stSessionEvent.eCasMenuGroupId = eCasMenuGroupId;

	hError = om_cas_mmi_PostCasEventDataToCasUiProc(eCasGrpId,
												ulCasSlotNum,
												0,
												eOEVT_CAS_CASUI_APP_OPEN_SESSION,
												0,
												0,
												&stSessionEvent,
												sizeof(OxMgrCasSessionEvent_t));
	if(hError != ERR_OK)
	{
		HxLOG_Info("om_cas_mmi_SendCasEventToCasUiProc ERROR\n");
	}




END_FUNC:
	return ;

}
STATIC void om_cas_mmi_CloseSession(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	DxCAS_MmiSessionType_e			eReqSessionType = eDxCAS_SESSIONTYPE_NONE;

	DxCAS_GroupId_e eCasGrpId;
	HUINT32 ulCasSlotNum;
	HERROR						hError = ERR_OK;
	OmCasMmiInstanceInfo_t	*pstInst = NULL;
	OmCasMmiSessionInfo_t	*pstSessionInfo = NULL;
	Handle_t						hSession;
	HUINT32 					ulUiHandle;
	OxMgrCasSessionEvent_t			stSessionEvent;


	HxLOG_Trace();


	hSession = (Handle_t)HxOBJECT_INT(argv[0]);
	ulUiHandle = (Handle_t)HxOBJECT_INT(argv[1]);
	eCasGrpId = HxOBJECT_INT(argv[2]);
	ulCasSlotNum = HxOBJECT_INT(argv[3]);
	eReqSessionType = HxOBJECT_INT(argv[4]);



	if(eCasGrpId == eDxCAS_GROUPID_NONE)
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}



	pstInst = om_cas_mmi_GetSubCasInstInfo(eCasGrpId);
	if(pstInst == NULL)
	{
		HxLOG_Error("instance NULL\n");
		hError = ERR_FAIL;

		goto END_FUNC;
	}

	pstSessionInfo = om_cas_mmi_GetSessionInfoBySessionTypeAndSessionHandle(pstInst->pstSessionList, eReqSessionType, hSession);
	if(pstSessionInfo == NULL)
	{
		HxLOG_Error("session handle NULL\n");
		hError = ERR_FAIL;

		goto END_FUNC;
	}

	HxLOG_Info("application request session stop(type %d)\n", pstSessionInfo->eSessionType);

	stSessionEvent.ulCasSlotNum = ulCasSlotNum;
	stSessionEvent.eSessionType = eReqSessionType;
	stSessionEvent.hSession = hSession;
	stSessionEvent.ulUiHandle = ulUiHandle;


	(void)om_cas_mmi_RemoveSessionInfo(&(pstInst->pstSessionList), pstSessionInfo);

	hError = om_cas_mmi_PostCasEventDataToCasUiProc(eCasGrpId,
												ulCasSlotNum,
												0,
												eOEVT_CAS_CASUI_APP_CLOSE_SESSION,
												0,
												0,
												&stSessionEvent,
												sizeof(OxMgrCasSessionEvent_t));

	if(hError != ERR_OK)
	{
		HxLOG_Info("om_cas_mmi_SendCasEventToCasUiProc ERROR\n");
	}

END_FUNC:

	return ;

}



STATIC void om_cas_mmi_UiResponse(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	DxCAS_MmiSessionType_e			eReqSessionType = eDxCAS_SESSIONTYPE_NONE;

	DxCAS_GroupId_e eCasGrpId;
	HUINT32 ulCasSlotNum;
	BUS_Result_t						eResult = ERR_BUS_INITIAL_FAILED;
	OmCasMmiInstanceInfo_t	*pstInst = NULL;
	OmCasMmiSessionInfo_t	*pstSessionInfo = NULL;
	OxMgrCasUiUserInputNotify_t		*pstUserInputNotify = NULL;
	HCHAR							*pzUserInputType = NULL;
	HxJSON_t						json_input = NULL;
	HxJSON_t						json_userInputData = NULL;
	HUINT32						ulLength = 0;
	HCHAR						*pzInputData = NULL;
	Handle_t							hSession;
//	HUINT32							ulUiHandle;

	HxLOG_Trace();



	eCasGrpId = HxOBJECT_INT(argv[0]);
	ulCasSlotNum = HxOBJECT_INT(argv[1]);
	eReqSessionType = HxOBJECT_INT(argv[2]);
	pzInputData = HxOBJECT_BIN_DATA(argv[3]);
	hSession = (Handle_t)HxOBJECT_INT(argv[4]);

	if(pzInputData != NULL)
	{
		ulLength = HxOBJECT_BIN_SIZE(argv[3]);
		pzInputData[ulLength] = '\0';

		HxLOG_Info("pzInputData: %s \n", pzInputData);

		json_input = HLIB_JSON_Decode(pzInputData);

	}

	if(json_input == NULL)
	{
		HxLOG_Error("\n");

		goto END_FUNC;
	}

	if(eCasGrpId == eDxCAS_GROUPID_NONE)
	{
		HxLOG_Error("\n");

		goto END_FUNC;
	}

	pstInst = om_cas_mmi_GetSubCasInstInfo(eCasGrpId);
	if(pstInst == NULL)
	{
		HxLOG_Error("instance NULL\n");

		goto END_FUNC;
	}
	pstSessionInfo = om_cas_mmi_GetSessionInfoBySessionTypeAndSessionHandle(pstInst->pstSessionList, eReqSessionType, hSession);
	if(pstSessionInfo == NULL)
	{
		HxLOG_Error("session handle NULL\n");

		goto END_FUNC;
	}

	pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiUserInputNotify_t));
	if(pstUserInputNotify == NULL)
	{
		HxLOG_Error("memory allocation failed\n");

		goto END_FUNC;
	}

	pzUserInputType = (HCHAR *)HLIB_JSON_Object_GetString(json_input, "inputtype");
	if(pzUserInputType == NULL)
	{
		HxLOG_Error("NOT found inputtype key\n");

		goto END_FUNC;
	}

	pstUserInputNotify->eInputType = eMgrCasInputType_MAX;
	pstUserInputNotify->eInputType = om_cas_mmi_dec_value_to_enum(_casui_app_inputtype_table.taglist, pzUserInputType);
	if(pstUserInputNotify->eInputType == eMgrCasInputType_MAX)
	{
		HxLOG_Error("invalid inputtype\n");

		goto END_FUNC;
	}

	json_userInputData = HLIB_JSON_Object_GetObject(json_input, "inputdata");
	if(json_userInputData == NULL)
	{
		HxLOG_Error("NOT found inputdata key\n");

		goto END_FUNC;
	}

	om_cas_mmi_DecodeUserInputData(pstUserInputNotify->eInputType, json_userInputData, &(pstUserInputNotify->utData));

#if defined(CONFIG_DEBUG)
	switch(pstUserInputNotify->eInputType)
	{
		case eMgrCasInputType_Button:
			{
				OxMgrCasUiNotifyButton_t	*pstBtn = (OxMgrCasUiNotifyButton_t *)&(pstUserInputNotify->utData);
				HxLOG_Info("input type button %d\n", pstUserInputNotify->eInputType);
				HxLOG_Info("\t btn type %d\n", pstBtn->eMgrCasBtnType);
			}
			break;

		case eMgrCasInputType_Number:
			{
				OxMgrCasUiNotifyNumber_t	*pstNumber = (OxMgrCasUiNotifyNumber_t *)&(pstUserInputNotify->utData);
				HxLOG_Info("input type number %d\n", pstUserInputNotify->eInputType);
				HxLOG_Info("\t number %d\n", pstNumber->ulNumber);
				HxLOG_Info("\t ulNumberLength %d\n", pstNumber->ulNumberLength);
				HxLOG_Info("\t numberstring %s\n", pstNumber->pszNumber);
			}
			break;

		case eMgrCasInputType_String:
			{
				OxMgrCasUiNotifyString_t	*pstString = (OxMgrCasUiNotifyString_t *)&(pstUserInputNotify->utData);
				HxLOG_Info("input type string %d\n", pstUserInputNotify->eInputType);
				HxLOG_Info("\t stringlength %d\n", pstString->ulStringLength);
				HxLOG_Info("\t string %s\n", pstString->szString);
			}
			break;

		case eMgrCasInputType_Selection:
			{
				OxMgrCasUiNotifySelection_t *pstSelection = (OxMgrCasUiNotifySelection_t *)&(pstUserInputNotify->utData);
				HxLOG_Info("input type selection %d\n", pstUserInputNotify->eInputType);
				HxLOG_Info("\t itemidx %d\n", pstSelection->ulItemIdx);
				HxLOG_Info("\t ulDataLength %d\n", pstSelection->ulDataLength);
				HxLOG_Info("\t aucData %s\n", pstSelection->aucData);
			}
			break;

		case eMgrCasInputType_Etc:
			{
				OxMgrCasUiNotifyEtc_t	*pstEtc = (OxMgrCasUiNotifyEtc_t *)&(pstUserInputNotify->utData);
				HxLOG_Info("input type ETC %d\n", pstUserInputNotify->eInputType);
				HxLOG_Info("\t etc type %d\n", pstEtc->eMgrCasEtcType);
			}
			break;

		case eMgrCasInputType_MAX:
		default:
			break;
	}
#endif /* defined(CONFIG_DEBUG) */


	pstUserInputNotify->hSession = hSession;
	pstUserInputNotify->ulUiHandle = pstSessionInfo->uiHandle;

	eResult = om_cas_mmi_PostCasEventDataToCasUiProc(pstInst->eCasGrpId,
													(HINT32)ulCasSlotNum,
													0,
													eOEVT_CAS_CASUI_APP_USERINPUTDATA,
													(HINT32)ulCasSlotNum,
													(HINT32)pstSessionInfo->eSessionType,
													(void *)pstUserInputNotify,
													sizeof(OxMgrCasUiUserInputNotify_t));
	if(eResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Info("om_cas_mmi_PostCasEventDataToCasUiProc ERROR\n");
	}

END_FUNC:

	if(pstUserInputNotify != NULL)
	{
		HLIB_STD_MemFree(pstUserInputNotify);
		pstUserInputNotify = NULL;
	}


	if(json_input != NULL)
	{
		HLIB_JSON_Delete(json_input);
		json_input = NULL;
	}

	return ;

}





#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_cas_mmi(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR						hError = ERR_OK;
	HUINT32						ulCasSlotNum = 0;
	void						*pvData = NULL;
	BUS_Result_t				hBusResult = MESSAGE_PASS;
	DxCAS_GroupId_e				eCasGrpId = eDxCAS_GROUPID_NONE;

	eCasGrpId = (DxCAS_GroupId_e)p1;
	ulCasSlotNum = (HUINT32)p2;
	pvData = (void *)p3;

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Info("%s created\n", (HINT8*) __FUNCTION__);
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;

		case eMEVT_CASUI_REQ_OPEN_SESSION:
		case eMEVT_CASUI_REQ_CLOSE_SESSION:
		case eMEVT_CASUI_SESSION_OPENED:
			hBusResult = MESSAGE_BREAK;

			if (pvData != NULL)
			{
				HxLOG_Info("SESSIONevent CasGrpId(0x%x) SlotNum(%d) pvData(0x%x)\n", eCasGrpId, ulCasSlotNum, pvData);
				hError = om_cas_mmi_SessionProcEvent(message, eCasGrpId, ulCasSlotNum, pvData);
				if (hError != ERR_OK)
				{
					HxLOG_Error("Error! om_cas_mmi_SessionProcEvent() failed. \n");
				}
			}
			else
			{
				HxLOG_Error("Error! Invalid param! \n");
			}
			break;

		case eMEVT_CASUI_TYPE_BANNER_DATA:
		case eMEVT_CASUI_TYPE_TEXTBOX_DATA:
		case eMEVT_CASUI_TYPE_MAIL_DATA:
		case eMEVT_CASUI_TYPE_MAILLIST_DATA:
		case eMEVT_CASUI_TYPE_MENULIST_DATA:
		case eMEVT_CASUI_TYPE_GRID_DATA:
		case eMEVT_CASUI_TYPE_INPUT_DATA:
		case eMEVT_CASUI_TYPE_INPUT_EX_DATA:
		case eMEVT_CASUI_TYPE_CLOSE:
			hBusResult = MESSAGE_BREAK;

			if (pvData != NULL)
			{
				HxLOG_Info("CASUI_TYPEevent CasGrpId(0x%x) SlotNum(%d) pvData(0x%x)\n", eCasGrpId, ulCasSlotNum, pvData);
				hError = om_cas_mmi_UiDataProcEvent(message, eCasGrpId, ulCasSlotNum, pvData);
				if (hError != ERR_OK)
				{
					HxLOG_Error("Error! om_cas_mmi_SessionProcEvent() failed. \n");
				}
			}
			else
			{
				HxLOG_Error("Error! Invalid param! \n");
			}
			break;

		default:
			hBusResult = MESSAGE_PASS;
			break;


	}


	return hBusResult;

}



#define ___GLOBAL_FUNCTIONS___

HERROR	OM_CAS_MMI_SendCasEventDataToCasUiProc(DxCAS_GroupId_e eCasGrpId, HUINT32 ulSlotNum, Handle_t hAction, HINT32 nMessage, HINT32 p1, HINT32 p2, void *pData, HUINT32 ulSize)
{
	return om_cas_mmi_PostCasEventDataToCasUiProc(eCasGrpId, ulSlotNum, hAction, nMessage, p1, p2, pData, ulSize);
}

HERROR	OM_CAS_MMI_SendCasEventToCasUiProc(DxCAS_GroupId_e eCasGrpId, HUINT32	ulSlotNum, Handle_t hAction, HINT32 nMessage, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return om_cas_mmi_SendCasEventToCasUiProc(eCasGrpId, ulSlotNum, hAction, nMessage, p1, p2, p3);
}

HERROR	OM_CAS_MMI_RegisterCasUiProc(DxCAS_GroupId_e eCasGrpId, BUS_Callback_t fnCasUiProc)
{
	HERROR						hError = ERR_OK;
	OmCasMmiInstanceInfo_t	*pstInstInfo = NULL;

	HxLOG_Trace();

	pstInstInfo = om_cas_mmi_GetSubCasInstInfo(eCasGrpId);
	if(pstInstInfo != NULL)
	{
		HxLOG_Error("eCasGrpId(0x%x)  exist!\n", eCasGrpId);
		hError = ERR_FAIL;

		goto END_FUNC;
	}

	pstInstInfo = om_cas_mmi_AppendSubCasInst(eCasGrpId, fnCasUiProc);
	HxLOG_Info("eCasGrpId 0x%x, fnCasUiProc 0x%x => 0x%x\n",
		pstInstInfo->eCasGrpId, fnCasUiProc, pstInstInfo);

END_FUNC:
	return hError;
}


HERROR OM_CAS_MMI_Init(void)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	HxLOG_Trace();

	if ( VK_SEM_Create(&s_ulOmCasMmiSemId, "s_ulOmCasMmiSemId", VK_SUSPENDTYPE_PRIORITY) != VK_OK )
	{
		HxLOG_Error("VK_SEM_Create error! \n");
	}


	// Create event handler proc
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_cas_mmi, 0, 0, 0, 0);


	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr == ERR_OK)
	{
		//	-------   RPC Methods   -------	//
		hErr = HLIB_RPC_RegisterCall (nRpcHandle, (HCHAR *)RPC_OAPI_CAS_MMI_UiResponse, (HCHAR *)"iiibi", (HxRPC_Func_t)om_cas_mmi_UiResponse, NULL,
								(HCHAR *)"Send UI Response (User input type...)");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_CAS_MMI_UiResponse, hErr);
		}


		hErr = HLIB_RPC_RegisterCall (nRpcHandle, (HCHAR *)RPC_OAPI_CAS_MMI_CloseSession, (HCHAR *)"iiiii", (HxRPC_Func_t)om_cas_mmi_CloseSession, NULL,
								(HCHAR *)"Close session");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_CAS_MMI_CloseSession, hErr);
		}

		hErr = HLIB_RPC_RegisterCall (nRpcHandle, (HCHAR *)RPC_OAPI_CAS_MMI_OpenSession, (HCHAR *)"iiiiib", (HxRPC_Func_t)om_cas_mmi_OpenSession, NULL,
								(HCHAR *)"Open Session");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_CAS_MMI_OpenSession, hErr);
		}


		hErr = HLIB_RPC_RegisterCall (nRpcHandle, (HCHAR *)RPC_OAPI_CAS_MMI_ChangeSessionState, (HCHAR *)"iiiii", (HxRPC_Func_t)om_cas_mmi_ChangeSessionState, NULL,
								(HCHAR *)"Cahnge session state");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_CAS_MMI_ChangeSessionState, hErr);
		}


		hErr = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_MMI_onSessionStatusChanged, "iiiiii",
								(HCHAR *)"Session Status Changed\n"
								);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_MMI_onSessionStatusChanged, hErr);
		}

		hErr = HLIB_RPC_CreateNotification (nRpcHandle, (HCHAR *)RPC_OBAMA_CAS_MMI_onGetEventData, "iiiiiib",
								(HCHAR *)"Get Event Data\n"
								);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_MMI_onGetEventData, hErr);
		}







	}

	return ERR_OK;
}



