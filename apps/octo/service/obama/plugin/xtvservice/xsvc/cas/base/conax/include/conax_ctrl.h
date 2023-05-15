/*
@file				conax_ctrl.h
@brief				conax_ctrl.h

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2011 HUMAX Co., Ltd. \n
All rights reserved. \n
*/
#include <conax.h>
#ifndef CONAX_CTRL_H
#define CONAX_CTRL_H


/**************************************************************************************************/
#define ________Conax_ctrl_Private_Type_Define_____________________________________________
/**************************************************************************************************/

// Request type: Card와 통신해서 받는 정보...

typedef enum tagControlTypeNa
{
	eCACTRL_CX_GetVersionInfo	= 0,
	eCACTRL_CX_GetCurMatRatLevel,
	eCACTRL_CX_ChangeMaturityRating,
	eCACTRL_CX_ReqStatusInfo,
	eCACTRL_CX_GetUserText,
	eCACTRL_CX_GetCountryIndicator,
	eCACTRL_CX_GetSCState,
	eCACTRL_CX_ChangeScPin,
	eCACTRL_CX_SetUserTextDisplayed,
	eCACTRL_CX_SetScPinForTokenPpv,
	eCACTRL_CX_GetCachedPin,
	eCACTRL_CX_SetScPin,
	eCACTRL_CX_OperateCachedPin,
	eCACTRL_CX_SetMainAction,
	eCACTRL_CX_SetFingerPrintDisplayed,
	eCACTRL_CX_GetScResetReason,
	eCACTRL_CX_SetCxMenuDisplayed,
	eCACTRL_CX_SetPinMmiDisplayed,
	eCACTRL_CX_MAX,
}ControlTypeCx_t;

typedef union tagCx_InParam_t	Cx_InParam_t;
typedef union tagCx_OutParam_t	Cx_OutParam_t;


/**************************************************************************************************/
#define ________Conax_ctrl_Private_Structure_____________________________________________
/**************************************************************************************************/

//xmgr_cas_CxGetGetVersionInfo
typedef struct tagCX_GetVersionInfoInParam_t
{
	HINT32	lScSlot;
} CX_GetVersionInfoInParam_t;
typedef struct tagCX_GetVersionInfoOutParam_t
{
	CxVersionInfo_t* 						pstInfo;
} CX_GetVersionInfoOutParam_t;

//xmgr_cas_CxGetCurMatRatLevel
typedef struct tagCX_GetCurMatRatLevelInParam_t
{
	HINT32	lScSlot;
} CX_GetCurMatRatLevelInParam_t;
typedef struct tagCX_GetCurMatRatLevelOutParam_t
{
	HUINT8* pucCurMatRatLevel;
} CX_GetCurMatRatLevelOutParam_t;

//xmgr_cas_CxChangeMaturityRating
typedef struct tagCX_ChangeMaturityRatingInParam_t
{
	HINT32	lScSlot;
	HUINT8	ucMaturityRatLevel;
	HUINT8* pucPin;
} CX_ChangeMaturityRatingInParam_t;

//AP_CAS_CX_GetStatusInfo
typedef struct tagCX_GetStatusInfoInParam_t
{
	HINT32					lScSlot;
	CxStatusSelectType_t	eType;
	HUINT32					ulRef;
} CX_GetStatusInfoInParam_t;
typedef struct tagCX_GetStatusInfoOutParam_t
{
	CxStatusList_t* pstList;
} CX_GetStatusInfoOutParam_t;

//xmgr_cas_CxGetUserText
typedef struct tagCX_GetUserTextOutParam_t
{
	HUINT32* 			pulNumMsg;
	CxUserTextList_t* 	pstList;
} CX_GetUserTextOutParam_t;

//xmgr_cas_CxGetCountryIndicator
typedef struct tagCX_GetCountryIndicatorInParam_t
{
	HINT32	lScSlot;
} CX_GetCountryIndicatorInParam_t;
typedef struct tagCX_GetCountryIndicatorOutParam_t
{
	HINT32* lCountryIdx;
} CX_GetCountryIndicatorOutParam_t;

//xmgr_cas_CxGetSCState
typedef struct tagCX_GetSCStateInParam_t
{
	HINT32	lScSlot;
} CX_GetSCStateInParam_t;

//xmgr_cas_CxChangeScPin
typedef struct tagCX_ChangeScPinInParam_t
{
	HINT32	nScSlot;
	HUINT8* pucOldPin;
	HUINT8* pucNewPin;
} CX_ChangeScPinInParam_t;

typedef struct tagCX_SetUserTextDisplayedInParam_t
{
	HUINT8 ucSeqNo;
	CxUTDisplayCount_t ulOpt;
} CX_SetUserTextDisplayedInParam_t;

//eCACTRL_CX_GetCachedPin
typedef struct tagCX_GetCachedPinOutParam_t
{
	HUINT8 *pucPin;
} CX_GetCachedPinOutParam_t;

//xmgr_cas_CxSetScPin
typedef struct tagCX_SetScPinInParam_t
{
	Handle_t		hHandle;
	HUINT8* 		pucPin;
	CxUiMsgType_t	eMsgType;
} CX_SetScPinInParam_t;
typedef struct tagCX_SetScPinOutParam_t
{
	CxUiMsgType_t eReturnMsg;
} CX_SetScPinOutParam_t;

typedef struct tagCX_ControlCaPinOverrideInParam_t
{
	Handle_t	hAction;
}CX_ControlCaPinOverrideInParam_t;

typedef struct tagCX_SetMainActionInParam_t
{
	Handle_t	hMainAction;
	Handle_t	hPrevAction;
}CX_SetMainActionInParam_t;

typedef struct tagCX_GetScResetReasonInParam_t
{
	HINT32 nScSlot;
}CX_GetScResetReasonInParam_t;

typedef struct tagCX_GetScResetReasonOutParam_t
{
	CxScReset_Reason_t eResetReason;
}CX_GetScResetReasonOutParam_t;

typedef struct tagCX_SetCxDisplayedInParam_t
{
	HBOOL		bDisplayed;
}CX_SetCxDisplayedInParam_t;

HERROR CA_CX_RegisterCtrlList(void);


#endif		//#ifndef CONAX_CTRL_H


