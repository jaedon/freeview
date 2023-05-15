/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_cas_mmi.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

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

#ifndef __OAPI_CAS_MMI_H__
#define __OAPI_CAS_MMI_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define RPC_OAPI_CAS_MMI_Init							"oapi.cas.mmi.Init"
//from application.
#define RPC_OAPI_CAS_MMI_UiResponse						"oapi.cas.mmi.UiResponse"
#define RPC_OAPI_CAS_MMI_OpenSession						"oapi.cas.mmi.OpenSession"
#define RPC_OAPI_CAS_MMI_CloseSession					"oapi.cas.mmi.CloseSession"
#define RPC_OAPI_CAS_MMI_ChangeSessionState				"oapi.cas.mmi.ChangeSessionState"

#define RPC_OBAMA_CAS_MMI_onSessionStatusChanged		"obama.cas.mmi.onSessionStatusChanged"
#define RPC_OBAMA_CAS_MMI_onGetEventData			"obama.cas.mmi.onGetEventData"





#define	OAPI_CASMMI_INPUT_NUMBER_MAX_LENGTH			(12)	// 10자리 : 4,294,967,295
#define	OAPI_CASMMI_DATA_LENGTH_LONG					(256)

// Nagra DAL test 중 3ms 단위로 들어오는 banner data를 놓치는 경우가 있어서 적용해둠.
// 기존에는 list에 쌓인 last data만 전달 해줌. 앞에 것들은 필요가 없으니...
#define	OAPI_CASMMI_NOTIFY_ALL_EVENT_DATA	1
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef	void	(*OxCasMmi_Notifier_t)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
typedef	void	(*OxCasMmi_NotifierForAppkit_t)(OxCasMmi_Notifier_t pfnCbFunc, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);


#if 0 //dlib_cas.h 이동.
typedef enum
{
	eOxCASMMI_EVENTTYPE_NONE,
	eOxCASMMI_EVENTTYPE_REQUESTSESSIONSTART,
	eOxCASMMI_EVENTTYPE_REQUESTSESSIONSTOP,
	eOxCASMMI_EVENTTYPE_SESSIONSTARTED,
	eOxCASMMI_EVENTTYPE_SESSIONSTOPPED,
	eOxCASMMI_EVENTTYPE_DISPLAYUI,

	eOxCASMMI_EVENTTYPE_MAX
} OxCasMmiSessionEventType_e;
#endif

typedef enum
{
	eOxCASMMI_SESSIONSTATUS_STOPPED,
	eOxCASMMI_SESSIONSTATUS_STARTREADY,
	eOxCASMMI_SESSIONSTATUS_STARTED,

	eOxCASMMI_SESSIONSTATUS_MAX
} OxCasMmiSessionStatus_e;



typedef enum
{
	eOxCASMMI_MENUGROUP_NONE,
	eOxCASMMI_MENUGROUP_1, //IRDETO: SmartCardStatus,
	eOxCASMMI_MENUGROUP_2, //IRDETO: SoftCellServiceStatus,
	eOxCASMMI_MENUGROUP_3,//IRDETO: LoaderStatus,
	eOxCASMMI_MENUGROUP_4,//IRDETO: Components,
	eOxCASMMI_MENUGROUP_5,//IRDETO: MailMessages,

	eOxCAS_MMIMENUGROUP_MAX
} OxCasMmiMenuGroupId_e;



typedef	struct
{
	DxCAS_GroupId_e				eModule;
	HUINT32						nSlot;
} OxCasMmiCasInstance_t;

#if 0 //dlib_cas.h 이동.
typedef struct
{
	OxCasMmiSessionEventType_e			eventType;
	DxCAS_MmiSessionType_e			sessionType;
	DxCAS_GroupId_e					eModule;
	HUINT32							nSlot;
	HCHAR	 						*pszEncodedUiEvent;
} OxCasMmiEvent_t;
#endif

