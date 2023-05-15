/**
	@file     mgr_cas.h
	@brief    file_name & simple comment.

	Description: \n
	Module:  \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/03/27		initiated					chmin\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MGR_CAS_H__
#define	__MGR_CAS_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <svc_cas.h>
#include <mgr_output.h>
#include <mgr_common.h>
#include <mgr_pg.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define	MGR_CAS_STR_LENGTH_SHORT		32
#define	MGR_CAS_STR_LENGTH_NORMAL		64
#define	MGR_CAS_STR_LENGTH_LONG			1024


#define	MGR_CAS_DATA_LENGTH_NORMAL		64
#define	MGR_CAS_DATA_LENGTH_LONG		256

#define	MGR_CAS_ITEM_MAX_SELECTION		200

#define	MGR_CAS_MAIL_RECEIVED_DATE_LENGTH			10
#define	MGR_CAS_MAIL_RECEIVED_TIME_LENGTH		10

#define MGR_CAS_ALLOWED_SCRAMBLE_TIME	10		// 10 second

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	eMEVT_CAS = eMEVT_CAS_START,
	eMEVT_CAS_OK,
	eMEVT_CAS_FAIL,
	eMEVT_CAS_CMD,
	eMEVT_CAS_CHECK,

	/* MMI events */
	eMEVT_CAS_MMI_OPEN,
	eMEVT_CAS_MMI_CLOSE,
	eMEVT_CAS_MMI_TEXT,
	eMEVT_CAS_MMI_ENQ,
	eMEVT_CAS_MMI_MENU,
	eMEVT_CAS_MMI_LIST,

	/* cam state event */
	eMEVT_CAS_CAM_REMOVED,
	eMEVT_CAS_CAM_INSERTED,
	eMEVT_CAS_CAM_INITIALIZED,

	/* ci plus host tune event */
	eMEVT_CAS_CAM_TUNE,

	/* ci plus */
	eMEVT_CAS_CIPLUS_CC_REPORT,
	eMEVT_CAS_CIPLUS_UPGRADE,

	/* ci plus operator profile */
	eMEVT_CAS_CIPLUS_OPERATOR_PREPARE_REQ,
	eMEVT_CAS_CIPLUS_OPERATOR_TUNE,
	eMEVT_CAS_CIPLUS_OPERATOR_NIT,

	/* ppv event */
	eMEVT_CAS_PPV_REQUEST,
	eMEVT_CAS_PPV_REPLY,	//manager --> service

	/* pincode action event */
	eMEVT_CAS_PINCODE_REQUEST,
	eMEVT_CAS_PINCODE_REPLY,	 //manager --> service

	/* Userdata Action Event */
	eMEVT_CAS_UDATAREAD_REQUEST,
	eMEVT_CAS_UDATAREAD_REPLY,	 //manager --> service

	eMEVT_CAS_SYSINFO_UPDATED,
	eMEVT_CAS_UPDATE_SMARTCARD_INFO,


	eMEVT_CAS_SC_REMOVED,
	eMEVT_CAS_SC_INSERTED,

	// DRM 명령을 위한 CAS Message
	eMEVT_CAS_DRM_SIGNALLED,
	eMEVT_CAS_DRM_UNKNOWN,
	eMEVT_CAS_DRM_NONE,

	// ----------------------------------------
	// Digital MacroVision을 위한 CAS Message
	eMEVT_CAS_DMV_SIGNALLED,
	eMEVT_CAS_DMV_UNKNOWN,
	eMEVT_CAS_DMV_NONE,



	eMEVT_CAS_CLOSE,

	eMEVT_CAS_CASUI,
	/* - Description :
	   - Parameters Usage :
		   handle : N.A.
		   p1 : N.A.
		   p2 : N.A.
		   p3 : 0 */
	eOEVT_CAS_CASUI_APP_OPEN_SESSION,

	/* - Description :
	   - Parameters Usage :
		   handle : N.A.
		   p1 : N.A.
		   p2 : N.A.
		   p3 : 0 */
	eOEVT_CAS_CASUI_APP_CLOSE_SESSION,

	/* - Description :
	   - Parameters Usage :
		   handle : N.A.
		   p1 : N.A.
		   p2 : N.A.
		   p3 : 0 */
	eOEVT_CAS_CASUI_APP_USERINPUTDATA,

	/* - Description :
	   - Parameters Usage :
		   handle : N.A.
		   p1 : N.A.
		   p2 : N.A.
		   p3 : 0 */
	eMEVT_CAS_ACTION_PREPARE_TUNE_FAIL,

	/* - Description :
	   - Parameters Usage :
		   handle : N.A.
		   p1 : N.A.
		   p2 : N.A.
		   p3 : 0 */
	eMEVT_CAS_NOTIFICATION,

	eMEVT_CAS_UPDATE_KEYINFO,

	eMEVT_CAS_PRM_PVR_UPDATE_KEYINFO,

	eMEVT_CAS_UPDATE_STATUS,

	eMEVT_CAS_ACTION_TUNE_REQUEST,
	eMEVT_CAS_ACTION_FORCED_TUNE,
	eMEVT_CAS_ACTION_RESERVED_TUNE,
	eMEVT_CAS_ACTION_UPDATE_START,
	eMEVT_CAS_ACTION_UPDATE_STOP,

	eMEVT_CASUI_REQ_OPEN_SESSION, //ok
	eMEVT_CASUI_REQ_CLOSE_SESSION, // ok
	eMEVT_CASUI_SESSION_OPENED,  // ok

	eMEVT_CASUI_TYPE_BANNER_DATA,
	eMEVT_CASUI_TYPE_TEXTBOX_DATA,
	eMEVT_CASUI_TYPE_MAIL_DATA,
	eMEVT_CASUI_TYPE_MAILLIST_DATA,
	eMEVT_CASUI_TYPE_MENULIST_DATA,
	eMEVT_CASUI_TYPE_GRID_DATA,
	eMEVT_CASUI_TYPE_INPUT_DATA,
	eMEVT_CASUI_TYPE_INPUT_EX_DATA,
	eMEVT_CASUI_TYPE_CLOSE, // 곧 제거될 event임... 사용하지 마세요!! OCTO 쪽에서 Webappl을 닫고 싶을 때 이 Event 호출 하도록... 원래는 각 UI마다 session 열고 닫으면서 관리가 되어야 하는 것인데, HMS1000s Phase 1에서 맞추기가 어려워 임시 처리 함. (kkkim)

