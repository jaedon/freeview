/*
 * (c) 2011-2015 Humax Co., Ltd.
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
/********************************************************************
 * $Workfile:   vmx_ui.h  $
 * Project: 	 Verimatrix CAS
 * Author:		ahn
 * Description:	API for Verimatrix
 *
 *                              Copyright (c) 2015 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef __INT_XSVC_VMXCOREUI_H__
#define __INT_XSVC_VMXCOREUI_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <stb_params.h>
#include "_svc_cas_mgr_common.h"

/************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#define VMX_FINGERPRINTING_LOCALDATA_MAX_NUM			(3)
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#define MAX_SC_NUMBER									(40)
#endif
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*
typedef enum
{
	eSvcXxx_Type
} xsvcXxx_Type_e;

typedef struct	
{
} xsvcXxx_Type_t;
*/

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)

typedef enum
{
	eVmx_Solution_Iptv_Hybrid = 0,
	eVmx_Solution_Dvb_Only
} Vmx_Solution_e;

typedef enum
{
	eVmxIptv_Display_FingerPrinting = 0,
	eVmxIptv_Display_OsdMessage,
	eVmxIptv_Display_ErrorMessage,
	eVmxIptv_Display_PinDialog,
	eVmxIptv_Display_Max
} VmxIptv_DisplayType_e;

typedef enum
{
	eVmxIptv_Mode_Always_On_Top = 0,
	eVmxIptv_Mode_User_Exit,
	eVmxIptv_Mode_Timeout,	
} VmxIptv_DisplayMode_e;

typedef enum
{
	// Decode Error Message
	eVmxIptv_ErrMsg_DecodeInit = 0,                   		// Initial state to detect first decode
	eVmxIptv_ErrMsg_DecodeOk,                     		// Decode proceeding
	eVmxIptv_ErrMsg_DecodeFailurePinRequired,    		// program Rating Level exceeds STB's
	eVmxIptv_ErrMsg_DecodeFailureKeyNotAvailable,     // Could be not authorized or network error getting key
	eVmxIptv_ErrMsg_DecodeFailureBlackout,             	// Service Class >= 0xFF0000 matched
	eVmxIptv_ErrMsg_DecodeFailureOther,                	// TBD
	eVmxIptv_ErrMsg_DecodeFailureTA,                		//  decryption fail due to trustzone applet error
	// Communication Error Message With VCAS
//	eVmxIptv_ErrMsg_Communication


	
	eVmxIptv_ErrMsg_Max,
} VmxIptv_ErrorMessage_e;

/* ---------- Finter Printing ---------- */
typedef enum
{
	eVmxIptv_Quadrant_C = 0,
	eVmxIptv_Quadrant_N,
	eVmxIptv_Quadrant_NE,	
	eVmxIptv_Quadrant_E,
	eVmxIptv_Quadrant_SE,
	eVmxIptv_Quadrant_S,
	eVmxIptv_Quadrant_SW,
	eVmxIptv_Quadrant_W,
	eVmxIptv_Quadrant_NW	
} VmxIptv_Quadrant_e;

typedef struct
{
	HUINT16 usType;
	HUINT8  ucDuration;
	VmxIptv_Quadrant_e  eQuadrant;
} VmxIptv_Fingerprinting_Format_t;

typedef struct
{
	HUINT16 usType;
	HUINT8  ucLength;
	HUINT8  *pData;
} VmxIptv_Fingerprinting_LocalData_t;

typedef struct
{
	HUINT16 usFormatLenght;
	VmxIptv_Fingerprinting_Format_t		stFormat;
	HUINT16 usLocalDataLength;
	VmxIptv_Fingerprinting_LocalData_t	stLocalData[VMX_FINGERPRINTING_LOCALDATA_MAX_NUM];
} VmxIptv_Fingerprinting_t;

typedef struct
{	
	HUINT16 usLenght;
	HUINT32 ulPeriod;
	HUINT8  *pucMsg;
} VmxIptv_PopupItem_t;

typedef struct
{
	VmxIptv_ErrorMessage_e eErrMsg;	
	HUINT32 ulChannel;
	HUINT32 ulValue;
	HUINT8  *pucMsg;
} VmxIptv_ErrorMsg_t;

