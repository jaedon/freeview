/**
	@file     _xmgr_cas_cx.h
	@brief    coordinate repository of conax

	Description: Conax AP Util header file for Ray_k \n
	Module: Application(Layer), AP_CORE(module), CONAX \n
	Remarks : \n
	 Date			Modification							Name\n
	-----------	-------------------------------------	------------------\n
	 2009/08/18		init					 				jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_CAS_CX_INT_H__
#define	__XMGR_CAS_CX_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <conax.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	TEXT_SIZE64			64
#define	TEXT_SIZE128		128
#define	TEXT_SIZE256		256
#define	TEXT_SIZE512		512

#define CX_SC_SLOT_0					0
#define CX_SC_SLOT_1					1


typedef enum
{
	eCAS_CX_POPUP_MODE_NONE				= 0x00000000,
	eCAS_CX_POPUP_MODE_MSG 				= 0x00010000,	// System Msg Mode
	eCAS_CX_POPUP_MODE_CHECK 			= 0x00020000,	// Check & Info msg with Pin-code
	eCAS_CX_POPUP_MODE_INFO				= 0x00040000,	// Info Msg
	eCAS_CX_POPUP_MODE_USEREXIT			= 0x00080000,	// Waiting for user Action
	eCAS_CX_POPUP_MODE_TIMEOUT			= 0x00100000,	// Waiting for time out
	eCAS_CX_POPUP_MODE_FINGERPRINT		= 0x00200000,	// Finger Print
	eCAS_CX_POPUP_MODE_USERTEXT			= 0x00400000,	// User Text
} XmgrCas_CxUiPopupMode_e;

typedef enum
{
	eCxCasUiType_CasMenu = 0,
	eCxCasUiType_SystemMessage,
	eCxCasUiType_MailIcon,
	eCxCasUiType_MailMessage,
	eCxCasUiType_FingerPrint,
	eCxCasUiType_PinDialog,
	eCxCasUiType_PpvDialog,
	eCxCasUiType_PurchaseList,
	eCxCasUiType_CheckMessage,

	eCxCasUiType_Max
} XmgrCas_CxUiType_e;

typedef enum
{
	eCasCXMenuItem_None,
	eCasCXMenuItem_Subscription,				// Subscription Status
	eCasCXMenuItem_Event,						// Event Status
	eCasCXMenuItem_Tokens,						// Tokens Status
	eCasCXMenuItem_Change_Pin,					// CA Pin Change
	eCasCXMenuItem_Maturity_Rating,				// Maturity Level
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	eCasCXMenuItem_Text_View,					// Message
#endif
	eCasCXMenuItem_About,						// CA System Information
	eCasCXMenuItem_Max
} XmgrCas_CxCaMenuItem_e;


typedef enum
{
	eCX_UI_SESSION_STATUS_Stopped,
	eCX_UI_SESSION_STATUS_StartReady,
	eCX_UI_SESSION_STATUS_Started,

	eCX_UI_SESSION_STATUS_Max
} XmgrCas_CxUiSessionStatus_e;

typedef enum
{
	eCxCas_ScRemoved,
	eCxCas_ScInserted,
	eCxCas_ScInitialized,

	eCxCas_ScStatusMax
} XmgrCas_CxScStatus_e;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      	Global Prototype	********************/
/*******************************************************************/
BUS_Result_t 	xproc_cas_CxUiMenu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t 	xproc_cas_CxUiPopUp(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);


//cas Alarm message
HERROR			xmgr_cas_CxGetErrorData(HINT32 lMsg, HUINT8 *pszMsg);

// cas Check Message
HERROR			xmgr_cas_CxGetCheckData(HINT32 lMsg, HUINT8 *pszMsg, HINT32 lData, HUINT32 ulMsgSize);

// cas Info Message
HERROR			xmgr_cas_CxGetInfoData(HINT32 lMsg, HUINT8 *pszMsg, HUINT32 ulMsgSize);


#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
// Cas Finger Print
BUS_Result_t	xproc_cas_CxFingerPrint(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif	// End of defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
// Cas Uset Text
BUS_Result_t	xproc_cas_CxUserText(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_CxMailMessage(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_CxMainIcon(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_CxMailMsgText(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif	//End of defined(CONFIG_MW_CAS_CONAX_TEXT)

BUS_Result_t	xproc_cas_CxSmartCardMenu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
//BUS_Result_t	xproc_cas_CxPinDialog(AP_PinDialogInstance_t *pstInstance, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Callback_t	xmgr_cas_CxGetUiProc(XmgrCas_CxUiType_e eUiType);
BUS_Result_t	xproc_cas_CxAbout(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_CxStatusList(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_CxMaturityRating(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#if 0
BUS_Result_t	xproc_cas_CxCheckPinDialog(AP_PinDialogInstance_t *pstInstance, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#if defined(CONFIG_DEBUG)
HCHAR *xmgr_cas_CxMessageDebugString(HINT32 message);
#endif

#endif /* !__XMGR_CAS_CX_INT_H__ */