//xmgr_cas.h 로 옮겨가야 하나, 기존 CI 관련 EVT 들도 (eSEVT_CAS_CAM_INSERTED) 등도 모두 mgr_cas.h 에서 받아서 처리하고 있는 관계로...
	eMEVT_SYSCTL_CI_CAM_STATUS,
	eMEVT_SYSCTL_CI_CAM_STATUS_EXTRACTED,
	eMEVT_SYSCTL_CI_CAM_STATUS_INSERTED,
	eMEVT_SYSCTL_CI_CAM_STATUS_INITIALIZED,

	// evnet by CONAX CAS
	eMEVT_SYSCTL_CONAX_PLAYBACK_ERROR,

	eMEVT_CASPRM_NOVA_TRANSPORTSESSIONINFO_NOTIFIER,
	eMEVT_CASPRM_NOVA_LICENSE_NOTIFIER,
	eMEVT_CASPRM_NOVA_KEYHANDLER_NOTIFIER,
	eMEVT_CASPRM_NOVA_PLATFORM_USAGERULES_NOTIFIER,
	eMEVT_CASPRM_NOVA_SERVICE_USAGERULES_NOTIFIER,

	eMEVT_CAS_END

}; /* Message to GWM */


typedef enum
{
	eMgrCasMenuGroup_None,
	eMgrCasMenuGroup_1, //IRDETO: SmartCardStatus,
	eMgrCasMenuGroup_2, //IRDETO: SoftCellServiceStatus,
	eMgrCasMenuGroup_3,//IRDETO: LoaderStatus,
	eMgrCasMenuGroup_4,//IRDETO: Components,
	eMgrCasMenuGroup_5,//IRDETO: MailMessages,
	eMgrCasMenuGroup_MAX
} OxMgrCasMenuGroupId_e;