typedef struct
{
	HUINT32		hAction;
	Vmx_Solution_e			eSolution;
	VmxIptv_DisplayType_e	eType;
	VmxIptv_DisplayMode_e   	eMode;

	union
	{
		VmxIptv_Fingerprinting_t		stFingrPrinting;	// finger print
		VmxIptv_PopupItem_t			stPopupItem;	// normal msg
		VmxIptv_ErrorMsg_t			stErrorMsg;
		// TODO : 필요시 아래에 추가함.
	} item;	
} VmxIptv_DisplayUi_t;
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
typedef enum
{
	eVmxBc_Bc_Osm = 0,
	eVmxBc_Bc_Pin,
} VmxBc_RequstType_e;

//>--------------------------- OSM  -----------------------------------------------
typedef enum
{
	eVmxBc_OsmType_Osd = 0,
	eVmxBc_OsmType_Trigger,
	eVmxBc_OsmType_Max
} VmxBc_OsmType_e;

typedef enum
{
	eVmxBc_OsmStatus_None = 0,	
	eVmxBc_OsmStatus_BuildWindow,
	eVmxBc_OsmStatus_DisplayWindow
} VmxBc_OsmStatus_e;

typedef enum
{
  	eVmxBc_TriggerType_Upgrade = 0,
	eVmxBc_TriggerType_Rescan,
	eVmxBc_TriggerType_Switch,
	eVmxBc_TriggerType_Watermarking,
	eVmxBc_TriggerType_PinNumberReset,	
	eVmxBc_TriggerType_CustomerBinding,	
	eVmxBc_TriggerType_Eas,
	eVmxBc_TriggerType_Max
} VmxBc_TriggerType_e;

typedef enum
{	
  	eVmxBc_UpgradeType_Forced = 0,
  	eVmxBc_UpgradeType_Now,
  	eVmxBc_UpgradeType_NowAfterUserConfirmation,
  	eVmxBc_UpgradeType_PowerDown,
  	eVmxBc_UpgradeType_PowerDownAfterUserConfirmation,
  	eVmxBc_UpgradeType_PowerUp,
  	eVmxBc_UpgradeType_PowerUpAfterUserConfirmation,
  	eVmxBc_UpgradeType_OnNextServiceChange,
  	eVmxBc_UpgradeType_OnNextNonDescramblingPeriod,  	
  	eVmxBc_UpgradeType_Max	
} VmxBc_UpgradeType_e;

typedef enum
{		
  	eVmxBc_RescanType_Now = 0,
  	eVmxBc_RescanType_NowAfterUserConfirmation,
  	eVmxBc_RescanType_PowerDown,
  	eVmxBc_RescanType_PowerDownAfterUserConfirmation,
  	eVmxBc_RescanType_PowerUp,
  	eVmxBc_RescanType_PowerUpAfterUserConfirmation,
  	eVmxBc_RescanType_OnNextServiceChange,
  	eVmxBc_RescanType_OnNextNonDescramblingPeriod,  	
  	eVmxBc_RescanType_Max	
} VmxBc_RescanType_e;

typedef enum
{		
  	eVmxBc_SwitchType_ServiceNow = 0,
  	eVmxBc_SwitchType_AtTime,
  	eVmxBc_SwitchType_ForTimePeriod,
  	eVmxBc_SwitchType_AtTimeForTimePeriod,  	
  	eVmxBc_SwitchType_IfServiceCannotBeDescrambled,
  	eVmxBc_SwitchType_UntilTimeReached,
  	eVmxBc_SwitchType_BackNow,
  	eVmxBc_SwitchType_BackInXSeconds,
  	eVmxBc_SwitchType_BackAtTime,
  	eVmxBc_SwitchType_Max	
} VmxBc_SwitchType_e;

typedef enum
{
	eVmxBc_DisplayWindow_RemoveByUser = 0,
	eVmxBc_DisplayWindow_NoRemoveByUser
} VmxBc_DisplayWindowMode_e;