typedef enum
{
	eOxCASMMI_INPUTTYPE_NONE,
	eOxCASMMI_INPUTTYPE_BUTTON,
	eOxCASMMI_INPUTTYPE_NUMBER,
	eOxCASMMI_INPUTTYPE_STRING,
	eOxCASMMI_INPUTTYPE_SELECTION,
	eOxCASMMI_INPUTTYPE_ETC,

	eOxCASMMI_INPUTTYPE_MAX
} OxCasMmiInputType_e;

typedef enum
{
	eOxCASMMI_BUTTON_OK,
	eOxCASMMI_BUTTON_YES,
	eOxCASMMI_BUTTON_NO,
	eOxCASMMI_BUTTON_CANCEL,
	eOxCASMMI_BUTTON_1st,
	eOxCASMMI_BUTTON_2nd,
	eOxCASMMI_BUTTON_3rd,

	eOxCASMMI_BUTTON_MAX
} OxCasMmiInputButtonType_e;

typedef enum
{
	eOxCASMMI_ETC_PREVIOUS,
	eOxCASMMI_ETC_EXIT,

	eOxCASMMI_ETC_MAX
} OxCasMmiNotifyEtcType_e;

typedef struct
{
	OxCasMmiInputButtonType_e	eBtnType;
} OxCasMmiNotifyButton_t;

typedef struct
{
	HUINT32 					ulNumber;		// max 4294967295
	HUINT32 					ulNumberLength;	// Length of pszNumber
	HCHAR					pszNumber[OAPI_CASMMI_INPUT_NUMBER_MAX_LENGTH];
} OxCasMmiNotifyNumber_t;

typedef struct
{
	HUINT32 					ulStringLength;
	HCHAR					szString[OAPI_CASMMI_DATA_LENGTH_LONG];
} OxCasMmiNotifyString_t;

typedef struct
{
	HUINT32 					ulItemIdx;
	HUINT32 					ulDataLength;						// Item에 달려있는 data length
	HCHAR						aucData[OAPI_CASMMI_DATA_LENGTH_LONG];	// Item에 달려있는 data
} OxCasMmiNotifySelection_t;

typedef struct
{
	OxCasMmiNotifyEtcType_e	eEtcType;
} OxCasMmiNotifyEtc_t;

typedef	struct
{
	OxCasMmiInputType_e		eInputType;

	union
	{
		OxCasMmiNotifyButton_t		stBtn;
		OxCasMmiNotifyNumber_t		stNumber;
		OxCasMmiNotifyString_t		stString;
		OxCasMmiNotifySelection_t	stSelection;
		OxCasMmiNotifyEtc_t			stEtc;
	} inputData;
} OxCasMmiInput_t;



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
void		OAPI_CAS_MMI_Init(OxCasMmi_NotifierForAppkit_t pfnAppkitNotifier);
void		OAPI_CAS_MMI_RemoveEvtInfo(DxCAS_MMI_Event_t *pstEvent);
void		OAPI_CAS_MMI_GetEvtInfo(DxCAS_MMI_Event_t **ppstEvent);
void		OAPI_CAS_MMI_RemoveUiEvent(DxCAS_MmiSessionType_e eSessionType, DxCAS_MMI_Event_t *pstEvent);
void		OAPI_CAS_MMI_GetUiEvent(DxCAS_MmiSessionType_e eSession, Handle_t hSession, DxCAS_MMI_Event_t **ppstEvent);

void		OAPI_CAS_MMI_SendUiResponse(DxCAS_GroupId_e eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSession, Handle_t hSession, const HCHAR *pzInputData);
void		OAPI_CAS_MMI_CloseSession(DxCAS_GroupId_e	eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSession, Handle_t hSession);
Handle_t		OAPI_CAS_MMI_OpenSession(DxCAS_GroupId_e	eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSession, const char *pcParamData);
void		OAPI_CAS_MMI_UnRegisterSessionNotifier(DxCAS_MmiSessionType_e eSession);
void		OAPI_CAS_MMI_RegisterSessionNotifier(DxCAS_MmiSessionType_e eSession, OxCasMmi_Notifier_t pfnObjectHandler);
void		OAPI_CAS_MMI_UnRegisterEventNotifier(HUINT32 objId);



#endif // __OAPI_CAS_MMI_H__