typedef enum
{
	eMgrCasInputType_Button = 0,
	eMgrCasInputType_Number,
	eMgrCasInputType_String,
	eMgrCasInputType_Selection,
	eMgrCasInputType_Etc,
	eMgrCasInputType_MAX,
} OxMgrCasInputType_e;

typedef enum
{
	eMgrCasUiType_Banner = 0,
	eMgrCasUiType_TextBox,
	eMgrCasUiType_MailMessage,
	eMgrCasUiType_MailMessageList,
	eMgrCasUiType_MenuList,
	eMgrCasUiType_Grid,
	eMgrCasUiType_InputDialog,
	eMgrCasUiType_InputDialog_Ex,
	eMgrCasUiType_MAX
} OxMgrCasUiType_e;


// UI attributes
typedef enum
{
	eMgrCasUiAttr_Flashing 			= 0x00000001,
	eMgrCasUiAttr_Scrollable		= 0x00000002,
	eMgrCasUiAttr_Timeout			= 0x00000004,
	eMgrCasUiAttr_SetPosition		= 0x00000008,
	eMgrCasUiAttr_RandomPosition	= 0x00000010,
	eMgrCasUiAttr_SetSize			= 0x00000020,
	eMgrCasUiAttr_SetFgColor		= 0x00000040,
	eMgrCasUiAttr_SetBgColor		= 0x00000080,
	eMgrCasUiAttr_Title				= 0x00000100,
	eMgrCasUiAttr_Subtitle 			= 0x00000200,
	eMgrCasUiAttr_Bottom			= 0x00000400,
	eMgrCasUiAttr_Multiline			= 0x00000800,
	eMgrCasUiAttr_HiddenInput		= 0x00001000,
	eMgrCasUiAttr_ProgressBar		= 0x00002000,
	eMgrCasUiAttr_InitValue			= 0x00004000,
	eMgrCasUiAttr_Selectable		= 0x00008000,
	eMgrCasUiAttr_IgnoreKey			= 0x00010000,
	eMgrCasUiAttr_AlwaysOnTop		= 0x00020000,
	eMgrCasUiAttr_Highlight			= 0x00040000,
	eMgrCasUiAttr_SetPrintTime		= 0x00080000,
	eMgrCasUiAttr_FingerPrint		= 0x00100000,
	eMgrCasUiAttr_Tenable			= 0x00200000,	// Timeout될 때까지 계속 유지되는 UI (Service 전환 등에 의해 없어지지 않는 UI)
} OxMgrCasUIAttribute_e;

// For the eMgrCasUiAttr_SetPosition
typedef enum
{
	eMgrCasUiPosition_Center		= 0,	// eMgrCasUiAttr_SetPosition이 attr 에 설정되지 않으면 center에 UI를 위치하는 것을 default로 한다.
	eMgrCasUiPosition_Top,
	eMgrCasUiPosition_Bottom,
	eMgrCasUiPosition_Coordinate, 				// 좌표로 position을 지정해 줘야 할 경우 설계 및 구현 필요

	eMgrCasUiPosition_Max
} OxMgrCasUiPosition_e;

// List Item Attributes
typedef enum
{
	eMgrCasUiAttr_Disabled 			= 0x00000001,
	eMgrCasUiAttr_Hide 				= 0x00000002,
	eMgrCasUiAttr_Selected 			= 0x00000004,
} OxMgrCasListItemAttribute_e;

typedef enum
{
	eMgrCasUiItemType_Str 			= 0x00000001,
	eMgrCasUiItemType_Bin 			= 0x00000002,
} OxMgrCasListItemType_e;