typedef enum
{
	eVmxBc_DisplayWindowType_FX_NEXTSTB = 1,
	eVmxBc_DisplayWindowType_RM_NEXTSTB,
	eVmxBc_DisplayWindowType_FX_NEXTCA,
	eVmxBc_DisplayWindowType_RM_NEXTCA,
	eVmxBc_DisplayWindowType_FX_STILLCA,
	eVmxBc_DisplayWindowType_RM_STILLCA,
	eVmxBc_DisplayWindowType_FX_IMMED,
	eVmxBc_DisplayWindowType_RM_IMMED	
} VmxBc_DisplayWindowType_e;

typedef enum
{
	eVmxBc_Osm_X_Pos_Left = 0,
	eVmxBc_Osm_X_Pos_Center,		
	eVmxBc_Osm_X_Pos_Right
} VmxBc_Osm_X_Position_e;

typedef enum
{
	eVmxBc_Osm_Y_Pos_Upper = 0,	
	eVmxBc_Osm_Y_Pos_Center,
	eVmxBc_Osm_Y_Pos_Bottom
} VmxBc_Osm_Y_Position_e;

typedef enum
{
	eVmxBc_Osm_Text_Alignment_LeftAligned = 0,
	eVmxBc_Osm_Text_Alignment_RightAligned,
	eVmxBc_Osm_Text_Alignment_Centered,
	eVmxBc_Osm_Text_Alignment_Justified,	
} VmxBc_Osm_Text_Alignment_e;
//<--------------------------- OSM  -----------------------------------------------

typedef enum
{
	eVmxBc_Desc_Ok = 0,
	eVmxBc_Desc_Error,
	eVmxBc_Desc_NoECMs,
	eVmxBc_Desc_ClearOrForeignEs,
	eVmxBc_Desc_Preview,
	eVmxBc_Desc_Pairing,
	eVmxBc_Desc_MS,
	eVmxBc_Desc_NoCWs,
	eVmxBc_Desc_Init   // only for internal use
} VmxBc_DescState_e;

//>--------------------------- PIN -----------------------------------------------
typedef enum
{
 	eVmxBc_PinReason_PC = 0,							/* check for parantal control */
	eVmxBc_PinReason_IPPV = 1,							/* check for Impulse Pay Per View */
	eVmxBc_PinReason_PC_UsingNonSmartcardPin = 3,		/* check for parental control using non-smartcard pin */
	eVmxBc_PinReason_ResumingOfEvent = 4,				/* check for resuming of an event */
	eVmxBc_PinReason_SelectiingOfEvent= 5,				/* check for selecting an event */
	eVmxBc_PinReason_InternalSTBForSC= 0xFE,			/* 0xFF: internal STB for Smart Card*/
	eVmxBc_PinReason_InternalSTBForNSC= 0xFE			/* 0xFF: internal STB for Non-Smart Card*/	
} VmxBc_PinReason_e;

typedef enum
{
	eVmxBc_PinIndex_PC_PIN2 = 0,							/* PIN Index 0: Parental control PIN 2 */
	eVmxBc_PinIndex_IPPC_PC_PIN1,					/* PIN Index 1: IPPV/parental control PIN 1 */
	eVmxBc_PinIndex_STB_LOCK_PIN,					/* PIN Index 2: STB Lock PIN */
	eVmxBc_PinIndex_HOME_SHOPPING_PIN				/* PIN Index 3: Home shopping PIN */
} VmxBc_PinIndex_e;

typedef enum
{
	eVmxBc_PinType_Check = 0,
	eVmxBc_PinType_Order,
} VmxBc_PinType_e;
//<--------------------------- PIN -----------------------------------------------

typedef struct
{
	HUINT8 *pDescriptor;
	HUINT16 usLength;
} VmxBc_Descriptor_t;

typedef struct
{
	VmxBc_UpgradeType_e eType;
	VmxBc_Descriptor_t stDesc;
	HUINT32 Reserved;
} VmxBc_Upgrade_t;

typedef struct
{
	VmxBc_RescanType_e eType;
	HUINT32 Reserved;	
} VmxBc_Rescan_t;

typedef struct
{
	HUINT8 ucPinNumberBuf[4];
	HUINT32 Reserved;	
} VmxBc_PinNumberReset_t;

typedef struct
{
	VmxBc_SwitchType_e eType;
	HUINT32 Reserved;	
} VmxBc_Switch_t;

