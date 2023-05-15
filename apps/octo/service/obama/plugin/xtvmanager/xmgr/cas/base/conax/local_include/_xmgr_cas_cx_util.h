/**
	@file     _xmgr_cas_cx_util.h
	@brief    coordinate repository of conax util

	Description: Conax AP Util header file for Ray_k \n
	Module: Application(Layer), AP_CORE(module), CONAX \n
	Remarks : \n
	 Date			Modification							Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/24		init					 				jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __XMGR_CAS_CX_UTIL_INT_H__
#define __XMGR_CAS_CX_UTIL_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <conax.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	XMGR_CAS_CX_PARAM_UNUSED(x) ((void)x)
#define	XMGR_CAS_CX_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4)		\
	{													\
		(void)arg1;										\
		(void)arg2;										\
		(void)arg3;										\
		(void)arg4;				 						\
	}


#define	CAS_CX_SECONDS				1000
#define ALERT_PIN_CODE_ID								102

#define	TEXT_SIZE64			64
#define	TEXT_SIZE128		128
#define	TEXT_SIZE256		256
#define	TEXT_SIZE512		512

#define	MAX_ELEMENT_TEXT_LENGTH				256
//#define	MGR_CAS_STR_LENGTH_LONG			256

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
	LIST OF ITU-T RECOMMENDATION E.164 ASSIGNED COUNTRY CODES  - No. 835 - 1.V.200
	에 정의된 내용 중에 CONAX에서 제공하는 Code만 정의
*/
typedef enum
{
	eITU_T_CountryCode_DEN		= 0x002e,
	eITU_T_CountryCode_FIN		= 0x0166,
	eITU_T_CountryCode_FRA		= 0x0021,
	eITU_T_CountryCode_GER		= 0x0031,
	eITU_T_CountryCode_ITA		= 0x0027,
	eITU_T_CountryCode_NOR		= 0x002f,
	eITU_T_CountryCode_SPA		= 0x0022,
	eITU_T_CountryCode_SWE		= 0x002d,
	eITU_T_CountryCode_UK		= 0x002c,
} XmgrCas_CxItuTE164CountryCode_e;


/*
	Check Pin-code Enter Type
*/
typedef enum
{
	eCX_PinCheckEnter_AcceptView = 0,
	eCX_PinCheckEnter_MR,
	eCX_PinCheckEnter_MR_CHG,
	eCX_PinCheckEnter_TokenDebit,
	eCX_PinCheckEnter_Max,
} XmgrCas_CxPinCheckEnterCaInDlg_e;


/*
	Check Pin-code Step
*/
typedef enum
{
	eCX_PinCheckStep_InDlg_InputPincode = 0,
	eCX_PinCheckStep_InDlg_WrongPincode,
	eCX_PinCheckStep_InDlg_MAX,
} XmgrCas_CxPinCheckStepInDlg_e;


/*
	Change Pin-code Step
*/
typedef enum
{
	eCX_PinChangeStep_InDlg_InputCurPincode = 0,
	eCX_PinChangeStep_InDlg_InputNewPincode,
	eCX_PinChangeStep_InDlg_VerifyNewPincode,
	eCX_PinChangeStep_InDlg_MAX,
} XmgrCas_CxPinChangeStepInDlg_e;


/*
	Change Pin-code Error
*/
typedef enum
{
	eCX_PinChangeError_Normal = 0,
	eCX_PinChangeError_InputCodeAreNotMatch,
	eCX_PinChangeError_ConfirmationAreNotMatch,
	eCX_PinChangeError_ChangeFailed,
	eCX_PinChangeError_MAX,
} XmgrCas_CxPinChangeError_e;

typedef enum
{
#if defined(CONFIG_CX_ORDERING)
	eCasCxMenu_Ordering,					//Product Ordering, Modem Ordering
#endif
	eCasCxMenu_Subscription,				// Subscription Status
	eCasCxMenu_Event,						// Event Status
	eCasCxMenu_Tokens,						// Tokens Status
	eCasCxMenu_Change_Pin,					// CA Pin Change
#if defined(CONFIG_CX_SIGNATURE)
	eCasCxMenu_Change_Signature_Pin,		//
#endif
	eCasCxMenu_Maturity_Rating,				// Maturity Level
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	eCasCxMenu_Text_View,					// Message
#endif
	eCasCxMenu_About,						// CA System Information
	eCasCxMenu_Max
} XmgrCas_CxMenu_e;
/*******************************************************************/
/********************      Struture        *************************/
/*******************************************************************/

#if 0
/**
 *	@defgroup	LeafSubItem
 	Leaf sub item structure
 */