typedef struct
{
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulCasSlotNum;
	OxMgrCasMenuGroupId_e				eCasMenuGroupId;
	Handle_t							hSession;
	HUINT32							ulUiHandle;
} OxMgrCasSessionEvent_t;

#define ______________INPUT_UI________________
// Input : Button
typedef enum
{
	eMgrCasBtn_OK		= 0x00000001,
	eMgrCasBtn_YES 		= 0x00000002,
	eMgrCasBtn_NO		= 0x00000004,
	eMgrCasBtn_CANCEL	= 0x00000008,
	eMgrCasBtn_1st 		= 0x00000010,
	eMgrCasBtn_2nd 		= 0x00000020,
	eMgrCasBtn_3rd 		= 0x00000040
} OxMgrCasInputButtonType_e;

#define MGR_CASINPUT_MAX_CUSTOM_BUTTON_NUMBER	5
typedef struct
{
	OxMgrCasUIAttribute_e		ulAttr;
	OxMgrCasInputButtonType_e	eMgrCasBtnType;
	HCHAR						aszButtonName[MGR_CASINPUT_MAX_CUSTOM_BUTTON_NUMBER][MGR_CAS_STR_LENGTH_SHORT];
} OxMgrCasUiInputButton_t;

// Input : Number
#define	MGR_CASINPUT_NUMBER_MAX_LENGTH			25
typedef struct
{
	OxMgrCasUIAttribute_e		ulAttr;
	HUINT32 					ulNumber;		// max 4294967295
	HUINT32 					ulNumberLength;	// Length of pszNumber
	HCHAR						pszNumber[MGR_CASINPUT_NUMBER_MAX_LENGTH];
} OxMgrCasUiInputNumber_t;

// Input : String
typedef struct
{
	OxMgrCasUIAttribute_e		ulAttr;
	HUINT32 					ulStringLength;
	HCHAR						szString[MGR_CAS_STR_LENGTH_LONG];
} OxMgrCasUiInputString_t;

// Input : Selection
// Menu List 등 Item을 선택할 수 있는 UI에서 어떤 Item을 선택했는지 알려주는 메시지..
typedef struct
{
	OxMgrCasUIAttribute_e		ulAttr;
	HUINT32 					ulItemIdx;
	HUINT32 					ulDataLength;						// Item에 달려있는 data length
	HCHAR						aucData[MGR_CAS_STR_LENGTH_SHORT];	// Item에 달려있는 data
} OxMgrCasUiInputSelection_t;

typedef union
{
	OxMgrCasUiInputButton_t		stBtn;
	OxMgrCasUiInputNumber_t		stNumber;
	OxMgrCasUiInputString_t		stString;
	OxMgrCasUiInputSelection_t	stSelection;
} OxMgrCasUiInputData_t;



#define ______________INPUT_NOTIFY________________
typedef struct
{
	OxMgrCasInputButtonType_e	eMgrCasBtnType;
} OxMgrCasUiNotifyButton_t;

// Input : Number
typedef struct
{
	HUINT32 					ulNumber;		// max 4294967295
	HUINT32 					ulNumberLength;	// Length of pszNumber
	HCHAR						pszNumber[MGR_CASINPUT_NUMBER_MAX_LENGTH];
} OxMgrCasUiNotifyNumber_t;

// Input : String
typedef struct
{
	HUINT32 					ulStringLength;
	HCHAR						szString[MGR_CAS_STR_LENGTH_LONG];
} OxMgrCasUiNotifyString_t;

// Input : Selection
// Menu List 등 Item을 선택할 수 있는 UI에서 어떤 Item을 선택했는지 알려주는 메시지..
typedef struct
{
	HUINT32 					ulItemIdx;
	HUINT32 					ulDataLength;						// Item에 달려있는 data length
	HCHAR						aucData[MGR_CAS_STR_LENGTH_SHORT];	// Item에 달려있는 data
} OxMgrCasUiNotifySelection_t;
typedef enum
{
	eMgrCasEtc_Previous			= 0x00000001,
	eMgrCasEtc_Exit 			= 0x00000002,
} OxMgrCasUiNotifyEtcType_e;