typedef struct
{
	HUINT8 	ucSCSerialNumStr[40]; // serial  number string을 그대로 display
	HUINT16	usDuration;
	HUINT16	x_frame;
	HUINT8  y_frame;
} VmxBc_Watermarking_t;

typedef struct
{
	HUINT32 Reserved;
} VmxBc_Eas_t;

typedef struct
{
	HUINT16 x;
	HUINT16 y;
	HUINT16 w;
	HUINT16 h;
} VmxBc_ScreenSize_t;

typedef struct
{
	VmxBc_Osm_X_Position_e		x_pos;
	VmxBc_Osm_Y_Position_e		y_pos;
	VmxBc_Osm_Text_Alignment_e 	text_alig;
} VmxBc_WindowMode_t;

typedef struct
{
	VmxBc_WindowMode_t 	stMode;
	VmxBc_ScreenSize_t	stScreen;
	HUINT8 ucBg;
	HUINT8 ucAlpha;
	HUINT8 ucFg;
} VmxBc_BuildWindow_t;

typedef struct
{
	VmxBc_DisplayWindowMode_e 	eMode;
	VmxBc_DisplayWindowType_e	eType;
	HUINT16 usDuration;
} VmxBc_DisplayWindow_t;

typedef struct
{
	HUINT8						ucGroup;	// trigger group
	HUINT8						ucID;		// trigger Id
	HUINT32						ucNumber[16];	// trigger number : 8 hex-characters
	
	VmxBc_TriggerType_e   		eTriggerType;
	union
	{
		VmxBc_Upgrade_t 			stUpgrade;
		VmxBc_Rescan_t			stRescan;
		VmxBc_Switch_t				stSwitch;
		VmxBc_PinNumberReset_t	stPinNumberReset;
		VmxBc_Watermarking_t		stWatermarking;
		VmxBc_Eas_t				stEas;
		HUINT8						usReserved[100];
	} item;
	
} VmxBc_Trigger_t;

typedef struct
{
	VmxBc_OsmStatus_e 	eOsmStatus;
	VmxBc_BuildWindow_t 	stBuildWindow;
	VmxBc_DisplayWindow_t  stDisplayWindow;	
	HUINT8 buf[255];
} VmxBc_OSD_t;

#if 1 // test
typedef struct
{		
	VmxBc_OsmType_e		eOsmType;	/* trigger or osm type 선택 */
	HBOOL					bIsWorking;	/* 현재 해당 osm에 대한 command가 동작중인지? */

	VmxBc_TriggerType_e   	eTriggerType; /* trigger가 선택된경우 선택된 trigger */
	VmxBc_Trigger_t 		stTrigger[eVmxBc_TriggerType_Max];

	VmxBc_OSD_t			stOSD;		/* osd가 선택된 경우 */
} VmxBc_Osm_t;
#else
typedef struct
{	
	VmxBc_OsmType_e		eOsmType;	
	VmxBc_OsmStatus_e 	eOsmStatus;
	VmxBc_BuildWindow_t 	stBuildWindow;
	VmxBc_DisplayWindow_t  stDisplayWindow;

	VmxBc_TriggerType_e   		eTriggerType;

	union
	{
		VmxBc_Upgrade_t 			stUpgrade;
		VmxBc_Rescan_t			stRescan;
		VmxBc_Switch_t				stSwitch;
		VmxBc_PinNumberReset_t	stPinNumberReset;
		VmxBc_Watermarking_t		stWatermarking;
		VmxBc_Eas_t				stEas;
		HUINT8						usReserved[100];
	} item;

	HUINT8 buf[255];
} VmxBc_Osm_t;
#endif
//>--------------------------- PIN -----------------------------------------------
typedef struct
{
	HUINT8 ucPin[4]; // pin number는 4개로 하자.
	HUINT8 ucPinLength;
} VmxBc_BCD_Pin_t;

typedef struct
{
	VmxBc_PinIndex_e ePinIndex;
	VmxBc_BCD_Pin_t stOldPin;
	VmxBc_BCD_Pin_t stNewPin;
} VmxBc_ChangePin_t;