typedef struct tagSettingsLeafSubItem
{
	HUINT32			id;
	OSD_Rect_t		rect;
	HBOOL			bDisable;							///< if TRUE, is disable

	HUINT32			mainItemId;
	HUINT8			mainItemText[MAX_ELEMENT_TEXT_LENGTH];

													///< itemTextList value
	HINT32			subItemValue;						///< sub item value
	HUINT8			itemString[32];						///< sub item text
													///< for selector, input obj
	HUINT32			numOfItemTextList;					///< total num of sub item
	HUINT32			idxOfTextList;						///< index of sub item
	HUINT8			**itemTextList;						///< list of sub item

	/* to launch */
	HUINT8			subItemText[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8			appName[MAX_ELEMENT_TEXT_LENGTH];
	BUS_Callback_t	childAppToLaunch;
	Handle_t		hAction;
	HUINT32			appParam1;
	HUINT32			appParam2;
	HUINT32			appParam3;
} XmgrCas_CxSettingsLeafSubItem_t;

typedef struct tagSettingsLeafContext
{
	/**@breif Public value */
	HUINT32							ulCurFocus;
	HUINT32							ulNumOfSubItem, ulMaxNumOfSubItem;
	XmgrCas_CxSettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8							ucTitle[MAX_ELEMENT_TEXT_LENGTH];
} XmgrCas_CxSettingsLeafContext_t;
#endif

typedef struct tagSettingsNaviSubItem
{
	HINT32	nTitleLocId;
	HUINT8	title[MAX_ELEMENT_TEXT_LENGTH];
} XmgrCas_CxSettingsNaviSubItem_t;

typedef struct tagSettingsNaviMainItem
{
	/* main item title */
	HUINT32		mainBtnId;
	HUINT32		mainBtnIdx;
	HINT32		nTitleLocId;
	HUINT8		title[MAX_ELEMENT_TEXT_LENGTH];
	HBOOL		bDisable;
	HBOOL		bHide;

	HINT32		subItemValue;						///< sub item value
	HUINT8		subItemString[32];						///< sub item text

	HUINT32		ulNumOfSubItem, ulMaxOfSubItem;
	XmgrCas_CxSettingsNaviSubItem_t		*subItem;

	HUINT8				appName[MAX_ELEMENT_TEXT_LENGTH];
//	BUS_Callback_t		appToLaunch;
	Handle_t			hAction;
	HUINT32				appParam1;
	HUINT32				appParam2;
	HUINT32				appParam3;
} XmgrCas_CxSettingsNaviMainItem_t;

typedef struct tagSettingsContext
{
	HINT32			nCurFocus;
	HUINT32			ulNumOfMainItem, ulMaxOfMainItem;
	HINT32			nDefaultFocusIdx;

	HUINT8			title[MAX_ELEMENT_TEXT_LENGTH];
//	Setting_ConvertObjIdToItemIdx	pConvFunc;
	XmgrCas_CxSettingsNaviMainItem_t	*mainItems;

	HINT32			lToUpper;
#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
	HBOOL			bDraw7Seg;
#endif
} XmgrCas_CxSettingsNaviContext_t;


/*******************************************************************/
/********************      	Global Prototype	********************/
/*******************************************************************/
HBOOL			xmgr_cas_CxGetPowerSavingStandby(void);
HBOOL			xmgr_cas_CxGetRunningInStandby(void);

HERROR			xmgr_cas_CxCheckSmartCardPin (CxUiMsgType_t	eUiMsgType, Handle_t hSvcHandle, HUINT8* szPincode);
HERROR			xmgr_cas_CxChangeSmartCardPin (HUINT8* szOldPincode, HUINT8* szNewPincode);
HERROR			xmgr_cas_CxPinCodeCheckAndAction (CxUiMsgType_t eUiMsgType, Handle_t hSvcHandle);

HUINT32			xmgr_cas_CxCalcDigit (HUINT8 ucNoDigit, HUINT32 numData);
void			xmgr_cas_CxGetWordFromString (HUINT8 *pszDest, HUINT8 *pszSrc, HUINT8 nIndex);
HERROR 			xmgr_cas_CxGetMonthString (HUINT8 ucMonth, HUINT8 *pszMonthString64);
HERROR			xmgr_cas_CxGetMatutiryRateStr (HUINT8 *pszStr, HUINT8 ucCurMatRatLevel);
HERROR			xmgr_cas_CxGetMatutiryRateLevelStr (HUINT8 *pszStr, HUINT8 ucCurMatRatLevel);

// MMI - MENU
HERROR			xmgr_cas_CxGetVersionInfo (HINT32 lScSlot, CxVersionInfo_t *pstInfo);
HERROR			xmgr_cas_CxGetCurMatRatLevel (HINT32 lScSlot, HUINT8 *pucCurMatRatLevel);
HERROR			xmgr_cas_CxChangeMaturityRating (HINT32 lScSlot, HUINT8 ucMaturityRatLevel, HUINT8 *pucPin);
CxStatusList_t *xmgr_cas_CxReqStatusInfo (HINT32 lScSlot, CxStatusSelectType_t eType, HUINT32 ulRef);
HERROR			xmgr_cas_CxGetUserText (HUINT32 *pulNumMsg, CxUserTextList_t *pstList);
HERROR			xmgr_cas_CxGetCountryIndicator (HINT32 lScSlot, HINT32 *lCountryIdx);
HERROR			xmgr_cas_CxGetSCState (HINT32 lScSlot);
HERROR			xmgr_cas_CxOperateCachedPin(HUINT8* pucInputPin);
HERROR			xmgr_cas_CxSetUserTextDisplayed(HUINT8 ucSeqNo, CxUTDisplayCount_t ulOpt);
HERROR			xmgr_cas_CxSetFingerPrintDisplayed(void);
HERROR			xmgr_cas_CxGetScResetReason(HINT32 nScSlot, CxScReset_Reason_t *peResetReason);

/* Command msg Process */
void			xmgr_cas_CxCMD(HUINT8 *pszCmd, HUINT8 *pszMsgType, HUINT8 *pszmsg1, HUINT8 *pszmsg2);

#if defined(CONFIG_DEBUG)
HUINT8			*xmgr_cas_CxGetMsgTypeStr(CxUiMsgType_t eMsg);
#endif

#endif /* __XMGR_CAS_CX_UTIL_INT_H__ */

/* end of file */