typedef struct
{
	OxMgrCasUiNotifyEtcType_e	eMgrCasEtcType;
} OxMgrCasUiNotifyEtc_t;

typedef union
{
	OxMgrCasUiNotifyButton_t	stBtn;
	OxMgrCasUiNotifyNumber_t	stNumber;
	OxMgrCasUiNotifyString_t	stString;
	OxMgrCasUiNotifySelection_t stSelection;
	OxMgrCasUiNotifyEtc_t 		stEtc;
} OxMgrCasUiInputNotify_t;




#define ______________UI_DATA_TYPE_______________
typedef struct
{
	HUINT32 			ulStringLength;
	HCHAR				szString[MGR_CAS_STR_LENGTH_SHORT];
} OxMgrCasUiStringShort_t;

typedef struct
{
	HUINT32 			ulStringLength;
	HCHAR				szString[MGR_CAS_STR_LENGTH_NORMAL];
} OxMgrCasUiStringNormal_t;

typedef struct
{
	HUINT32 			ulStringLength;
	HCHAR				szString[MGR_CAS_STR_LENGTH_LONG];
} OxMgrCasUiStringLong_t;

typedef struct
{
	HUINT32 			ulDataLength;
	HUINT8				aucData[MGR_CAS_DATA_LENGTH_LONG];
} OxMgrCasUiBinaryDataItem_t;

typedef struct
{
	HUINT32 				ulIndex;
	OxMgrCasUiStringLong_t	stString;
} OxMgrCasUiStringBlock_t;

// MSG_OAPI_CASUI_MAIL
// Params : CasType, SlotNum, UiHandle, OxMgrCasUiMail_t
/* Mail Header */
typedef struct
{

	HINT32				nMailIdx;
	HCHAR				strReceivedDate[MGR_CAS_MAIL_RECEIVED_DATE_LENGTH];
	HCHAR				strReceivedTime[MGR_CAS_MAIL_RECEIVED_TIME_LENGTH];
	HINT32				nMailType;

	/** < Attribute */
	OxMgrCasUIAttribute_e	ulAttr;
	HUINT32 				ulTimeoutSecond;
	HUINT32 				ulFontSize;
	HUINT32				ulCoverage;
	HUINT32 				ulPosX;
	HUINT32 				ulPosY;
	HUINT32 				ulWidth;
	HUINT32 				ulHeight;
	HUINT32 				ulBgColor;
	HUINT32 				ulFgColor;
	HUINT32 				ulProgress;

	HBOOL				bRead;
	HBOOL				bIsFp;
	HBOOL				bIsAttributed;
	HUINT32				ulFpType;
	HBOOL				bUseIcon;
	HUINT32				ulMailIconType;


	/** < String */
	HCHAR					szTitle[MGR_CAS_STR_LENGTH_LONG];		/** < 타이틀 영역에 표시될 스트링 */
	HCHAR					szSubTitle[MGR_CAS_STR_LENGTH_LONG]; 	/** < 서브타이틀 영역에 표시될 스트링 */
	HCHAR					szBottom[MGR_CAS_STR_LENGTH_LONG];		/** < 하단 영역에 표시된 스트링 */
} OxMgrCasUiMailHeader_t;

// MSG_OAPI_CASUI_MENULIST
// Params : CasType, SlotNum, UiHandle, OxMgrCasUiMenulist_t *
typedef struct
{
	HUINT32 					ulIndex;
	OxMgrCasListItemAttribute_e	ulAttr;
	OxMgrCasListItemType_e		ulType;
	union
	{
		OxMgrCasUiStringLong_t			stString;
		OxMgrCasUiBinaryDataItem_t		stBinary;
	} utData;
} OxMgrCasUiMenuItem_t;