typedef struct
{
	VmxBc_BCD_Pin_t stBcdPin;
	HUINT8 ucPinIndex;
	HUINT8 ucReason;
	HUINT8 ucServiceIdx;
	VmxBc_PinReason_e eReason;
} VmxBc_VerificationPin_t;

typedef struct
{
	HUINT8 bPinIndex;
	HUINT32 lEventId;
	HUINT8 bServiceIdx;
	VmxBc_PinReason_e eReason;
	HUINT8 Credit_Buf[256];
	HUINT8 Cost_Buf[256];
} VmxBc_ISCOrderPin_t;

typedef struct
{
	HUINT8 bPinIndex;
	HUINT8 bServiceIdx;
	VmxBc_PinReason_e eReason;
} VmxBc_ISCCheckPin_t;

typedef struct
{
	VmxBc_PinType_e		eType;
	VmxBc_ISCOrderPin_t	stOrderPin;
	VmxBc_ISCCheckPin_t	stCheckPin;
} VmxBc_ISC_Pin_t;

//<--------------------------- CHECK PIN, ORDER PIN -----------------------------------------------

typedef struct
{
	VmxBc_RequstType_e		eType;
	VmxBc_Osm_t				stOsm;
	VmxBc_ISC_Pin_t			stPin;
} VmxBc_Requst_t;
#endif
/*******************************************************************/
/********************      Global Variables     ********************/
/******************************************************************/


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
// HERROR xsvc_xxx_yyy_DoSomething( ... );

HERROR vmx_vr_ui_Init(void);
	
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
HERROR vmx_vr_iptvh_ui_Init(void);
void xsvc_vmx_core_ui_iptv_FingerPrintingCallback(void*pUserParam, int FingerPrintingdataLength, void*pFingerPrintingData, int localdataLength, void*pLocalData);
void xsvc_vmx_core_ui_iptv_OSDMessgaeCallback(void *pUserParam, unsigned int nDataSize, void *pOsdMessage);
void xsvc_vmx_core_ui_iptv_DecodeCallback(void *pUserParam, unsigned long lChannel,  eVmDecodeResult eReason);
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
void xsvc_cas_vmx_DebugPrintForRequest(VmxBc_Requst_t *pstRequest, HUINT8 *pMsg);
HERROR xsvc_cas_vmx_UpdateCasMessage( Handle_t hAction, CAS_MsgType_t eMsgType, CasGroupId_t eCasId, void *pMessage, CAS_ShowType_t eShowType );
HERROR xsvc_cas_vmx_SetPinVerificationToBC(VmxBc_VerificationPin_t *pVPin);

void vmx_ui_DebugPrintforRequest(VmxBc_Requst_t *pstRequest, HUINT8 *pMsg);
void vmx_vr_bc_ui_SetPinChangeToBC(VmxBc_ChangePin_t *pChPin);
HERROR vmx_vr_bc_ui_SetPinVerificationToBC(VmxBc_VerificationPin_t *pVPin);
HERROR vmx_vr_bc_ui_SetOsdBuildWindow(HUINT8*pabMsg, HINT16 wMode, HINT16 wX, HINT16 wY, HINT16 wW, HINT16 wH, HINT8 bBackground, HINT8 bAlpha, HINT8 bForeGround);
HERROR vmx_vr_bc_ui_SetOsdDisplayWindow(HUINT8 bDisplayMode, HUINT16 wDuration);
void vmx_vr_bc_ui_SetCheckPin(HUINT8 bPinIndex, HUINT8 bTextSelector, HUINT8 bServiceIdx);
void vmx_vr_bc_ui_SetOrderPin(HUINT8 bPinIndex, HUINT32 lPurse, HUINT32 lCost, HUINT16 wMult, HUINT16 wDiv, HUINT8 bLocation, HUINT8 bSign0, HUINT8 bSign1000, HUINT8 bCount, HUINT8 bLen, HUINT8 *abText, HUINT32 lEventId, HUINT8 bServiceIdx);
HERROR vmx_vr_bc_ui_SetDescramblingState(HUINT16 wIndex, HUINT16* pawStreamPid, void* penDescState, HUINT8 bServiceIdx);
#endif
#endif 	//#ifndef __INT_XSVC_VMXCOREUI_H__