/* Menu List Header */
typedef struct
{
	/** < Attribute */
	OxMgrCasUIAttribute_e		ulAttr;

	/** < String */
	HCHAR						szTitle[MGR_CAS_STR_LENGTH_LONG];		/** < 타이틀 영역에 표시될 스트링 */
	HCHAR						szSubTitle[MGR_CAS_STR_LENGTH_LONG];	/** < 서브타이틀 영역에 표시될 스트링 */
	HCHAR						szBottom[MGR_CAS_STR_LENGTH_LONG];		/** < 하단 영역에 표시된 스트링 */

	/** < Index */
	HUINT32 					ulFocusIndex;							/** < Cursor 위치 */
	HUINT32 					ulStartIndex;							/** < 화면에 처음으로 위치할 Item Index */
} OxMgrCasUiMenuHeader_t;

// MSG_OAPI_CASUI_GRID
// Params : CasType, SlotNum, UiHandle, OxMgrCasUiGrid_t *
typedef struct
{
	HUINT32 					ulIndex;
	OxMgrCasListItemAttribute_e	ulAttr;
	HUINT32 					ulStringLength;
	HCHAR						szString[MGR_CAS_STR_LENGTH_LONG];		/** < "John|001|Manager|45" */
} OxMgrCasUiGridItem_t;

typedef enum
{
	eMgrCasUiFocusType_Column 	= 0x00000001,
	eMgrCasUiFocusType_Cell 	= 0x00000002,
} OxMgrCasGridFocusType_e;

typedef struct
{
	HUINT32 	ulHorIdx;
	HUINT32 	ulVerIdx;
} OxMgrCasUiPos;

typedef struct
{
	HCHAR		szTitleAlign[MGR_CAS_STR_LENGTH_LONG];		/** < "center|center|center|center" */
	HCHAR		szTitle[MGR_CAS_STR_LENGTH_LONG];			/** < "Name|Number|Title|Age" */
} OxMgrCasUiGridTitle;

/* Grid Header */
typedef struct
{
	OxMgrCasUIAttribute_e		ulAttr;
	HUINT32 					ulNumOfColumn;								/** < Number of Column */
	HCHAR						szTitleAlign[MGR_CAS_STR_LENGTH_LONG];		/** < "center|center|center|center" */
	HCHAR						szTitle[MGR_CAS_STR_LENGTH_LONG];			/** < "Name|Number|Title|Age" */
	HCHAR						szSubTitle[MGR_CAS_STR_LENGTH_LONG];			/** < 서브타이틀 영역에 표시될 스트링 */
	HCHAR						szBottom[MGR_CAS_STR_LENGTH_LONG];			/** < 하단 영역에 표시된 스트링 */
	HCHAR						szItemAlign[MGR_CAS_STR_LENGTH_LONG];		/** < "left|right|center|right" */

	OxMgrCasGridFocusType_e		ulFocusType;							/** Focus Type */
	OxMgrCasUiPos	 			stFocus;									/** < Cursor 위치 */
	OxMgrCasUiPos 				stStartPos; 								/** < 화면에 처음으로 위치할 Cell Position */
} OxMgrCasUiGridHeader_t;

typedef struct
{
	/** < Attribute */
	OxMgrCasUIAttribute_e	ulAttr;
	HUINT32 				ulTimeoutSecond;
	HUINT32 				ulFontSize;
	HUINT32 				ulPosX;
	HUINT32 				ulPosY;
	HUINT32 				ulWidth;
	HUINT32 				ulHeight;
	HUINT32 				ulBgColor;
	HUINT32 				ulFgColor;
	HUINT32 				ulProgress;

	/** < String */
	HCHAR					szTitle[MGR_CAS_STR_LENGTH_LONG];		/** < 타이틀 영역에 표시될 스트링 */
	HCHAR					szSubTitle[MGR_CAS_STR_LENGTH_LONG]; 	/** < 서브타이틀 영역에 표시될 스트링 */
	HCHAR					szBottom[MGR_CAS_STR_LENGTH_NORMAL];		/** < 하단 영역에 표시된 스트링 */
} OxMgrCasUiInputHeader_t;

/* TextBox Header */
typedef struct
{
	/** < Attribute */
	OxMgrCasUIAttribute_e	ulAttr;
	HUINT32 				ulTimeoutMilliSecond;
	OxMgrCasUiPosition_e	ulPosition;
	HUINT32 				ulPosX;
	HUINT32 				ulPosY;
	HUINT32 				ulWidth;
	HUINT32 				ulHeight;
	HUINT32 				ulPrintTime;

	/** < String */
	HCHAR					szTitle[MGR_CAS_STR_LENGTH_LONG];		/** < 타이틀 영역에 표시될 스트링 */
	HCHAR					szSubTitle[MGR_CAS_STR_LENGTH_LONG];	/** < 서브타이틀 영역에 표시될 스트링 */
	HCHAR					szBottom[MGR_CAS_STR_LENGTH_LONG];		/** < 하단 영역에 표시된 스트링 */
} OxMgrCasUiTextboxHeader_t;

#define ____________EVENT_START____________

typedef struct
{
	OxMgrCasUIAttribute_e		ulAttr;
	HUINT32 					ulTimeoutSecond;
	OxMgrCasUiStringLong_t		stString;
} OxMgrCasUiBanner_t;

typedef struct
{
	OxMgrCasUiTextboxHeader_t	stHeader;
	HUINT32 					ulStringLength;
	HCHAR						*pszString;
} OxMgrCasUiTextbox_t;

typedef struct
{
	OxMgrCasUiMailHeader_t		stHeader;
	HUINT32						ulTextLength;
	HCHAR						*pszText;
} OxMgrCasUiMail_t;

typedef struct
{
	OxMgrCasUiMenuHeader_t		stHeader;								/** < Menu List Header*/
	HUINT32 					ulTotalNum; 							/** < 아이템의 개수 */
	OxMgrCasUiMenuItem_t		*astListItem;
} OxMgrCasUiMenulist_t;

typedef struct
{
	OxMgrCasUiMenuHeader_t		stHeader;								/** < Menu List Header 와 동일*/
	HUINT32 					ulTotalNum; 							/** < 아이템의 개수 */
	OxMgrCasUiMail_t			*astListItem;
} OxMgrCasUiMailList_t;


typedef struct
{
	OxMgrCasUiGridHeader_t		stHeader;
	HUINT32 					ulTotalNum; 							/** < Number of Total Item */
	OxMgrCasUiGridItem_t		*astGridItem;
} OxMgrCasUiGrid_t;

typedef struct
{
	OxMgrCasInputType_e			eInputType;
	OxMgrCasUiInputData_t		utInputData;
} OxMgrCasUiInput_t;

#define ____________EVENT_END____________


typedef struct
{
	HUINT32			ulUiHandle;
} OxMgrCasUiClose_t;

typedef struct
{
	OxMgrCasInputType_e			eInputType;
	Handle_t							hSession;
	HUINT32							ulUiHandle;
	OxMgrCasUiInputNotify_t		utData;
} OxMgrCasUiUserInputNotify_t;

typedef struct
{
	DxCAS_MmiSessionType_e			eUiSession;
} OxMgrCasUiSessionStart_t;

typedef struct
{
	DxCAS_MmiSessionType_e			eUiSession;
} OxMgrCasUiSessionStop_t;

typedef struct
{
	HCHAR				szLangCode[4];
} OxMgrCasUiChangeLanguage_t;

// EVT_CASUI_REQ_SESSION_START
// EVT_CASUI_REQ_SESSION_STOP
// eMEVT_CASUI_SESSION_OPENED
typedef struct
{
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32						ulUiHandle; // 이름 변경. Handle_t hUi
	Handle_t						hSession;
} OxMgrCasUiEvtSession_t;


// EVT_CASUI_BANNER_SHOW, EVT_CASUI_BANNER_HIDE
typedef struct
{
	OxMgrCasUiEvtSession_t	stSessionInfo;
	OxMgrCasUiBanner_t		stBanner;
} OxMgrCasUiEvtBanner_t;

// eMEVT_CASUI_TYPE_TEXTBOX_DATA
typedef struct
{
	OxMgrCasUiEvtSession_t	stSessionInfo;
	OxMgrCasUiTextbox_t		stTextbox;
} OxMgrCasUiEvtTextbox_t;

// EVT_CASUI_TYPE_MAIL
typedef struct
{
	OxMgrCasUiEvtSession_t	stSessionInfo;
	OxMgrCasUiMail_t		stMail;
} OxMgrCasUiEvtMail_t;

typedef struct
{
	OxMgrCasUiEvtSession_t	stSessionInfo;
	OxMgrCasUiMailList_t		stMailList;
} OxMgrCasUiEvtMailList_t;


// eMEVT_CASUI_TYPE_MENULIST_DATA
typedef struct
{
	OxMgrCasUiEvtSession_t	stSessionInfo;
	OxMgrCasUiMenulist_t	stMenulist;
} OxMgrCasUiEvtMenulist_t;

// eMEVT_CASUI_TYPE_GRID_DATA
typedef struct
{
	OxMgrCasUiEvtSession_t	stSessionInfo;
	OxMgrCasUiGrid_t		stGrid;
} OxMgrCasUiEvtGrid_t;

// eMEVT_CASUI_TYPE_INPUT_DATA
typedef struct
{
	OxMgrCasUiEvtSession_t	stSessionInfo;
	OxMgrCasUiInputHeader_t	stHeader;
	OxMgrCasUiInput_t		stInputData;
} OxMgrCasUiEvtInput_t;



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/*	CAS Common
 */
#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI)
HERROR	MGR_CAS_Init(void);
HERROR	MGR_CAS_Start(void);
HERROR	MGR_CAS_TotTdtUpdated(void);
HERROR	MGR_CAS_GetCaSystemId(Handle_t hAction, HUINT32 *pucCaSysId);
HERROR	MGR_CAS_OnShowProgramCasControl(Handle_t hAction, Handle_t hSvc, HUINT32 *pulMsg, HUINT32 *pulParam1, HUINT32 *pulParam2, HUINT32 *pulParam3);
HERROR	MGR_CAS_IsSmartcardRemoved(HUINT8 ucSlotIndex, HBOOL *pbRemoved);
HERROR 	MGR_CAS_DefaultSetting(void);

HERROR	MGR_CAS_StartService(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActionType, DxCAS_GroupId_e eCasGroupId, void *pvData);
HERROR	MGR_CAS_StopService(Handle_t hAction, Handle_t hNextSvc, ApiSyncMode_t ulSyncMode);

HBOOL 	MGR_CAS_NeedCasRatingCheck(void);
HBOOL 	MGR_CAS_IsRatingOK(HUINT32 ulActionId, Handle_t hService, DxDeliveryType_e svcDeliveryType, DxRatingAge_e eRatingSetup, DxRatingAge_e eContentRating);
HBOOL 	MGR_CAS_NeedCasRatingWaitCheck(void);
HBOOL 	MGR_CAS_IsWaitParentalRatingState(MgrPg_Info_t *pstPgInfo);

HERROR 	MGR_CAS_GetRatingFromCasMsg(void *pvCasMsg, HUINT32 *pulRating);
HERROR 	MGR_CAS_IsSameCasIdInSc(HUINT8 ucSlotId, HUINT16 usCaSysId, HUINT8 *pbSame);


#else
#define MGR_CAS_StartService(hAction, hSvc, eActionType, eCasGroupId, pvData)		SVC_CAS_StartService(hAction, hSvc, eActionType, NULL)
#define MGR_CAS_StopService(hAction, hNextSvc, ulSyncMode)							SVC_CAS_StopService(hAction, hNextSvc, ulSyncMode)
#endif

#endif /* __MGR_CAS_H__ */

